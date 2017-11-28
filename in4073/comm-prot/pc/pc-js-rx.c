
/* pc-js-rx.c
 * Dictionary of commands to be received from the Joystick
 */


#include "pc_terminal.h"
#include "uart-sm.h"
#include "drone-state.h"
#include "pc-js-rx.h"

#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#define JS_CONSTANT 	32767
#define MAX_MOTOR_VAL	1000
#define MAX_JS_VAL		250
#define MAX_LIFT_VAL	500
#define JS_DEV	"/dev/input/js0"

int fd;
struct js_event js;

int16_t jsInputRoll = 0;
int16_t jsInputPitch = 0;
int16_t jsInputYaw = 0;
int16_t jsInputLift = 0;

/* initJS()
 * Author: 		Prashanth G L
 * Function: 	Initialize Joystick: Open the port and make it non-blocking if JS is connected, else return 0;
*/

int initJS(){

	if ((fd = open(JS_DEV, O_RDONLY)) < 0) {
		
		return 0;
	}

	/* non-blocking mode  */
	fcntl(fd, F_SETFL, O_NONBLOCK);

	return 1;
}

/* jsInput()
 * Author: 		Prashanth G L
 * Function: 	Read input from the Joystick, scale them and put it in a buffer
*/
void jsInput() {

/*  Joystick mapping(Range: -32767 to 32767):

	1. Joystick throttle up/dn: lift up/down
	2. Joystick left/right: roll up/down
	3. Joystick forward/backward:pitch down/up
	4. Joystick twist clockwise/counter-clockwise: yaw up/down
	5. Joystick fire button:  abort/exit
*/

//Read the Joystick values 
	while (read(fd, &js, sizeof(struct js_event)) == sizeof(struct js_event)){

			/* register data	 */
			switch(js.type & ~JS_EVENT_INIT) {
				case JS_EVENT_BUTTON:
					button[js.number] = js.value;
					break;
				case JS_EVENT_AXIS:
					axis[js.number] = js.value;
					break;
			}
		}

/*Scale them and store in the respective buffers
 Range of roll, pitch, yaw: -1000 to 1000, lift: 0:700 */

 	jsInputRoll = (MAX_MOTOR_VAL * axis[0]) / JS_CONSTANT;
 	jsInputPitch = (MAX_MOTOR_VAL * axis[1]) / JS_CONSTANT;
 	jsInputYaw =  (MAX_MOTOR_VAL * axis[2]) / JS_CONSTANT;
 	jsInputLift = (MAX_MOTOR_VAL * -axis[3]) / JS_CONSTANT;

 	jsInputLift = (jsInputLift + MAX_MOTOR_VAL) / 2;	
 	if(jsInputLift > 400) {
 		/* Linear increase lift range: 0-400, exponential increase range: 401-700 */
 		jsInputLift = 400 + (jsInputLift - 400) / 2;	
 	}
/* fireButton to put the done in Panic mode */
 	fireButton	= button[0];
}