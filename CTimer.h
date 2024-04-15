#ifndef CTIMER_H
#define CTIMER_H
#include <Arduino.h>
#include "phmeter.h"
#include "PushButton.h"
//#include "config.h"
#include <U8g2lib.h>  //fonts  https://lopaka.app/
#include <U8x8lib.h>

enum TimerState { Stopped, Running, Finished };

class CTimer {
private:
  unsigned long countdownTime;
  unsigned long lastUpdateTime;
  unsigned long lastSetCountdownTime;
  unsigned long initialCountdownTime;
  TimerState state;
  U8G2_SSD1309_128X64_NONAME2_F_HW_I2C* display;

public:
  CTimer();
  CTimer(unsigned long duration, U8G2_SSD1309_128X64_NONAME2_F_HW_I2C& displayRef);
  void start();
  void stop();
  void reset();
  void update();
  void addTime(unsigned long additionalTime);
  void subtractTime(unsigned long subtractTime);
  bool isRunning();
  void updateLastSetTime(unsigned long time);
  unsigned long getCountdownTime() const;
  unsigned long getLastSetTime() const;
};

#endif  // CTIMER_H
