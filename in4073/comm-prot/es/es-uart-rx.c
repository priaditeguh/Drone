
/* es-uart-rx.c
 * Functions executed when receiving messages from the terminal
 */

#include "es-uart-rx.h"
#include "es-uart-tx.h"
#include "uart-sm.h"
#include "in4073.h"
#include "sm.h"
#include "control.h"
#include <inttypes.h>
#include "data_log.h"

void msg00_status();
void msg01_quit();
void msg02_change_state();
void msg03_joystick();
void msg04_inc_lift();
void msg05_dec_lift();
void msg06_inc_roll();
void msg07_dec_roll();
void msg08_inc_pitch();
void msg09_dec_pitch();
void msg10_inc_yaw();
void msg11_dec_yaw();
void msg12_inc_py();
void msg13_dec_py();
void msg14_inc_p1();
void msg15_dec_p1();
void msg16_inc_p2();
void msg17_dec_p2();
void msg18_write_log();
void msg19_read_log();
void msg20_inc_pheight();
void msg21_dec_pheight();
void msg22_inc_p_i();
void msg23_dec_p_i();
void msg24_inc_p_d();
void msg25_dec_p_d();
int16_t unfoldSI16(int index);
void msg26_joystick_ble();
// Message table
MsgType msgTable[TOTAL_PC_MESSAGES] = {
	/* Action Name */		/* Length */
	{{msg00_status}, 		0},				// 00:	Requests board status
	{{msg01_quit}, 			0},				// 01:	Program termination
	{{msg02_change_state},	1},				// 02:	Request board state change
	{{msg03_joystick}, 		9},			    // 03:	Send Joystick values
	{{msg04_inc_lift},		0},				// 04:	Increment Lift (with keyboard)
	{{msg05_dec_lift},		0},				// 05:	Decrement Lift (with keyboard)
	{{msg06_inc_roll},		0},				// 06:	Increment Roll (with keyboard)
	{{msg07_dec_roll},		0},				// 07:	Decrement Roll (with keyboard)
	{{msg08_inc_pitch},		0},				// 08:	Increment Pitch (with keyboard)
	{{msg09_dec_pitch},		0},				// 09:	Decrement Pitch (with keyboard)
	{{msg10_inc_yaw},		0},				// 10:	Increment Yaw (with keyboard)
	{{msg11_dec_yaw},		0},				// 11:	Decrement Yaw (with keyboard)	
	{{msg12_inc_py},		0},				// 12:	Increment Pyaw (with keyboard
	{{msg13_dec_py},		0},				// 13:	Decrement Pyaw (with keyboard
	{{msg14_inc_p1},		0},				// 14:	Increment P1 (with keyboard)
	{{msg15_dec_p1},		0},				// 15:	Decrement P1 (with keyboard)
	{{msg16_inc_p2},		0},				// 16:	Increment P2 (with keyboard)
	{{msg17_dec_p2},		0},				// 17:	Decrement P2 (with keyboard)
	{{msg18_write_log}, 	0},				// 18: Start logging into flash memory
	{{msg19_read_log}, 		0},				// 19: Send the logging data from flash to PC
/* Commented some parts to avoid out of ram error */
	// {{msg20_inc_pheight},	0},				// 20: Increment Pheight
	// {{msg21_dec_pheight},	0},				// 21: Decrement Pheight
	// {{msg22_inc_p_i},	0},					// 22: increment p_i
	// {{msg23_dec_p_i},	0},					// 23: decrement p_i
	// {{msg24_inc_p_d},	0},					// 24: increment p_d
	// {{msg25_dec_p_d},	0},					// 25: decrement p_d
	{{msg26_joystick_ble}, 4},
};

/* msg00_status()
 * Author: 		Sergio Soto
 * Function: 	Sends pong, just for debugging purpose
*/
void msg00_status() {

	sendMessageEs(MSG00_STATUS_ANS);
}

/* msg01_quit()
 * Author: 		Sergio Soto
 * Function: 	Send the drone shut down message to PC
*/
void msg01_quit() {

	sendMessageEs(MSG01_QUIT_ANS);
	demo_done = true;
}

/* msg02_change_state()
 * Author: 		Sergio Soto
 * Function: 	Changes the state based on a request from PC and transmits the changed state back to PC
*/
void msg02_change_state() {

	int prevSt = smCurrState;
	TransValid result = switchState((StateEs) rxBuff[0]);	// Attempt to change state

	if(result == TRANS_OK) {
		sendMessageEs(MSG02_CURR_STATE);
		if(smCurrState == S8_WIR)
			sendMessageEs(MSG13_STOP_JS);		
		if(prevSt == S8_WIR && smCurrState == S0_SAF)
			sendMessageEs(MSG14_START_JS);		
	} else {
		sendMessageEs(MSG03_INV_STATE_TR);
	}
}

/* msg02_change_state()
 * Author: 		Sergio Soto
 * Function: 	Sets the roll, pitch, yaw, lift which are received from PC
*/
void msg03_joystick() {
	roll = 	unfoldSI16(0);
	pitch = unfoldSI16(2);
	yaw = 	unfoldSI16(4);
	lift = 	unfoldSI16(6);

//Go to panic mode if firebutton is pressed and send the resulting state to PC
	if (rxBuff[8] == 1)
	{
		TransValid result = switchState(S1_PAN);		// Attempt to change state
		if(result == TRANS_OK) {
			sendMessageEs(MSG02_CURR_STATE);
		} else {
			sendMessageEs(MSG03_INV_STATE_TR);
		}
	}
	uartTimeout = 0;

	nrf_gpio_pin_toggle(RED);
}

