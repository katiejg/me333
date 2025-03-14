#include "currcontrol.h"

// clockwise = negative = 1
// counterclockwise = positive = 0

// Timer5 Interrupt
void __ISR(_TIMER_5_VECTOR, IPL5SOFT) Controller(void)
{
      // motor direction digital output
      DIGOUTLAT = direction;
      enum Mode opmode = get_mode(); // get current mode
      switch (opmode)
      {
      case IDLE:
      {
            // put H-bridge in brake mode (duty cycle = 0, ground both pins, enable = 0)
            OC1RS = 0;
            break;
      }
      case PWM:
      {
            // set duty cycle
            OC1RS = duty_cycle * 2400; // duty cycle = OC1RS/(PR2+1)
            break;
      }
      case ITEST:
      {
            makeRef();
            float current = INA219_read_current();
            break;
      }
      default:
      {
            // not a valid mode
            break;
      }
      }
      IFS0bits.T5IF = 0; // clear interrupt flag
}

// Initialize Timer5 for 5kHz ISR / fixed-frequency control loop
void initTimer5() {
      // RB10 as output to control motor direction
      ANSELB = 0; // turn off any analog input just in case
      DIGOUTTRIS = 0; // set as output
      DIGOUTLAT = 0;   // initial direction
      // Set up interrupt for Timer5
      T5CONbits.ON = 0;    // turn off Timer5
      T5CONbits.TCKPS = 0; // prescaler, N=1
      // configure Timer5 to call an ISR at a frequency of 5 kHz
      TMR5 = 0; // reset
      // Freq = 48MHz / (Prescaler * PR5)
      PR5 = 9599; // PR5 = (48MHz / (Prescaler * 5000Hz)) - 1
      __builtin_disable_interrupts();
      INTCONbits.INT2EP = 0;         // external interrupt 2, falling edge trigger
      IPC5bits.T5IP = 5;             // priority
      IPC5bits.T5IS = 0;             // subpriority
      IFS0bits.T5IF = 0;             // clear interrupt flag
      IEC0bits.T5IE = 1;             // enable
      __builtin_enable_interrupts(); // enable interrupts
      T5CONbits.ON = 1;              // turn on Timer5
}

void initPWMT2OC()
{
      OC1CON = 0; // turn off for now
      RPB7R = 0b0101;       // set RPB7 to be OC1
      TRISBbits.TRISB7 = 0; // set as output
      // Use Timer2
      T2CONbits.ON = 0;    // Turn off Timer2
      T2CONbits.TCKPS = 0; // Set prescaler, N=1
      // 20 kHz PWM
      PR2 = 2399;             // PR2 = (48MHz / (1 * 20000Hz)) - 1
      TMR2 = 0;               // Initialize count to zero
      OC1CONbits.OCM = 0b110; // PWM with no fault pin
      OC1RS = (int)(0.25 * 2400);    // duty cycle = OC1RS/(PR2+1) = 25%
      OC1R = 600;
      OC1CONbits.OCTSEL = 0; // Timer2 is base
      T2CONbits.ON = 1;       // Turn on Timer2
      OC1CONbits.ON = 1;     // Turn on OC1
}

void set_duty_cycle(int percent, int inputDir) {
      direction = inputDir; // set direction
      duty_cycle = (float) percent / 100.0; // set duty cycle
}

void set_cgains(float kp, float ki) {
      kpc = kp;
      kic = ki;
}

float get_kpc() { return kpc; }

float get_kic() { return kic; }

void makeRef() {
      // 200 mA 100 Hz square wave
      volatile int A = 200;
      for (int count=0; count < 100; ++count) {
            if (count % 25 == 0) {
                  A = -1*A;
            }
            refCurrent[count] = A;
      }
      set_mode(IDLE);
}