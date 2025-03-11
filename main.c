#include "nu32dip.h"
#include "encoder.h"

#define BUF_SIZE 200

int main() {
      char buffer[BUF_SIZE];
      NU32DIP_Startup();
      NU32DIP_GREEN = 1; // turn off the LEDs
      NU32DIP_YELLOW = 1;
      // INA219_Startup(); 
      __builtin_disable_interrupts();
      UART2_Startup();
      // initialize modules or peripherals here ...
      __builtin_enable_interrupts();

      while(1) {
            NU32DIP_ReadUART1(buffer, BUF_SIZE); // next character expect menu command
            NU32DIP_GREEN = 1;
            switch (buffer[0]) {
                  case 'c': { // read encoder count
                        WriteUART2("a"); // 'a' print back encoder count as an integer with a newline
                        while(!get_encoder_flag()) {;} // wait for get_encoder_flag to return 1
                        set_encoder_flag(0); // clear flag
                        char m[50];
                        int p = get_encoder_count();
                        sprintf(m, "%d\r\n",p);
                        NU32DIP_WriteUART1(m); // send encoder count to client
                        break;
                  }
                  case 'd': { // dummy command
                        int n = 0;
                        NU32DIP_ReadUART1(buffer, BUF_SIZE);
                        sscanf(buffer, "%d", &n);
                        sprintf(buffer, "%d\r\n", n+1); // return the number + 1
                        NU32DIP_WriteUART1(buffer);
                        break;
                  }
                  case 'e': { // reset encoder
                        WriteUART2("b"); // reset encoder count, no reply
                        break;
                  }
                  case 'q': { 
                        // hangle q for quit
                        break;
                  }
                  default: {
                        NU32DIP_GREEN = 0; // turn on Green LED to indicate an error
                        break;
                  }
            }
      }

      return 0;
}