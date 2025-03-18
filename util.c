#include "util.h"

// setter for the mode
void set_mode(enum Mode setting) {
      currentMode = setting;
}

// getter for the mode
enum Mode get_mode() {
      return currentMode;
}