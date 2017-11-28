
/* sm.c
 * Board state machine
 * Author: Sergio Soto
 */

#include "sm.h"

StateEs smCurrState = S0_SAF;

TransValid noTrans();
TransValid invTrans();
TransValid stMan();
TransValid stCal();
TransValid stYaw();
TransValid stFul();
TransValid stRaw();
TransValid stHgt();
TransValid stWir();
TransValid panic();
TransValid calDone();
TransValid chkLand();
TransValid diseng();
TransValid stRaw2();

// Machine state transitions from ROW to COL
TransitionEs esSmTable[NUM_STATES_ES][NUM_STATES_ES] = {	
		/*S0_SAF*/	/*S1_PAN*/	/*S2_MAN*/	/*S3_CAL*/	/*S4_YAW*/	/*S5_FUL*/	/*S6_RAW*/	/*S7_HGT*/	/*S8_WIR*/			
/*S0*/	{{noTrans}, {invTrans}, {stMan}, 	{stCal}, 	{stYaw}, 	{stFul}, 	{stRaw}, 	{invTrans}, 	{stWir}},
/*S1*/	{{diseng}, 	{noTrans}, 	{invTrans}, {invTrans}, {invTrans}, {invTrans}, {invTrans}, {invTrans}, {invTrans}},
/*S2*/	{{chkLand}, {panic}, 	{noTrans}, 	{invTrans}, {invTrans}, {invTrans}, {invTrans}, {invTrans}, {invTrans}},
/*S3*/	{{calDone}, {invTrans}, {invTrans}, {noTrans}, 	{invTrans}, {invTrans}, {stRaw2},   {invTrans}, {invTrans}},
/*S4*/	{{chkLand}, {panic}, 	{invTrans}, {invTrans}, {noTrans}, 	{invTrans}, {invTrans}, {invTrans}, {invTrans}},
/*S5*/	{{chkLand}, {panic}, 	{invTrans}, {invTrans}, {invTrans}, {noTrans}, 	{invTrans}, {stHgt},    {invTrans}},
/*S6*/	{{chkLand}, {panic}, 	{invTrans},	{stCal},    {invTrans}, {invTrans}, {noTrans}, 	{invTrans}, {invTrans}},
/*S7*/	{{chkLand}, {panic}, 	{invTrans}, {invTrans}, {invTrans}, {invTrans}, {invTrans}, {noTrans}, 	{invTrans}},
/*S8*/	{{chkLand}, {panic}, 	{invTrans}, {invTrans}, {invTrans}, {invTrans}, {invTrans}, {invTrans}, {noTrans}}
};


/* TransValid noTrans() 
 * Author: 		Sergio Soto
 * Function: 	Informs that the new desired state was the same as the current
 * OUT:			TransValid - The transition outcome
*/
TransValid noTrans() {

	return TRANS_NONE;
}

/* TransValid invTrans() 
 * Author: 		Sergio Soto
 * Function: 	Informs that a transition from the current to the new state is not allowed
 * OUT:			TransValid - The transition outcome
*/
TransValid invTrans() {

	return TRANS_INVALID;
}

/* TransValid stMan() 
 * Author: 		Sergio Soto
 * Function: 	Start manual mode
 * OUT:			TransValid - The transition outcome
*/
TransValid stMan() {
	if(lift > 100 || roll > 100 || pitch > 100 || yaw > 100) {

		smCurrState = S0_SAF;
		return TRANS_INVALID;
	} else {

		smCurrState = S2_MAN;
		return TRANS_OK;
	}
}

/* TransValid stCal() 
 * Author: 		Prashanth G L
 * Function: 	Start calibration mode
 * OUT:			TransValid - The transition outcome
*/
TransValid stCal() {

/* If the transition is occuring from raw mode then set the calibINrawMode variable
 to true to indicate that the calibration is occuring in raw mode and start calibration, 
 else just start calibration.*/
	if (smCurrState==S6_RAW)
	{
		calibINrawMode = true;
	}
	smCurrState = S3_CAL;
	startDmpCal();		//Starts the calibration of MPU6050 sensor module
	return TRANS_OK;
}

/* TransValid stYaw() 
 * Author: 		Sergio Soto
 * Function: 	Start Yaw-controlled mode
 * OUT:			TransValid - The transition outcome
*/
TransValid stYaw() {
	
	// Check if joystick lift is at 0 and drone has been calibrated
	if(lift > 100 || roll > 100 || pitch > 100 || yaw > 100 || isCalibrated == 0) {	

		smCurrState = S0_SAF;
		return TRANS_INVALID;
	} else {

		smCurrState = S4_YAW;
		return TRANS_OK;
	}	
}

