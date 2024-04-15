#ifndef PUSH_BUTTON_H
#define PUSH_BUTTON_H
#include <Arduino.h>
#include "phmeter.h"
#include "CTimer.h"
//#include "config.h"

class PushButton {
private:
  static const int DEBOUNCE_DELAY = 50;
  int _pin[4];
  unsigned long _lastDebounceTime[4];
  bool _lastButtonState[4];
  bool _buttonState[4];
  bool _pullup;
  bool readButton(int index);
  void (*_onSSPressedCallback)();
  void (*_onMPPressedCallback)();
  void (*_onMMPressedCallback)();
  void (*_onRTPressedCallback)();

public:
  PushButton();
  PushButton(int pinSS, int pinMP, int pinMM, int pinRT, bool pullup = true);
  void init();
  bool isPressedSS();
  bool isPressedMP();
  bool isPressedMM();
  bool isPressedRT();
  void checkButtons();
  void onSSPressed(void (*callback)());
  void onMPPressed(void (*callback)());
  void onMMPressed(void (*callback)());
  void onRTPressed(void (*callback)());
};

#endif
