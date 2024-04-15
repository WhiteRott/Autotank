#include "PHMeter.h"
#include <Arduino.h>

#define BLACK oled->color565(0, 0, 0)
#define WHITE oled->color565(255, 255, 255)
#define RED oled->color565(255, 0, 0)
#define ORANGE oled->color565(255, 165, 0)
#define YELLOW oled->color565(255, 255, 0)
#define GREEN oled->color565(0, 255, 0)
#define BLUE oled->color565(0, 0, 255)
#define INDIGO oled->color565(6, 2, 112)
#define LTBLUE oled->color565(81, 96, 253)
#define PEACH oled->color565(255, 218, 185)



PHMeter::PHMeter(int sensorPin, float calibrationSlope, float calibrationIntercept, Adafruit_SSD1351& displayRef, int buttonPinPlus, int buttonPinMinus)
  : pin(sensorPin), slope(calibrationSlope), intercept(calibrationIntercept), oled(&displayRef), buttonPlus(buttonPinPlus), buttonMinus(buttonMinus) {

  calibrationSlope = 0.0;      // Default values
  calibrationIntercept = 7.0;  // Default pH value at neutral


}
uint16_t PHMeter::getColorForPH(float pH) {

  if (pH <= 0) return RED;
  else if (pH <= 1) return blendColor(RED, ORANGE, 0.25);
  else if (pH <= 2) return ORANGE;
  else if (pH <= 3) return blendColor(ORANGE, WHITE, 0.1);
  else if (pH <= 4) return blendColor(ORANGE, WHITE, 0.25);
  else if (pH <= 5) return blendColor(PEACH, WHITE, 0.5);
  else if (pH <= 6) return blendColor(PEACH, WHITE, 0.75);
  else if (pH <= 7) return WHITE;
  else if (pH <= 8) return blendColor(WHITE, BLUE, 0.5);
  else if (pH <= 9) return blendColor(WHITE, BLUE, 0.75);
  else if (pH <= 10) return blendColor(WHITE, BLUE, 0.9);
  else if (pH <= 11) return LTBLUE;
  else if (pH <= 12) return blendColor(LTBLUE, BLUE, 0.75);
  else if (pH <= 13) return BLUE;
  else if (pH >= 14) return blendColor(BLUE, INDIGO, 0.25);
  return BLACK;
  }

uint16_t PHMeter::blendColor(uint16_t colorA, uint16_t colorB, float ratio) {

  int redA = (colorA >> 11) & 0x1F;
  int greenA = (colorA >> 5) & 0x3F;
  int blueA = colorA & 0x1F;

  int redB = (colorB >> 11) & 0x1F;
  int greenB = (colorB >> 5) & 0x3F;
  int blueB = colorB & 0x1F;

  int redBlended = (int)(redA * (1 - ratio) + redB * ratio) & 0x1F;
  int greenBlended = (int)(greenA * (1 - ratio) + greenB * ratio) & 0x3F;
  int blueBlended = (int)(blueA * (1 - ratio) + blueB * ratio) & 0x1F;

  return oled->color565(redBlended, greenBlended, blueBlended);
  }


void PHMeter::begin() {
  pin = pin;
  checkCalibration();
  pinMode(buttonPlus, INPUT_PULLUP);
  pinMode(buttonMinus, INPUT_PULLUP);
  if (!loadCalibration(SLOPE_FILE.c_str(), INTERCEPT_FILE.c_str())) {
  Serial.println("Calibration load failed, using defaults.");
  Serial.println("Calibration load failed, using defaults.");
  }
}

void PHMeter::checkCalibration() {
  oled->fillScreen(BLACK);
  oled->setTextColor(WHITE);
  oled->setTextSize(2);
  oled->setCursor(0, 0);
  oled->println("Calibrate?");
  oled->println("Yes: +, No: -");
}

void PHMeter::calibratePH() {
}

