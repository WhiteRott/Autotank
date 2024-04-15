#ifndef PHMETER_H
#define PHMETER_H
#include <Arduino.h>
#include "CTimer.h"
#include "PushButton.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1351.h>
//#include "config.h"
static const String SLOPE_FILE = "/slope.txt";
static const String INTERCEPT_FILE = "/intercept.txt";

constexpr int SCREEN_WIDTH = 128;  // Assuming SSD1351 128x128
constexpr int SCREEN_HEIGHT = 128;
constexpr int VISIBLE_SEGMENTS = 5;

class PHMeter {
public:
  PHMeter(int, float, float, Adafruit_SSD1351 &, int, int);
  void begin();
  void checkCalibration();
  float readPH();
  void displayPH(float);
  void drawPHScale(float);
  uint16_t getColorForPH(float pH);
  uint16_t blendColor(uint16_t colorA, uint16_t colorB, float ratio);
  void calibratePH();
  void saveCalibration();
  bool loadCalibration(const char *slopeFile, const char *interceptFile);
  float loadAndValidate(const char *filename, float minVal, float maxVal, float defaultValue);
  bool readButtonState(int buttonPin);
  static const int sensorValueNeutral = 400;
  static const int sensorValueAcid = 259;

private:
  int pin;
  float slope;
  float intercept;
  Adafruit_SSD1351 *oled;
  int buttonPlus;
  int buttonMinus;
};

#endif
