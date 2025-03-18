// This module is used for various bookkeeping tasks. It maintains a variable
// holding the operating mode (IDLE, PWM, ITEST, HOLD, or TRACK) and arrays
// (buffers) to hold data collected during trajectory tracking (TRACK) or current tracking
// (ITEST). The interface utilities.h provides functions to set the operating mode, return
// the current operating mode, receive the number N of samples to save into the data buffers
// during the next TRACK or ITEST, write data to the buffers if N is not yet reached, and to
// send the buffer data to the client when N samples have been collected (TRACK or ITEST
// has completed).

#ifndef UTIL__H__
#define UTIL__H__

#include <xc.h> // processor SFR definitions
#include <sys/attribs.h> // __ISR macro

#include "NU32DIP.h"

// define Mode as enumerated
enum Mode {
      IDLE,
      PWM,
      ITEST,
      HOLD,
      TRACK
};

static volatile enum Mode currentMode; // holds the current mode

void set_mode(enum Mode setting);
enum Mode get_mode();

#endif // UTIL__H__
