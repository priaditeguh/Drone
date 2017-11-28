#ifndef PC_TERMINAL_H_
#define PC_TERMINAL_H_

#include "rs232_service.h"
#include "uart-sm.h"			
#include "pc-key-rx.h"			
#include "pc-js-rx.h"
#include "interface.h"
#include <string.h>
#include <signal.h>
#include <time.h>
#include <sys/time.h>
#include <ncurses.h>
#include <termios.h>
#include <inttypes.h>
#include <unistd.h>
#include "pc_logging.h"
#include "redis.h"

typedef enum StateEs_t { 
	S0_SAF,							// S0 - Safe mode
	S1_PAN,							// S1 - Panic mode
	S2_MAN,							// S2 - Manual mode
	S3_CAL,							// S3 - Calibration mode
	S4_YAW,							// S4 - Yaw-control mode
	S5_FUL,							// S5 - Full-control mode
	S6_RAW,							// S6 - Raw mode
	S7_HGT,							// S7 - Height-control mode
	S8_WIR,							// S8 - Wireless mode
	NUM_STATES_ES,
} StateEs;

char demo_running;
char enable_JS;

#endif /* PC_TERMINAL_H_ */