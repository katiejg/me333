// This module owns the ADC peripheral, used to sense the motor current. The
// interface isense.h should provide functions for a one-time setup of the ADC, to provide
// the ADC value in ADC counts (0-1023), and to provide the ADC value in terms of
// milliamps of current through the motor.

#ifndef ISENS__H__
#define ISENS__H__

#include <xc.h> // processor SFR definitions
#include <sys/attribs.h> // __ISR macro

#include "NU32DIP.h"

// function headers go here

#endif // ISENS__H__

