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

// our digital output pins
#define DIGOUTLAT LATBbits.LATB15
#define DIGOUTTRIS TRISBbits.TRISB15
#define NUMSAMPS 1000 // num of points in waveform
#define PLOTPTS 200 // num of data points to plot
#define DECIMATION 10 // plot every 10th point
#define EINTMAX 9000 // Ki*EINTMAX should be no more than the max control effort

// set as volatile
static volatile float duty_cycle = 0.25;
static volatile int direction = 0;
static volatile int refCurrent[100];
static volatile int dataCurrent[100];
static volatile int Eint = 0;
static volatile float kpc = 0, kic = 0;
static volatile int count = 0;
static volatile int StoringData = 0; // If this flag = 1, currently storing plot data

// function headers go here
void initTimer5(); // initialize Timer5
void initPWMT2OC(); // initialize PWM
void set_duty_cycle(int percent, int inputDir); // set duty cycle and direction
void set_cgains(float kp, float ki);
float get_kpc();
float get_kic();
void pi_controller();

#endif // CC__H__
