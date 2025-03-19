// This module owns a timer to implement the 200 Hz position control
// loop. The interface positioncontrol.h should provide functions to initialize the module,
// load a trajectory from the client, load position control gains from the client, and send
// position control gains back to the client.

#ifndef PC__H__
#define PC__H__

#include <xc.h> // processor SFR definitions
#include <sys/attribs.h> // __ISR macro

#include "NU32DIP.h"
#include "util.h"

static volatile float kpp = 0, kip = 0, kdp = 0;

// function headers go here
void initTimer4();
void set_pgains(float kp, float ki, float kd);
float get_kpp();
float get_kip();
float get_kdp();

#endif // PC__H__