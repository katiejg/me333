#include "currcontrol.h"

// Initialize Timer5 for 5kHz ISR / fixed-frequency control loop
void initTimer5() {
      // Set up interrupt for Timer5
      T5CONbits.ON = 0; // turn off Timer5
      T5CONbits.TCKPS = 0;
      // configure Timer5 to call an ISR at a frequency of 1 kHz
      TMR5 = 0;
      PR5 = 9599; // PR5 = (48MHz / (1 * 5000Hz)) - 1
      __builtin_disable_interrupts(); 
      INTCONbits.INT4EP = 0; // external interrupt 4, falling edge trigger
      IPC5bits.T5IP = 5;
      IPC5bits.T5IS = 0;
      IFS0bits.T5IF = 0;
      IEC0bits.T5IE = 1;
      __builtin_enable_interrupts();  // step 7: enable interrupts
      T5CONbits.ON = 1; // turn on Timer5
}

void initPWMT2OC() {
      RPB7R = 0b0101; // set RPB7 to be OC1
      TRISBbits.TRISB7 = 0; // set as output
      // Use Timer2
      T2CONbits.ON = 0; // Turn off timer2
      T2CONbits.TCKPS = 0; // Set prescaler, N=1
      PR2 = 1999; // PR2 = (48MHz / (1 * 1000Hz)) - 1
      TMR2 = 0; // Initialize count to zero
      T2CONbits.ON = 1; // Turn on Timer3
      OC1CONbits.OCM = 0b110; // PWM with no fault pin
      OC1RS = 1500; // duty cycle = OC1RS/(PR3+1) = 75%
      OC1R = 1500;
      OC1CONbits.OCTSEL = 0; // Timer2 is base
      OC1CONbits.ON = 1; // Turn on OC1
}