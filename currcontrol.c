#include "currcontrol.h"

// clockwise = negative = 1
// counterclockwise = positive = 0

volatile int StoringData = 0; // define initial value for Storing Data

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
			INA219_Startup();
			char m[50]; // for debugging
			static int counter = 0;	 // initialize counter once
			static volatile int ampl = 200; // amplitude of the reference square wave

			// create the reference square wave
			if (counter % 25 == 0) { // switches signs every 25th count (making it a 100Hz sq wave)
				ampl = -1*ampl;
			}
		
			refCurrent[counter] = ampl; // save in refCurrent

			// PI Controller
			float ival = INA219_read_current();
			// sprintf(m, "we're okay %.2f\r\n", ival);
			// NU32DIP_WriteUART1(m);
			dataCurrent[counter] = ival;  // read current value, save in dataCurrent
			static float eprev = 0;
			float s = dataCurrent[counter];
			float r = refCurrent[counter];
			float e = r - s;		 // calculate the error
			float edot = e - eprev;	 // error difference
			Eint = Eint + e;		 // error sum
			// integrator anti-windup
			if (Eint > EINTMAX) {
				Eint = EINTMAX;
			} else if (Eint < -EINTMAX) {
				Eint = -EINTMAX;
			}
			// get gains
			float kpgain = get_kpc();
			float kigain = get_kic();
			float u = (kpgain * e) + (kigain * Eint); // calculate u
			eprev = e; // set current e as previous e for next iteration

			// center u at 0, saturate at the ends to ensure it is a possible duty cycle
			if (u > 100.0) {
				u = 100.0;
			} else if (u < -100.0) {
				u = -100.0;
			}

			// determine direction bit and set new duty cycle
			if (u < 0) {  // clockwise
				set_duty_cycle(-1*u, 1);
			} else {  // counterclockwise
				set_duty_cycle(u, 0);
			}
			
			// turn on PWM
                  OC1RS = duty_cycle * 2400;	// duty cycle = OC1RS/(PR2+1)

			counter++;
			// if finished graphing
			if (counter > COUNTMAX) {
				StoringData = 0; // reset
				counter = 0; // reset
				set_mode(IDLE); // set mode to IDLE
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

float get_ref(int index) {
	return refCurrent[index];
}
float get_actual(int index) {
	return dataCurrent[index];
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

