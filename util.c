#include "util.h"
#include <stdio.h>

volatile enum Mode currentMode;

void set_mode(enum Mode setting) {
      currentMode = setting;
}

enum Mode get_mode() {
      return currentMode;
}