/*
 * Simple pc terminal in C
 */

#include "pc_terminal.h"
#include <pthread.h>

#define JS_MESSAGE_INTERVAL_US	10000

char demo_running = 1;
int jsDetected = 0;
char enable_JS = 1;

/* jsMsgThreadFunc()
 * Author: 		Sergio Soto
 * Function: 	Dedicated thread to send Joystick messages
*/
void *jsMsgThreadFunc(void *arg) 
{
	while(demo_running)
	{
		#ifndef JS_DETECTION_OVERRIDE
		if(jsDetected) {
		#endif
			if (boardCurrState==S6_RAW)
			{
				usleep(JS_MESSAGE_INTERVAL_US);
			}
			usleep(JS_MESSAGE_INTERVAL_US);
			if(enable_JS)
				sendMessagePc(MSG03_JOYSTICK);
		#ifndef JS_DETECTION_OVERRIDE
		}
		#endif
	}

	return NULL;
}

/* jsInpThreadFunc()
 * Author: 		Sergio Soto
 * Function: 	Dedicated thread to read Joystick messages
*/
void *jsInpThreadFunc(void *arg) 
{
	while(demo_running)
	{
		#ifndef JS_DETECTION_OVERRIDE
		if(jsDetected) {
		#endif
			usleep(JS_MESSAGE_INTERVAL_US);
			#ifndef JS_DETECTION_OVERRIDE
			jsInput();
			#endif
		#ifndef JS_DETECTION_OVERRIDE
		}
		#endif
	}

	return NULL;
}

/* jsInpThreadFunc()
 * Author: 		Sergio Soto
 * Function: 	Main function on PC side to read Keyboard input and UART input and act on those inputs.
*/

int main(int argc, char **argv){

	int c;
	int i;
	pthread_t jsMsgThread;
	pthread_t jsInpThread;
	
	setRedisConnection();
	initInterface(); //Initialize the User Interface
	dispMsg("Welcome to Tequila Drone!");

	/* Initialize Serial interface */
	dispMsg("Opening serial interface...");
	rs232_open();
	dispMsg("Serial interface connected.");

	/* Initialize Joystick */	
	dispMsg("Detecting Joystick..."); 
	jsDetected = initJS();
	
	/*Check whether the Joystick is connected or not */
	if(jsDetected) {
		dispMsg("Joystick detected");
	} else {
		dispMsg("Joystick not detected");
	}

	if(has_key(KEY_UP)) {
		dispMsg("This console doesn't support arrow keys");
	} else {
		dispMsg("This console supports arrow keys");
	}
	
	/* Wait for user input */
	while(waitForUser);

	/* Create 2 pthreads to handle Joystick inputs: One for reading JS, other for sending to ES */
	pthread_create(&jsMsgThread, NULL, jsMsgThreadFunc, NULL);
	pthread_create(&jsInpThread, NULL, jsInpThreadFunc, NULL);

	/* send & receive */
	while(demo_running) {

		reply = redisCommand(context, "GET KEYBOARD");

		if (reply->type != REDIS_REPLY_NIL) {
           
           i = atoi(reply->str);
           freeReplyObject(reply);
           keyInput(i);

           reply = redisCommand(context, "DEL KEYBOARD");
           freeReplyObject(reply);
		} else if ((i = readCommand()) != -1) {
			keyInput(i);
		}


		/* Read the UART input */
		if ((c = rs232_getchar_nb()) != -1) {
			setrxByte(c);
		}
	}

	// Wait for js threads to join
	dispMsg("Waiting for threads...");
	pthread_join(jsMsgThread,NULL);

	// Close the UART channels and User interface.
	dispMsg("Closing serial interface...");
	rs232_close();
	dispMsg("Press any key to exit");
	timeout(-1);
	getch();
	closeInterface();
    redisFree(context);
	return 0;
}
