#include "poscontrol.h"

static volatile float PEint = 0;
static volatile float desired_angle = 0;
static volatile float encoder_angle;
static volatile int num_traj;

// Timer4 Interrupt
void __ISR(_TIMER_4_VECTOR, IPL4SOFT) T4Controller(void) {
	enum Mode opmode = get_mode();	// get current mode
	// ensure it is in hold
	switch (opmode) {
		case HOLD: {
			pid_controller(desired_angle);
			break;
		}
		case TRACK: {
			static int ind = 0;
			actTraj[ind] = 360.0 * (read_encoder() / 1000.0);
			pid_controller(get_reftraj(ind));
			ind++;
			// collect motor angle data for plotting
			if (ind == num_traj) {
				StoringData = 0;
				set_angle(get_reftraj(num_traj - 1));
				set_mode(IDLE);
			}
			break;
		}
		default:
			break;
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
	IPC4bits.T4IP = 4;				// priority
	IPC4bits.T4IS = 0;				// subpriority
	IFS0bits.T4IF = 0;				// clear interrupt flag
	IEC0bits.T4IE = 1;				// enable
	__builtin_enable_interrupts();	// enable interrupts
	T4CONbits.ON = 1;				// turn on Timer5
}

void pid_controller(float stept) {
	// read encoder
	encoder_angle =
		360.0 * (read_encoder() / 1000.0);	// calculate degrees,
											// assuming 4x decoupling
	// calculate ref current using PID control gains
	static int eprev = 0;
	// compare actual angle to desired angle
	float error = encoder_angle - stept;
	float edot = error - eprev;	 // error difference
	PEint = PEint + error;		 // error sum
	// integrator anti-windup
	if (PEint > PEINTMAX) {
		PEint = PEINTMAX;
	} else if (PEint < -PEINTMAX) {
		PEint = -PEINTMAX;
	}
	float kpgain = get_kpp(), kigain = get_kip(), kdgain = get_kdp();
	float u = (kpgain * error) + (kigain * PEint) + (kdgain * edot);
	eprev = error;
	// u is now the ref current
	set_holdCurrent(u);
}

void set_angle(float angledeg) { desired_angle = angledeg; }

void set_numtraj(int n) { num_traj = n; }

int get_numtraj() { return num_traj; }
void set_reftraj(int n, float val) { refTraj[n] =  val; }
float get_reftraj(int n) { return refTraj[n]; }
float get_acttraj(int n) { return actTraj[n]; }

void set_pgains(float kp, float ki, float kd) {
	kpp = kp;
	kip = ki;
	kdp = kd;
}

float get_kpp() { return kpp; }

float get_kip() { return kip; }

float get_kdp() { return kdp; }