void PHMeter::saveCalibration() {

  File file = SD.open(SLOPE_FILE, FILE_WRITE);
  if (file) {
    file.println(slope);
    file.close();
  } else {
    Serial.println("Failed to open slope file for writing.");
  }

  file = SD.open(INTERCEPT_FILE, FILE_WRITE);
  if (file) {
    file.println(intercept);
    file.close();
  } else {
    Serial.println("Failed to open intercept file for writing.");
  }
}

void PHMeter::loadCalibration(const char *slopeFile, const char *interceptFile) {
  bool loaded = false;
  calibrationSlope = loadAndValidate(slopeFile, -0.1, 0.1, 0.0);
  calibrationIntercept = loadAndValidate(interceptFile, 6.0, 8.0, 7.0);

  File file = SD.open(SLOPE_FILE);
  if (file) {
    slope = file.parseFloat();
    file.close();
    bool success = true;
  } else {
    
    Serial.println("Failed to open slope file for reading.");
  }

  file = SD.open(INTERCEPT_FILE);
  if (file) {
    intercept = file.parseFloat();
    file.close();
    bool success = true;
  } else {
    Serial.println("Failed to open intercept file for reading.");
  }
}


float PHMeter::loadAndValidate(const char *filename, float minVal, float maxVal, float defaultValue) {
  File file = SD.open(filename);
  if (file) {
    float value = file.parseFloat();
    file.close();
    if (value >= minVal && value <= maxVal) {
      
    } else {
      Serial.print("Value out of range: ");
      Serial.println(value);
    }
  } else {
    Serial.print("Failed to open ");
    Serial.println(filename);
  }
  return defaultValue;

  while (true) {
    if (readButtonState(buttonPlus)) {
      calibratePH();  // Start calibration process
      break;
    }
    if (readButtonState(buttonMinus)) {
      break;  // Skip calibration
    }
  }
}

  bool PHMeter::readButtonState(int buttonPin) {
  return digitalRead(buttonPin) == LOW;
}

float PHMeter::readPH() {
  int buf[10], temp;
  for (int i = 0; i < 10; i++) {
    buf[i] = analogRead(pin);
    delay(10);
  }
  for (int i = 0; i < 9; i++) {
    for (int j = i + 1; j < 10; j++) {
      if (buf[i] > buf[j]) {
        temp = buf[i], buf[i] = buf[j], buf[j] = temp;
      }
    }
  }
  float phValue = 0;
  for (int i = 2; i < 8; i++) phValue += buf[i];
  phValue /= 6.0;  // Average the middle values

  phValue = (phValue * slope) + intercept;  // Apply two-point calibration
  return phValue;
}

void PHMeter::displayPH(float pH) {
  oled->fillScreen(BLACK);
  oled->setTextColor(WHITE);
  oled->setTextSize(2);
  oled->setCursor(0, 0);
  oled->print("pH: ");
  oled->println(pH, 2);
}


void PHMeter::drawPHScale(float currentPH) {

  int segmentWidth = SCREEN_WIDTH / VISIBLE_SEGMENTS;
  int pHScaleHeight = SCREEN_HEIGHT / 4;
  int yStart = SCREEN_HEIGHT / 2;
  int firstVisiblePH = (int)currentPH - (VISIBLE_SEGMENTS / 2);

  for (int i = 0; i < VISIBLE_SEGMENTS; i++) {
    float pHValue = firstVisiblePH + i;
    uint16_t color = getColorForPH(pHValue);
    int x = i * segmentWidth;

  // Draw the segment
  oled->fillRect(x, yStart, segmentWidth, pHScaleHeight, color);
  // Draw the pH number below the segment
  oled->setCursor(x + (segmentWidth / 2), yStart + pHScaleHeight + 10);
  oled->setTextColor(WHITE, BLACK);
  oled->setTextSize(1);
  oled->print((int)pHValue);
  }

// Highlight the current pH segment
  int highlightX = 2 * segmentWidth;
  oled->drawRect(highlightX, yStart, segmentWidth, pHScaleHeight, BLACK);
}


