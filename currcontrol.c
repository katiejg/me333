#include "currcontrol.h"

// clockwise = negative = 1
// counterclockwise = positive = 0

// Timer5 Interrupt
void __ISR(_TIMER_5_VECTOR, IPL5SOFT) T5Controller(void) {
	// motor direction digital output
	DIGOUTLAT = direction;
	enum Mode opmode = get_mode();	// get current mode
	switch (opmode) {
		case IDLE: {
			// put H-bridge in brake mode (duty cycle = 0, ground both pins,
			// enable = 0)
			OC1RS = 0;
			break;
		}
		case PWM: {
			// set duty cycle
			OC1RS = duty_cycle * 2400;	// duty cycle = OC1RS/(PR2+1)
			break;
		}
		case ITEST: {
			static int counter = 0;	 // initialize counter once
			static int plotind = 0;	 // index for data arrays
			static int decctr = 0;	// counts to store data one every decimation
			static int ival = 0;

			// PI Controller
			ival = INA219_read_current();  // read current value
			static int eprev = 0;
			int s = ival;
			int r = Waveform[counter];
			int e = r - s;		 // calculate the error
			int edot = e - eprev;	 // error difference
			Eint = Eint + e;		 // error sum
			// integrator anti-windup
			if (Eint > EINTMAX) {
				Eint = EINTMAX;
			} else if (Eint < -EINTMAX) {
				Eint = -EINTMAX;
			}
			float u = (kpc * e) + (kic * Eint);
			eprev = e;

			// center u at 50, saturate at the ends
			float unew = u + 50.0;
			if (unew > 100.0) {
				unew = 100.0;
			} else if (unew < 0.0) {
				unew = 0.0;
			}

			// determine direction bit and set new duty cycle
			if (u < 0) {  // clockwise
				set_duty_cycle(unew, 1);
			} else {  // counterclockwise
				set_duty_cycle(unew, 0);
			}

                  OC1RS = duty_cycle * 2400;	// duty cycle = OC1RS/(PR2+1)

			if (StoringData) {
				decctr++;
				if (decctr == DECIMATION) {
					decctr = 0; // reset
					dataCurrent[plotind] = ival;  // store data
					refCurrent[plotind] = Waveform[counter];
					plotind++;	// increment plot data index
				}
				if (plotind == PLOTPTS) {
					plotind = 0;
					StoringData = 0;
				}
			}

			counter++;
			if (counter == NUMSAMPS) {
				counter = 0;	 // rollover
				set_mode(IDLE);	 // FIX
			}

			break;
		}
		case HOLD: {
			break;
		}
		default: {
			// not a valid mode
			break;
		}
	}
	IFS0bits.T5IF = 0;	// clear interrupt flag
}

// Initialize Timer5 for 5kHz ISR / fixed-frequency control loop
void initTimer5() {
	// RB10 as output to control motor direction
	ANSELB = 0;		 // turn off any analog input just in case
	DIGOUTTRIS = 0;	 // set as output
	DIGOUTLAT = 0;	 // initial direction
	// Set up interrupt for Timer5
	T5CONbits.ON = 0;	  // turn off Timer5
	T5CONbits.TCKPS = 0;  // prescaler, N=1
	// configure Timer5 to call an ISR at a frequency of 5 kHz
	TMR5 = 0;  // reset
	// Freq = 48MHz / (Prescaler * PR5)
	PR5 = 9599;	 // PR5 = (48MHz / (Prescaler * 5000Hz)) - 1
	__builtin_disable_interrupts();
	INTCONbits.INT2EP = 0;	// external interrupt 2, falling edge trigger
	IPC5bits.T5IP = 5;		// priority
	IPC5bits.T5IS = 0;		// subpriority
	IFS0bits.T5IF = 0;		// clear interrupt flag
	IEC0bits.T5IE = 1;		// enable
	__builtin_enable_interrupts();	// enable interrupts
	T5CONbits.ON = 1;				// turn on Timer5
}

void initPWMT2OC() {
	OC1CON = 0;			   // turn off for now
	RPB7R = 0b0101;		   // set RPB7 to be OC1
	TRISBbits.TRISB7 = 0;  // set as output
	// Use Timer2
	T2CONbits.ON = 0;	  // Turn off Timer2
	T2CONbits.TCKPS = 0;  // Set prescaler, N=1
	// 20 kHz PWM
	PR2 = 2399;					 // PR2 = (48MHz / (1 * 20000Hz)) - 1
	TMR2 = 0;					 // Initialize count to zero
	OC1CONbits.OCM = 0b110;		 // PWM with no fault pin
	OC1RS = (int)(0.25 * 2400);	 // duty cycle = OC1RS/(PR2+1) = 25%
	OC1R = 600;
	OC1CONbits.OCTSEL = 0;	// Timer2 is base
	T2CONbits.ON = 1;		// Turn on Timer2
	OC1CONbits.ON = 1;		// Turn on OC1
}

void set_duty_cycle(int percent, int inputDir) {
	direction = inputDir;				  // set direction
	duty_cycle = (float)percent / 100.0;  // set duty cycle
}

void set_cgains(float kp, float ki) {
	Eint = 0;
	kpc = kp;
	kic = ki;
}

float get_kpc() { return kpc; }

float get_kic() { return kic; }

void make_waveform() {
	int i = 0, A = 200;	 // square wave, center is 0
	for (i = 0; i < NUMSAMPS; ++i) {
		if (i < NUMSAMPS / 2) {
			Waveform[i] = A;
		} else {
			Waveform[i] = -1 * A;
		}
	}
}
