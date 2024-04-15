#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1351.h>
#include <U8g2lib.h>  //fonts  https://lopaka.app/
#include <U8x8lib.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include "PHMeter.h"
#include "CTimer.h"
#include "PushButton.h"
//#include "config.h"

#define DEBUG 1
#if DEBUG == 1
#define debug(x) Serial.print(x)
#define debugln(x) Serial.println(x)
#else
#define debug(x)
#define debugln(x)
#endif

#define SCLK 52 //yellow
#define MOSI 51 //blue
#define DC_PIN 9 //green
#define CS_PIN 53 //orange
#define RST_PIN 8 //white

#define PH_PIN A0
#define TDS_PIN A1
#define TEMP_PIN A2

#define I2C_SDA 20
#define I2C_SCL 21
//#define I2nd_SCL 21
//#define I2nd_SDA 20

#define BUTTON_START_STOP 30
#define BUTTON_MIN_PLUS 31
#define BUTTON_MIN_MINUS 32
#define BUTTON_RESET 33
#define BUZZER_PIN 25
#define chipSelect 10

#define BLACK 0x0000

bool calibrationButtonPressed = false;

constexpr int OLED_SCREEN_WIDTH = 128;
constexpr int OLED_SCREEN_HEIGHT = 128;
constexpr int I2C_SCREEN_WIDTH = 128;
constexpr int I2C_SCREEN_HEIGHT = 64;

//U8G2_SSD1309_128X64_NONAME2_F_HW_I2C i2nd(U8G2_R0, U8X8_PIN_NONE, I2nd_SCL, I2nd_SDA);
extern const uint16_t RED, ORANGE, WHITE, BLUE;
const int SEGMENT_HEIGHT = SCREEN_HEIGHT / 4;
const int NUMBER_Y_POS = SCREEN_HEIGHT - 10;
constexpr int VISIBLE_SEGENTS = 5;
const long interval = 1000;
static float lastPH = -1;
const int buttonPins[4] = { BUTTON_START_STOP, BUTTON_MIN_PLUS, BUTTON_MIN_MINUS, BUTTON_RESET };
const unsigned long timeAdjustmentAmount = 60000;
float calibrationSlope = 0.0;
float calibrationIntercept = 0.0;

Adafruit_SSD1351 oled = Adafruit_SSD1351(SCREEN_WIDTH, SCREEN_HEIGHT, &SPI, CS_PIN, DC_PIN, RST_PIN);
PHMeter pH(PH_PIN, calibrationSlope, calibrationIntercept, oled, BUTTON_MIN_PLUS, BUTTON_MIN_MINUS);
U8G2_SSD1309_128X64_NONAME2_F_HW_I2C i2c(U8G2_R0, U8X8_PIN_NONE, I2C_SCL, I2C_SDA);

unsigned long previousMillis = 0;
const int tdsPin = A1;
CTimer timer(60000, i2c);

const int pinSS = 30;  // Start/Stop button
const int pinMP = 31;  // Minute Plus
const int pinMM = 32;  // Minute Minus
const int pinRT = 33;  // Reset


PushButton buttons(pinSS, pinMP, pinMM, pinRT, true);

//************************************************************************playBeep
void playBeep() {
  for (int i = 0; i < 3; i++) {
    tone(BUZZER_PIN, 750 + i * 250, 200);
    delay(200);
  }
}

void drawScaleSegment(int segment, int startY, bool topRow);
void drawHighlightBox(float pH, int scaleStartY, int scaleRowHeight, int rowGap);
//***************************************************************************Setup
void setup() {
  Serial.begin(115200);
  Wire.begin();
  oled.begin();
  oled.fillScreen(BLACK);
  i2c.begin();
  pH.begin();
  buttons.init();
  static bool timerStarted = false;
  if (!timerStarted) {
  timer.start();
  timerStarted = true;
  }
  if (calibrationButtonPressed) {
    pH.calibratePH();
  }
  if (!SD.begin(chipSelect)) {
    Serial.println("Initialization failed!");
    return;
  }
  buttons.onSSPressed(onStartStopPressed);
  buttons.onMPPressed(onMinutePlusPressed);
  buttons.onMMPressed(onMinuteMinusPressed);
  buttons.onRTPressed(onResetPressed);
}

//****************************************************************************Loop
void loop() {
  static bool beepPlayed = false;
  buttons.checkButtons();
  timer.update();
  updateDisplay();

  unsigned long currentMillis = millis();
  previousMillis = currentMillis;
  float currentPH = pH.readPH();

  if (currentPH != lastPH) {
  lastPH = currentPH;
    pH.displayPH(currentPH);
    pH.drawPHScale(currentPH);
    }
  Serial.print("Calibrated pH: ");
  Serial.println(currentPH, 2);

  if (!timer.isRunning() && timer.getCountdownTime() == 0 && !beepPlayed) {
    playBeep();
    beepPlayed = true;
  }
  if (timer.isRunning() || timer.getCountdownTime() == 60000) {
    beepPlayed = false;
  }

}

void onStartStopPressed() {
  if (timer.isRunning()) {
    timer.stop();
    Serial.println("Timer Stopped");
  } else {
    timer.start();
    Serial.println("Timer Started");
  }
}

void onMinutePlusPressed() {
  if (!timer.isRunning()) {
    Serial.println("Adding time...");
    timer.addTime(60000);
  }
}

void onMinuteMinusPressed() {
  if (!timer.isRunning() && timer.getCountdownTime() > 60000) {
    Serial.println("Subtracting time...");
    timer.subtractTime(60000);
  }
}

void onResetPressed() {
  timer.reset();
}



//*******************************************************************updateDisplay
void updateDisplay() {
  i2c.clearBuffer();

  unsigned long lastSetTime = timer.getLastSetTime();
  int setMinutes = lastSetTime / 60000;
  int setSeconds = (lastSetTime % 60000) / 1000;

  char buffer[20];
  sprintf(buffer, "Set Time: %02d:%02d", setMinutes, setSeconds);
  i2c.setFont(u8g2_font_profont15_tr);
  i2c.drawStr(5, 10, buffer);

  unsigned long currentTime = timer.getCountdownTime();
  int currentMinutes = currentTime / 60000;
  int currentSeconds = (currentTime % 60000) / 1000;
  sprintf(buffer, "%02d:%02d", currentMinutes, currentSeconds);
  i2c.setFont(u8g2_font_profont29_tr);
  i2c.drawStr(25, 45, buffer);

  i2c.sendBuffer();
}

// void tdsMeter(){
//   int tdsValue = analogRead(tdsPin);
//   Serial.print("TDS: ");
//   Serial.print(tdsValue);
//   Serial.println(" ppm");
//   delay(1000);
//}


// void tempSensor(){

// }
