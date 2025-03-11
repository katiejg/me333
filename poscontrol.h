// This module owns a timer to implement the 200 Hz position control
// loop. The interface positioncontrol.h should provide functions to initialize the module,
// load a trajectory from the client, load position control gains from the client, and send
// position control gains back to the client.

#ifndef PC__H__
#define PC__H__

#include <xc.h> // processor SFR definitions
#include <sys/attribs.h> // __ISR macro

#include "NU32DIP.h"

// function headers go here

#endif // PC__H__