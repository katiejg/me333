#include "nu32dip.h"
#include "util.h"
#include "encoder.h"

#define BUF_SIZE 200

int read_encoder()
{
      WriteUART2("a"); // 'a' print back encoder count as an integer with a newline
      while (!get_encoder_flag()) {;} // wait for get_encoder_flag to return 1
      set_encoder_flag(0); // clear flag
      int p = get_encoder_count();
      return p;
}

int main()
{
      char buffer[BUF_SIZE];
      char m[50];
      NU32DIP_Startup();
      NU32DIP_GREEN = 1; // turn off the LEDs
      NU32DIP_YELLOW = 1;
      set_mode(IDLE);
      __builtin_disable_interrupts();
      UART2_Startup();
      INA219_Startup();
      // initialize modules or peripherals here ...
      __builtin_enable_interrupts();

      while (1)
      {
            NU32DIP_ReadUART1(buffer, BUF_SIZE); // next character expect menu command
            NU32DIP_GREEN = 1;
            switch (buffer[0])
            {
            case 'c':
            {
                  int count = read_encoder();
                  sprintf(m, "%d\r\n", count);
                  NU32DIP_WriteUART1(m); // send encoder count to client
                  break;
            }
            case 'd':
            { // read encoder deg
                  int count = read_encoder();
                  float degrees = 360.0*(count/1000.0); // calculate degrees, assuming 4x decoupling TO FIX
                  sprintf(m, "%.1f\r\n",degrees); // return the number + 1
                  NU32DIP_WriteUART1(m);
                  break;
            }
            case 'e':
            {                      // reset encoder
                  WriteUART2("b"); // reset encoder count, no reply
                  break;
            }
            case 'p':
            {
                  set_mode(IDLE);
                  break;
            }
            case 'r':
            {
                  enum Mode curr_mode = get_mode();
                  sprintf(m, "%d\r\n", curr_mode); // return the number + 1
                  NU32DIP_WriteUART1(m);
                  break;
            }
            case 'q':
            {
                  // hangle q for quit
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