#include "PushButton.h"
#include <Arduino.h>

PushButton::PushButton(int pinSS, int pinMP, int pinMM, int pinRT, bool pullup)
  : _pullup(pullup) {
  _pin[0] = pinSS;
  _pin[1] = pinMP;
  _pin[2] = pinMM;
  _pin[3] = pinRT;
  for (int i = 0; i < 4; i++) {
    _lastDebounceTime[i] = 0;
    _lastButtonState[i] = false;
    _buttonState[i] = false;
  }
}

void PushButton::init() {
  for (int i = 0; i < 4; i++) {
    pinMode(_pin[i], _pullup ? INPUT_PULLUP : INPUT);
  }
}

bool PushButton::readButton(int index) {
  bool reading = digitalRead(_pin[index]) == (_pullup ? LOW : HIGH);
  if (reading != _lastButtonState[index]) {
    _lastDebounceTime[index] = millis();
  }
  if ((millis() - _lastDebounceTime[index]) > DEBOUNCE_DELAY) {
    _buttonState[index] = reading;
  }
  _lastButtonState[index] = reading;
  return _buttonState[index];
}


bool PushButton::isPressedSS() {
  return readButton(0);
}
bool PushButton::isPressedMP() {
  return readButton(1);
}
bool PushButton::isPressedMM() {
  return readButton(2);
}
bool PushButton::isPressedRT() {
  return readButton(3);
}


void PushButton::onSSPressed(void (*callback)()) {
  _onSSPressedCallback = callback;
}
void PushButton::onMPPressed(void (*callback)()) {
  _onMPPressedCallback = callback;
}
void PushButton::onMMPressed(void (*callback)()) {
  _onMMPressedCallback = callback;
}
void PushButton::onRTPressed(void (*callback)()) {
  _onRTPressedCallback = callback;
}

void PushButton::checkButtons() {
  if (isPressedSS() && _onSSPressedCallback != nullptr) {
    _onSSPressedCallback();
  }
  if (isPressedMP() && _onMPPressedCallback != nullptr) {
    _onMPPressedCallback();
  }
  if (isPressedMM() && _onMMPressedCallback != nullptr) {
    _onMMPressedCallback();
  }
  if (isPressedRT() && _onRTPressedCallback != nullptr) {
    _onRTPressedCallback();
  }
}



