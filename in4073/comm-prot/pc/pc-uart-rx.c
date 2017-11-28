
/* pc-uart-rx.c
 * Functions executed when receiving messages from the board
 */

#include "pc-uart-rx.h"
#include "pc_terminal.h"
#include "uart-sm.h"
#include <inttypes.h>
#include "pc_terminal.h"
#include "drone-state.h"

void msg00_status_ans();
void msg01_quit_ans();
void msg02_curr_state();
void msg03_inv_state_tr();
void msg04_disp_motor();
void msg05_sensor_data();
void msg06_adc_data();
void msg07_string();
void msg08_timer_value();
void msg09_rollpitchyaw();
void msg10_p_constants();
void msg11_store_log_data();
void msg12_notify_write_flash();
void msg13_stop_js();
void msg14_start_js();

int16_t unfoldSI16(int index);
int32_t unfoldSI32(int index);
void unfoldString(char * recString);

// Message database
MsgType msgTable[TOTAL_ES_MESSAGES] = {
	/* Action Name */		/* Length */
	{{msg00_status_ans}, 	0},				// 00: Performs some action
	{{msg01_quit_ans}, 		0},				// 01:	Answer program termination
	{{msg02_curr_state}, 	1},				// 02:	Inform terminal of current state
	{{msg03_inv_state_tr}, 	0},				// 03: 	Inform invalid state transition error
	{{msg04_disp_motor}, 	8},				// 04: 	Send motor status
	{{msg05_sensor_data}, 	18},			// 05: 	Sensor and DMP data values
	{{msg06_adc_data},		6},				// 06: 	Temp, Pressure and Battery data values
	{{msg07_string},		20},			// 07:  Sending a string
	{{msg08_timer_value},	4},				// 08: 	Sending timer values (for profiling)
	{{msg09_rollpitchyaw}, 	8},				// 09:	Send Roll Pitch Yaw Lift
	{{msg10_p_constants}, 	10},				// 10: 	Send the values of the P constants
	{{msg11_store_log_data}, TOTAL_LOG_BYTES},	// 11: Receive all logged data from flash memory to PC
	{{msg12_notify_write_flash}, 	0},			// 12: Notify PC after finish writing data into flash memory
	{{msg13_stop_js}, 	0},					// 13: Inform terminal to stop sending Joystick message
	{{msg14_start_js}, 	0},					// 14: Inform terminal to start ending Joystick message	
};

/* msg00_status_ans()
 * Author: 		Sergio Soto
 * Function: 	Displays the debug message
*/
void msg00_status_ans() {
	dispMsg("Pong!");
}

/* msg01_quit_ans()
 * Author: 		Sergio Soto
 * Function: 	Displays the drone shut down message
*/
void msg01_quit_ans() {
	dispMsg("Drone has shut down.");
	demo_running = 0;
}

/* msg02_curr_state()
 * Author: 		Sergio Soto
 * Function: 	Displays the current drone state
*/
void msg02_curr_state() {
	boardCurrState = rxBuff[0];
	dispMsg("The board is now in state %d", boardCurrState);
	dispVal(DISP_MODE, boardCurrState);

	sendModeRedis(boardCurrState);
}


/* msg03_inv_state_tr()
 * Author: 		Sergio Soto
 * Function: 	Displays the invalid state transition request
*/
void msg03_inv_state_tr() {
	dispMsg("Invalid state transition requested!");
}

/* msg04_disp_motor()
 * Author: 		Sergio Soto
 * Function: 	Displays the motor values received from ES
*/
void msg04_disp_motor() {

	int16_t m0 = unfoldSI16(0);
	int16_t m1 = unfoldSI16(2);
	int16_t m2 = unfoldSI16(4);
	int16_t m3 = unfoldSI16(6);
	dispVal(DISP_AE0, m0);
	dispVal(DISP_AE1, m1);
	dispVal(DISP_AE2, m2);
	dispVal(DISP_AE3, m3);

	sendMotorRedis(m0,m1,m2,m3);		

}

/* msg05_sensor_data()
 * Author: 		Sergio Soto
 * Function: 	Displays the sensor values received from ES
*/
void msg05_sensor_data() {

	int16_t s0 = unfoldSI16(0);
	int16_t s1 = unfoldSI16(2);
	int16_t s2 = unfoldSI16(4);
	int16_t s3 = unfoldSI16(6);
	int16_t s4 = unfoldSI16(8);
	int16_t s5 = unfoldSI16(10);
	int16_t s6 = unfoldSI16(12);
	int16_t s7 = unfoldSI16(14);
	int16_t s8 = unfoldSI16(16);
	dispVal(DISP_PHI, s0);
	dispVal(DISP_THE, s1);
	dispVal(DISP_PSI, s2);
	dispVal(DISP_P, s3);
	dispVal(DISP_Q, s4);
	dispVal(DISP_R, s5);
	dispVal(DISP_AX, s6);
	dispVal(DISP_AY, s7);
	dispVal(DISP_AZ, s8);

	sendSensorRedis(s0,s1,s2,s3,s4,s5,s6,s7,s8); 
}

