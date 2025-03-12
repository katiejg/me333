// This module implements the 5 kHz current control loop. It owns a timer
// to implement the fixed-frequency control loop, an output compare and another timer to
// implement a PWM signal to the H-bridge, and one digital output controlling the motor’s
// direction (see the description of the DRV8835 in Chapter 27). Depending on the PIC32
// operating mode, the current controller either brakes the motor (IDLE mode), implements
// a constant PWM (PWM mode), uses the current control gains to try to provide a current specifed by the position controller (HOLD or TRACK mode), or uses the current control
// gains to try to track a 100 Hz ±200 mA square wave reference (ITEST mode). The
// interface currentcontrol.h should provide functions to initialize the module, receive a
// fxed PWM command in the range [−100, 100], receive a desired current (from the
// positioncontrol module), receive current control gains, and provide the current
// control gains

#ifndef CC__H__
#define CC__H__

#include <xc.h> // processor SFR definitions
#include <sys/attribs.h> // __ISR macro

#include "NU32DIP.h"
#include "util.h"

// function headers go here
void __ISR(_TIMER_5_VECTOR, IPL5SOFT) Controller(void);
void initTimer5();
void initPWMT2OC();

#endif // CC__H__
