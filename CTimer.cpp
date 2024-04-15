#include "HardwareSerial.h"
#include "CTimer.h"
#include <Arduino.h>

#define DEBUG 1
#if DEBUG == 1
#define debug(x) Serial.print(x)
#define debugln(x) Serial.println(x)
#else
#define debug(x)
#define debugln(x)
#endif


CTimer::CTimer(unsigned long duration, U8G2_SSD1309_128X64_NONAME2_F_HW_I2C& displayRef)
  : countdownTime(duration), lastUpdateTime(0), lastSetCountdownTime(duration),
    initialCountdownTime(duration), state(Stopped), display(&displayRef) {}

void CTimer::start() {
  if (state != Running) {
  state = Running;
  debugln("Timer Started");
  }
}

void CTimer::stop() {
  if (state == Running) {
    state = Stopped;
    debugln("Timer Stopped");
  }
}

void CTimer::reset() {
  countdownTime = 60000;
  state = Stopped;
  debugln("Timer Reset");
}

void CTimer::update() {
    if (state == Running && millis() - lastUpdateTime >= 1000) {
    lastUpdateTime = millis();
    if (countdownTime > 0) {
      countdownTime -= 1000;
    } else {
      debugln("Timer Finished");
      stop();
      countdownTime = initialCountdownTime;
      // start(); restart timer immediately
    }
  }
}

void CTimer::addTime(unsigned long additionalTime) {
    if (!this->isRunning()) {
    this->countdownTime += additionalTime;
    debug("Added Time: ");
    debugln(additionalTime);
  }
}

void CTimer::subtractTime(unsigned long subtractTime) {
    if (!this->isRunning() && this->countdownTime > subtractTime) {
    this->countdownTime -= subtractTime;
    debug("Subtracted Time: ");
    debugln(subtractTime);
  }
}

bool CTimer::isRunning() {
  return state == Running;
}

void CTimer::updateLastSetTime(unsigned long time) {
  this->lastSetCountdownTime = time;
  debug("Set Time Updated: ");
  debugln(time);
  }


unsigned long CTimer::getCountdownTime() const {
  return countdownTime;
}

unsigned long CTimer::getLastSetTime() const {
  return lastSetCountdownTime;
}
