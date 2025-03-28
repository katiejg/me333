// This module implements the 5 kHz current control loop. It owns a timer
// to implement the fixed-frequency control loop, an output compare and another
// timer to implement a PWM signal to the H-bridge, and one digital output
// controlling the motor’s direction (see the description of the DRV8835 in
// Chapter 27). Depending on the PIC32 operating mode, the current controller
// either brakes the motor (IDLE mode), implements a constant PWM (PWM mode),
// uses the current control gains to try to provide a current specifed by the
// position controller (HOLD or TRACK mode), or uses the current control gains
// to try to track a 100 Hz ±200 mA square wave reference (ITEST mode). The
// interface currentcontrol.h should provide functions to initialize the module,
// receive a fxed PWM command in the range [−100, 100], receive a desired
// current (from the positioncontrol module), receive current control gains, and
// provide the current control gains

#ifndef CC__H__
#define CC__H__

#include <sys/attribs.h>  // __ISR macro
#include <xc.h>			  // processor SFR definitions

#include "NU32DIP.h"
#include "util.h"
#include "ina219.h"

// our digital output pins
#define DIGOUTLAT LATBbits.LATB15
#define DIGOUTTRIS TRISBbits.TRISB15
#define COUNTMAX 99  // num of points
#define EINTMAX 2000  // Ki*EINTMAX should be no more than the max control effort

extern volatile int StoringData; // If this flag = 1, currently storing plot data

// set as volatile
static volatile float duty_cycle = 0;
static volatile int direction = 0;
static volatile float refCurrent[COUNTMAX];
static volatile float dataCurrent[COUNTMAX];
static volatile float Eint = 0;
static volatile float kpc = 0, kic = 0;

// function headers go here
void initTimer5();								 // initialize Timer5
void initPWMT2OC();								 // initialize PWM
void set_duty_cycle(int percent, int inputDir);	 // set duty cycle and direction
void pi_controller(int counter);
void set_holdCurrent(float hc);
float get_ref(int index);
float get_actual(int index);
void set_cgains(float kp, float ki);
float get_kpc();
float get_kic();

#endif	// CC__H__