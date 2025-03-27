#include "util.h"
#include "encoder.h"

// setter for the mode
void set_mode(enum Mode setting) {
      currentMode = setting;
}

// getter for the mode
enum Mode get_mode() {
      return currentMode;
}

int read_encoder() {
	WriteUART2(
		"a");  // 'a' print back encoder count as an integer with a newline
	while (!get_encoder_flag()) {
		;
	}  // wait for get_encoder_flag to return 1
	set_encoder_flag(0);  // clear flag
	int p = get_encoder_count();
	return p;
}