/* msg06_adc_data()
 * Author: 		Prashanth G L
 * Function: 	Displays the pressure, temperature and battery voltage values received from ES
*/
void msg06_adc_data() {
	int16_t a0 = unfoldSI16(0) * 10;  // Batt voltage in mV
	int16_t a1 = unfoldSI16(2);
	uint32_t a2 = (101325 + unfoldSI16(4));
	dispVal(DISP_BATT, a0);
	dispVal(DISP_TEMP, a1);
	dispValP(DISP_PRESS, a2);

	//dispMsg("Telemetry received");

	sendAdcRedis(a0,a1,a2);

}

/* msg07_string()
 * Author: 		Prashanth G L
 * Function: 	Displays the string received from ES
*/
void msg07_string() {
char receivedString[20];
unfoldString(receivedString);
dispMsg(receivedString);

}

/* msg08_timer_value()
 * Author: 		Prashanth G L
 * Function: 	Displays the Timer values received from ES
*/
void msg08_timer_value() {
uint32_t timerValue = (uint32_t) unfoldSI32(0);
dispMsg("Timer value received: %d us", timerValue);
}

/* msg09_rollpitchyaw()
 * Author: 		Prashanth G L
 * Function: 	Displays the current roll, pitch, yaw and lift values received from ES
*/

void msg09_rollpitchyaw() {
	rollBuff = unfoldSI16(0);
	pitchBuff = unfoldSI16(2);
	yawBuff = unfoldSI16(4);
	liftBuff = unfoldSI16(6);
	dispVal(DISP_ROLL, rollBuff);
	dispVal(DISP_PITCH, pitchBuff);
	dispVal(DISP_YAW, yawBuff);
	dispVal(DISP_LIFT, liftBuff);

	sendRPYLRedis(rollBuff,pitchBuff,yawBuff,liftBuff);
}

/* msg10_p_constants()
 * Author: 		Prashanth G L
 * Function: 	Displays the sensor values received from ES
*/
void msg10_p_constants() {
	int16_t a0 = unfoldSI16(0);
	int16_t a1 = unfoldSI16(2);
	int16_t a2 = unfoldSI16(4);
	int16_t a3 = unfoldSI16(6);
	int8_t a4 = rxBuff[8];
	int8_t a5 = rxBuff[9];
	dispMsg("Py: %d\tP1: %d\tP2: %d\tPHgt: %d\tP_I: %d\tP_D: %d", a0, a1, a2, a3, a4, a5);
}

/* msg11_store_log_data()
 * Author: 		Priadi
 * Function: 	Stores the logged data received from ES
*/
void msg11_store_log_data()
{
	static int count_MSG4 = 0;

	if (count_MSG4 == 0)
	{
		file_log = fopen("logfile.dat", "w");
	}

	parsePacketLog();
	storeLogInTxt(file_log);
	count_MSG4++;
	if (count_MSG4 == 8191)
	{
		dispMsg("Receive all logged data to PC done");
		count_MSG4 = 0;
		fclose(file_log);
	}
}

/* msg12_notify_write_flash()
 * Author: 		Priadi
 * Function: 	Notify the logging to text file is done.
*/
void msg12_notify_write_flash()
{
	dispMsg("Log data in flash done");
}

/* msg13_stop_js()
 * Author: 		Priadi
 * Function: 	Disable the joystick to operate in wireless mode
*/
void msg13_stop_js()
{
	dispMsg("Disable Joystick. Switch to Bluetooth App");
	enable_JS = 0;
}

/* msg14_start_js()
 * Author: 		Priadi
 * Function: 	Enable the Joystick to operate in tethered mode
*/
void msg14_start_js()
{
	dispMsg("Enable Joystick");
	enable_JS = 1;
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

/* unfoldString(char * recString)
 * Author: 		Prashanth G L
 * Function: 	Unfolds and stores a string from the rx buffer 
 * IN:			int index - The starting index in the buffer to store the value
*/
void unfoldString(char * recString){
	for(int i=0; i<20; i++)
		recString[i]=rxBuff[i];
}
