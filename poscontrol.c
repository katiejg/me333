#include "poscontrol.h"

// Timer4 Interrupt
void __ISR(_TIMER_4_VECTOR, IPL5SOFT) T4Controller(void) {
	enum Mode opmode = get_mode();	// get current mode
	switch (opmode) {
		case HOLD: {
			break;
		}
		default: {
			// not a valid mode
			break;
		}
	}
	IFS0bits.T4IF = 0;	// clear interrupt flag
}

// Initialize Timer4 for 200 Hz ISR / fixed-frequency control loop
void initTimer4() {
	// Set up interrupt for Timer4
	T4CONbits.ON = 0;	  // turn off Timer4
	T4CONbits.TCKPS = 1;  // prescaler, N=8
	// configure Timer4 to call an ISR at a frequency of 200 Hz
	TMR4 = 0;  // reset
	// Freq = 48MHz / (Prescaler * PR4)
	PR4 = 29999;  // PR4 = (48MHz / (Prescaler * 200Hz)) - 1
	__builtin_disable_interrupts();
	INTCONbits.INT4EP = 0;	// external interrupt 2, falling edge trigger
	IPC4bits.T4IP = 5;		// priority
	IPC4bits.T4IS = 0;		// subpriority
	IFS0bits.T4IF = 0;		// clear interrupt flag
	IEC0bits.T4IE = 1;		// enable
	__builtin_enable_interrupts();	// enable interrupts
	T4CONbits.ON = 1;				// turn on Timer5
}

void set_pgains(float kp, float ki, float kd) {
	kpp = kp;
	kip = ki;
	kdp = kd;
}

float get_kpp() { return kpp; }

float get_kip() { return kip; }

float get_kdp() { return kdp; }