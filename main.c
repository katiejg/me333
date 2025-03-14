#include "nu32dip.h"
#include <stdio.h>
#include "util.h"
#include "ina219.h"
#include "encoder.h"
#include "currcontrol.h"

// Ch 28 Part 1

#define BUF_SIZE 200

int read_encoder()
{
      WriteUART2("a"); // 'a' print back encoder count as an integer with a newline
      while (!get_encoder_flag())
      {
            ;
      } // wait for get_encoder_flag to return 1
      set_encoder_flag(0); // clear flag
      int p = get_encoder_count();
      return p;
}

int main()
{
      char buffer[BUF_SIZE];
      char m[50];
      // initialize modules or peripherals
      NU32DIP_Startup();
      UART2_Startup();
      INA219_Startup();
      NU32DIP_GREEN = 1; // turn off the LEDs
      NU32DIP_YELLOW = 1;
      set_mode(IDLE);
      __builtin_disable_interrupts();
      initTimer5();
      initPWMT2OC();
      __builtin_enable_interrupts();

      // menu loop
      while (1)
      {
            NU32DIP_ReadUART1(buffer, BUF_SIZE); // next character expecting menu command
            NU32DIP_GREEN = 1; // turn off green LED
            switch (buffer[0])
            {
            case 'a':
            {                                     // read current sensor adc counts
                  short count = readINA219(0x04);
                  sprintf(m, "%d\r\n", count);
                  NU32DIP_WriteUART1(m); // send adc count to client
                  break;
            }
            case 'b':
            { // read current sensor mA
                  float current = INA219_read_current();
                  sprintf(m, "%.1f\r\n", current);
                  NU32DIP_WriteUART1(m); // send amp count to client
                  break;
            }
            case 'c': // read encoder count
            {
                  int count = read_encoder();
                  sprintf(m, "%d\r\n", count);
                  NU32DIP_WriteUART1(m); // send encoder count to client
                  break;
            }
            case 'd':
            { // read encoder deg
                  int count = read_encoder();
                  float degrees = 360.0 * (count / 1000.0); // calculate degrees, assuming 4x decoupling TO FIX
                  sprintf(m, "%.1f\r\n", degrees);
                  NU32DIP_WriteUART1(m);
                  break;
            }
            case 'e':
            {                      // reset encoder
                  WriteUART2("b"); // reset encoder count, no reply
                  break;
            }
            case 'f':
            {
                  int pf = 0;
                  NU32DIP_ReadUART1(buffer, BUF_SIZE);
                  sscanf(buffer, "%d", &pf);
                  // determine direction
                  if (pf < 0) { // clockwise
                        set_duty_cycle(-1*pf,1);
                  } else { // counterclockwise
                        set_duty_cycle(pf,0);
                  }
                  set_mode(PWM); // set new duty cycle
                  break;
            }
            case 'g': { // set current gains
                  float kpc, kic;
                  NU32DIP_ReadUART1(buffer, BUF_SIZE);
                  sscanf(buffer, "%f", &kpc);
                  NU32DIP_ReadUART1(buffer, BUF_SIZE);
                  sscanf(buffer, "%f", &kic);
                  set_cgains(kpc,kic);
                  break;
            }
            case 'h': { // get current gains
                  float kpc = get_kpc();
                  float kic = get_kic();
                  sprintf(m, "%.2f\r\n", kpc);
                  NU32DIP_WriteUART1(m);
                  sprintf(m, "%.2f\r\n", kic);
                  NU32DIP_WriteUART1(m);
                  break;
            }
            case 'k': { // test current control
                  set_mode(ITEST);
                  break;
            }
            case 'p': // Unpower the motor
            {
                  set_mode(IDLE);
                  break;
            }
            case 'r': // get current mode
            {
                  enum Mode currmode = get_mode();
                  sprintf(m, "%d\r\n", (int)currmode);
                  NU32DIP_WriteUART1(m); // write current mode as enum (python will handle printing)
                  break;
            }
            case 'q':
            {
                  // handle q for quit
                  set_mode(IDLE);
                  break;
            }
            default:
            {
                  NU32DIP_GREEN = 0; // turn on Green LED to indicate an error
                  break;
            }
            }
      }

      return 0;
}