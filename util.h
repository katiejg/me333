#ifndef UTIL__H__
#define UTIL__H__

#include <xc.h> // processor SFR definitions
#include <sys/attribs.h> // __ISR macro

#include "NU32DIP.h"

enum Mode {
      IDLE,
      PWM,
      ITEST,
      HOLD,
      TRACK
};

void set_mode(enum Mode setting);
enum Mode get_mode();

#endif // UTIL__H__