/* msg04_inc_lift(), msg05_dec_lift(), msg06_inc_roll(), msg07_dec_roll(),msg08_inc_pitch(),
msg09_dec_pitch(), msg10_inc_yaw(), void msg11_dec_yaw()
 * Author: 		Sergio Soto
 * Function: 	Responds to keyboard input for change in lift, roll, pitch and yaw
*/
void msg04_inc_lift(){
	manUpdate(LIFT_UP);
}
void msg05_dec_lift(){
	manUpdate(LIFT_DOWN);
}
void msg06_inc_roll(){
	manUpdate(ROLL_UP);
}
void msg07_dec_roll(){
	manUpdate(ROLL_DOWN);
}
void msg08_inc_pitch(){
	manUpdate(PITCH_UP);
}
void msg09_dec_pitch(){
	manUpdate(PITCH_DOWN);
}
void msg10_inc_yaw(){
	manUpdate(YAW_UP);
}
void msg11_dec_yaw(){
	manUpdate(YAW_DOWN);
}

/* msg12_inc_py(), msg13_dec_py(), msg14_inc_p1(), msg15_dec_p1(), msg16_inc_p2(), msg17_dec_p2()
 * Author: 		Prashanth G L
 * Function: 	Responds to keyboard input for change in constants required in control modes (4,5,6 & 8)
*/
void msg12_inc_py() {
	pYaw += 1;
	sendMessageEs(MSG10_P_CONSTANTS);
}
void msg13_dec_py() {
	pYaw -= 1;
	sendMessageEs(MSG10_P_CONSTANTS);
}
void msg14_inc_p1() {
	p1 += 1;
	sendMessageEs(MSG10_P_CONSTANTS);
}
void msg15_dec_p1() {
	p1 -= 1;
	sendMessageEs(MSG10_P_CONSTANTS);
}
void msg16_inc_p2() {
	p2 += 1;
	sendMessageEs(MSG10_P_CONSTANTS);
}
void msg17_dec_p2() {
	p2 -= 1;
	sendMessageEs(MSG10_P_CONSTANTS);
}

/* msg18_write_log()
 * Author: 		Priadi
 * Function: 	starts logging data to flash for the received keyboard input 'b' from PC
*/

void msg18_write_log(){
	write_log_data();
}

/* msg18_write_log()
 * Author: 		Priadi
 * Function: 	starts sending logged data from flash to PC, for the received keyboard input 'n' from PC
*/
void msg19_read_log(){
	read_log_data(FLASH_START_ADDRESS);
}

/* msg20_inc_pheight(), msg21_dec_pheight(), msg20_inc_p_i(), msg21_dec_p_i(), msg20_inc_p_d(), msg21_dec_p_d()
 * Author: 		Prashanth G L
 * Function: 	Responds to keyboard input for change in constants required in control mode 7
*/

void msg20_inc_pheight(){
	pHeight+=1;
	sendMessageEs(MSG10_P_CONSTANTS);
}
void msg21_dec_pheight(){
	pHeight-=1;
	sendMessageEs(MSG10_P_CONSTANTS);
}
void msg22_inc_p_i(){
	P_I +=1;
	sendMessageEs(MSG10_P_CONSTANTS);
}
void msg23_dec_p_i(){
	P_I -=1;
	sendMessageEs(MSG10_P_CONSTANTS);
}
void msg24_inc_p_d(){
	P_D +=1;
	sendMessageEs(MSG10_P_CONSTANTS);
}
void msg25_dec_p_d(){
	P_D -=1;
	sendMessageEs(MSG10_P_CONSTANTS);
}

/* msg26_joystick_ble()
 * Author: 		Sergio Soto
 * Function: 	Receives the roll, pitch, yaw and lift from bluetooth (smartphone)
*/

void msg26_joystick_ble() {

	roll = 	(int8_t)rxBuff[0] * 8;		// * 4 since originally the range was [0,1000]
	pitch = (int8_t)rxBuff[1] * 8;	    // and now it's [0,250] in order to fit in 8 bits
	yaw = 	(int8_t)rxBuff[2] * 8;
	lift = 	(int8_t)rxBuff[3] * 8;
	uartTimeout = 0;

	nrf_gpio_pin_toggle(YELLOW);
}

/* unfoldSI16(int index)
 * Author: 		Sergio Soto
 * Function: 	Unfolds and stores a signed int16 from the rx buffer to result variable
 * IN:			int index - The starting index in the buffer to store the value
*/

int16_t unfoldSI16(int index) {

	int16_t result = 0;

	result |= (rxBuff[0 + index] & 0xff);
	result |= (rxBuff[1 + index] & 0xff) << 8;
	return result;
}

/* unfoldSI32(int index)
 * Author: 		Sergio Soto
 * Function: 	Unfolds and stores a signed int32 from the rx buffer to result variable
 * IN:			int index - The starting index in the buffer to store the value
*/
int32_t unfoldSI32(int index) {

	int32_t result = 0;

	result |= (rxBuff[0 + index] & 0xff);
	result |= (rxBuff[1 + index] & 0xff) << 8;
	result |= (rxBuff[2 + index] & 0xff) << 16;
	result |= (rxBuff[3 + index] & 0xff) << 24;
	return result;
}