/* TransValid stFul() 
 * Author: 		Sergio Soto
 * Function: 	Start Full-controlled mode
 * OUT:			TransValid - The transition outcome
*/
TransValid stFul() {
	// Check if joystick lift is at 0 and drone has been calibrated
	if(lift > 100 || roll > 100 || pitch > 100 || yaw > 100 || isCalibrated == 0) {	

		smCurrState = S0_SAF;
		return TRANS_INVALID;
	} else {

		smCurrState = S5_FUL;
		return TRANS_OK;
	}
}

/* TransValid stRaw() 
 * Author: 		Prashanth G L
 * Function: 	Start Raw-controlled mode
 * OUT:			TransValid - The transition outcome
*/
TransValid stRaw() {
	// Check if joystick lift is at 0 
	if(lift > 100 || roll > 100 || pitch > 100 || yaw > 100) {	

		smCurrState = S0_SAF;
		return TRANS_INVALID;
	} else {

		imu_init(false, 700);	//changed from 100
		initializeValues();
		smCurrState = S6_RAW;
		return TRANS_OK;
	}
	
}

/* TransValid stRaw2() 
 * Author: 		Prashanth G L
 * Function: 	Start raw mode after finishing calibration
 * OUT:			TransValid - The transition outcome
*/

TransValid stRaw2() {
	// Check if joystick lift is at 0 
	if(lift > 100 || roll > 100 || pitch > 100 || yaw > 100) {	

		smCurrState = S0_SAF;
		return TRANS_INVALID;
	} else {

		finishDmpCal();
		smCurrState = S6_RAW;
		return TRANS_OK;
	}
}

/* TransValid stHgh() 
 * Author: 		Prashanth G L
 * Function: 	Start Height-controlled mode
 * OUT:			TransValid - The transition outcome
*/
TransValid stHgt() {
	//Transition allowed only from mode 5, so no check for lift values
	desiredPressure = pressure;
	initializeHeightvalues();
	smCurrState = S7_HGT;
	return TRANS_OK;
}

/* TransValid stWir() 
 * Author: 		Sergio Soto
 * Function: 	Start Wireless mode
 * OUT:			TransValid - The transition outcome
*/
TransValid stWir() {
	// TODO: Initialize motors
	// TODO: Check calibration, joystick, etc
	// TODO: Initialize BT connection, protocol, etc
	// if (everything is ok)
	if(lift > 100 || roll > 100 || pitch > 100 || yaw > 100 || isCalibrated == 0) {	

		smCurrState = S0_SAF;
		return TRANS_INVALID;
	} else {
	smCurrState = S8_WIR;
	return TRANS_OK;
	}
}

/* TransValid panic() 
 * Author: 		Sergio Soto
 * Function: 	Enter Panic mode
 * OUT:			TransValid - The transition outcome
*/
TransValid panic() {
	// Lower RPM, start seconds timer and disable user input
	if (smCurrState == S8_WIR)
	{
		sendMessageEs(MSG14_START_JS);
	}
	smCurrState = S1_PAN;
	return TRANS_OK;
}

/* TransValid calDone() 
 * Author: 		Sergio Soto
 * Function: 	Exit calibration mode into safe
 * OUT:			TransValid - The transition outcome
*/
TransValid calDone() {
	finishDmpCal();
	smCurrState = S0_SAF;
	return TRANS_OK;
}

/* TransValid chkLand() 
 * Author: 		Prashanth G L
 * Function: 	Check if the drone is airborne before attempting to go into safe mode
 * OUT:			TransValid - The transition outcome
*/
TransValid chkLand() {
	// If coming from raw mode, re-initialize the MPU6050 to enable the DMP, else just switch to Safe mode
		if(smCurrState == S6_RAW) {
			imu_init(true, 100);
			calibINrawMode = false;
		}
		smCurrState = S0_SAF;
		return TRANS_OK;
}

/* TransValid diseng() 
 * Author: 		Sergio Soto
 * Function: 	After some time in panic, disengage motors and go to safe mode
 * OUT:			TransValid - The transition outcome
*/
TransValid diseng() {
	// Disengage motors, send warning by blinking Red LED
	smCurrState = S0_SAF;
	nrf_gpio_pin_set(RED);		// Turn of RED LED if it was on
	panicTimeout = 0;
	return TRANS_OK;
}

/* switchState(StateEs smNewState)
 * Author: 		Sergio Soto
 * Function: 	Attempts to switch to a new state
 * IN:			StateEs smNewState - The desired state to change into
 * OUT:			TransValid - The transition outcome, whether it was allowed or not
*/
TransValid switchState(StateEs smNewState) {

	return esSmTable[smCurrState][smNewState].action();
}
