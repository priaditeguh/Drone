/*------------------------------------------------------------------
 *  control.c -- here you can implement your control algorithm
 *		 and any motor clipping or whatever else
 *		 remember! motor input =  0-1000 : 125-250 us (OneShot125)
 *
 *  I. Protonotarios
 *  Embedded Software Lab
 *
 *  July 2016
 *------------------------------------------------------------------
 */

#include "in4073.h"
#include "control.h"
#include <stdlib.h>

#define MIN_ROTOR_OUTPUT	200	// Minimum output that makes the rotors spin
#define PANIC_ROTOR_DEC		

// Dynamic inputs from joystick
int16_t lift = 0;
int16_t roll = 0;
int16_t pitch = 0;
int16_t yaw = 0;

// Static inputs from keyboard
int16_t kbLift = 0;
int16_t kbRoll = 0;
int16_t kbPitch = 0;
int16_t kbYaw = 0;

// Aggregated signals
int16_t totLift;
int16_t totRoll;
int16_t totPitch;
int16_t totYaw;

// Aggregated signals
int16_t ctrlLift;
int16_t ctrlRoll;
int16_t ctrlPitch;
int16_t ctrlYaw;
int16_t ctrlHeight;
// Control constants
int16_t pYaw =33;
int16_t p1 = 19;
int16_t p2 = 31;
int16_t pHeight=35;
int8_t P_I = 24;
int8_t P_D = 8;



int32_t iSum=0; 
int32_t old_error = 0;

/* update_motors()
 * Author: 		Priadi
 * Function: 	Update the motors for each state
*/
void update_motors(void)
{	

	switch(smCurrState){
		case S0_SAF:
			ae[0] = 0;
			ae[1] = 0;
			ae[2] = 0;
			ae[3] = 0;

			if (kbLift>130 || kbRoll >130 || kbPitch>130 || kbYaw >130)
			{
				kbLift  = 0;
				kbRoll  = 0;
				kbPitch = 0;
				kbYaw   = 0;
			}
			
			break;

		case S1_PAN:
			ae[0] -= 1;
			ae[1] -= 1;
			ae[2] -= 1;
			ae[3] -= 1;
			break;

		case S2_MAN:
			totLift = 	lift  + kbLift;
			totRoll = 	(roll >> 4) 	+ kbRoll;
			totPitch = 	(pitch >> 4) 	+ kbPitch;
			totYaw = 	(yaw >> 3) 		+ kbYaw;
	
			ae[0] = totLift + totPitch 	- totYaw;
			ae[1] = totLift - totRoll   + totYaw;
			ae[2] = totLift - totPitch 	- totYaw;
			ae[3] = totLift + totRoll   + totYaw;
			break;

		case S3_CAL:
			ae[0] = 0;
			ae[1] = 0;
			ae[2] = 0;
			ae[3] = 0;

			if (kbLift>130 || kbRoll >130 || kbPitch>130 || kbYaw >130)
			{
				kbLift  = 0;
				kbRoll  = 0;
				kbPitch = 0;
				kbYaw   = 0;
			}
			break;

		case S4_YAW:
			totLift = 	lift  						+ kbLift;
			totRoll = 	roll / 20 					+ kbRoll;
			totPitch = 	pitch / 20 					+ kbPitch;
			totYaw = 	yaw / 10  - ctrlYaw + kbYaw;

	
			ae[0] = totLift + totPitch 	- totYaw;
			ae[1] = totLift - totRoll   + totYaw;
			ae[2] = totLift - totPitch 	- totYaw;
			ae[3] = totLift + totRoll   + totYaw;
			break;

		case S5_FUL:
		case S6_RAW:
		case S8_WIR:
			totLift = 	lift  								+ kbLift;
			totRoll = 	roll / 20    + (ctrlRoll ) 		    + kbRoll;
			totPitch = 	pitch / 20   - (ctrlPitch)			+ kbPitch;
			totYaw = 	yaw / 10     - (ctrlYaw) 			+ kbYaw;
	
			ae[0] = totLift + totPitch 	- totYaw;
			ae[1] = totLift - totRoll   + totYaw;
			ae[2] = totLift - totPitch 	- totYaw;
			ae[3] = totLift + totRoll   + totYaw;
			break;

		case S7_HGT:
			totLift = 	lift  		 + ctrlHeight			+ kbLift;
			totRoll = 	roll / 20    + (ctrlRoll ) 		    + kbRoll;
			totPitch = 	pitch / 20   - (ctrlPitch)			+ kbPitch;
			totYaw = 	yaw / 10     - (ctrlYaw) 			+ kbYaw;

			ae[0] = totLift + totPitch 	- totYaw;
			ae[1] = totLift - totRoll   + totYaw;
			ae[2] = totLift - totPitch 	- totYaw;
			ae[3] = totLift + totRoll   + totYaw;
			break;


		default:
			break;

	}	

/* Motors are limited to 750 max and if lift is greater than 250, then they are limited to 250
so that abrupt change in the motors to 0 is avoided */

	if(lift > 250 && (smCurrState != S0_SAF 
					&& smCurrState != S1_PAN 
					&& smCurrState != S3_CAL)) {
		ae[0] = (ae[0] < 750) ? ((ae[0] < 250) ? 250: ae[0]) : 750;
		ae[1] = (ae[1] < 750) ? ((ae[1] < 250) ? 250: ae[1]) : 750;
		ae[2] = (ae[2] < 750) ? ((ae[2] < 250) ? 250: ae[2]) : 750;
		ae[3] = (ae[3] < 750) ? ((ae[3] < 250) ? 250: ae[3]) : 750;
	} else {
		ae[0] = (ae[0] < 750) ? ((ae[0] < 0) ? 0: ae[0]) : 750;
		ae[1] = (ae[1] < 750) ? ((ae[1] < 0) ? 0: ae[1]) : 750;
		ae[2] = (ae[2] < 750) ? ((ae[2] < 0) ? 0: ae[2]) : 750;
		ae[3] = (ae[3] < 750) ? ((ae[3] < 0) ? 0: ae[3]) : 750;		
	}

/* Finally assign the ae values to motor */
	motor[0] = ae[0];
	motor[1] = ae[1];
	motor[2] = ae[2];
	motor[3] = ae[3];

	nrf_gpio_pin_toggle(YELLOW);
}

/* manUpdate(ManCtrl ctrl)
 * Author: 		Sergio soto
 * Function: 	Keyboard trimming for motors
*/
void manUpdate(ManCtrl ctrl) {

	if(smCurrState == S2_MAN || smCurrState == S4_YAW || smCurrState == S5_FUL || smCurrState == S6_RAW|| smCurrState ==S8_WIR) {
		switch(ctrl) {
			case LIFT_UP:
				kbLift += 2;
				break;
			case LIFT_DOWN:
				kbLift -= 2; 
				break;
			case ROLL_UP:
				kbRoll += 1;
				break;
			case ROLL_DOWN:
				kbRoll -= 1;
				break;
			case PITCH_UP:
				kbPitch += 1;
				break;
			case PITCH_DOWN:
				kbPitch -= 1;
				break;
			case YAW_UP:
				kbYaw += 1;			
				break;
			case YAW_DOWN:
				kbYaw -= 1;	
				break;
		}
	}
}

/* run_filters_and_control()
 * Author: 		Prashanth G L
 * Function: 	Feedback control is implemented here, filters are implemented in mpu_wrapper.c
*/
void run_filters_and_control()
{

/* Proportional control for Yaw control mode */
	if(smCurrState == S4_YAW) {
		// Check if drone is "flying"
		if(lift + kbLift > MIN_ROTOR_OUTPUT) {
			ctrlYaw = pYaw * (-yaw - (sr - calSr));
		} else {
			ctrlYaw = 0;
		}
		ctrlYaw /= 150;
	}

/* P(ID) or cascased P control is implemented for FULL control and raw mode*/
	if(smCurrState == S5_FUL || smCurrState ==S8_WIR) {
		// Check if drone is "flying"
		if(lift + kbLift > MIN_ROTOR_OUTPUT) {
			ctrlYaw = (pYaw * (-yaw - (sr - calSr)))/150;
			ctrlRoll = (p1 * (roll - (phi - dmpCalPhi)))/1000 - (p2 * (sp - calSp))/400;
			ctrlPitch = (-p1 * (pitch - (theta - dmpCalTheta)))/1000 - (p2 * (sq - calSq))/400;
		} else {
			ctrlYaw = 0;
			ctrlRoll = 0;
			ctrlPitch = 0;
		}	
	}

	if(smCurrState == S6_RAW) {

/* Kalman filter is implemented in mpu_wrapper.c, so whenever the raw mode 
is used the values are always filtered irrespective of the mode */

	    if(lift + kbLift > MIN_ROTOR_OUTPUT) { // Check if drone is "flying"
			ctrlYaw = (pYaw * (-yaw - (sr - calSr)))/150;
			ctrlRoll = (p1 * (roll - (phi - dmpCalPhi)))/1000 - (p2 * (sp - calSp))/400;
			ctrlPitch =(-p1 * (pitch - (theta - dmpCalTheta)))/1000 - (p2 * (sq - calSq))/400;
		} else {
			ctrlYaw = 0;
			ctrlRoll = 0;
			ctrlPitch = 0;
		}

	}

/* PID control for height and cascaded P control for roll and pitch */
	if (smCurrState == S7_HGT)
	{
		// Check if drone is "flying"
		if(lift + kbLift > MIN_ROTOR_OUTPUT) {
			ctrlHeight = pidHeightControl();  
			ctrlYaw = (pYaw * (yaw - sr - calSr))/150;
			ctrlRoll = (p1 * (roll - (phi - dmpCalPhi)))/1000 - (p2 * (sp - calSp))/400;
			ctrlPitch = (-p1 * (pitch - (theta - dmpCalTheta)))/1000 - (p2 * (sq - calSq))/400;
		} else {
			ctrlYaw = 0;
			ctrlRoll = 0;
			ctrlPitch = 0;
		}
	}

	update_motors();
}


/* run_filters_and_control()
 * Author: 		Prashanth G L
 * Function: 	Initialize the PID values for height control
*/
void initializeHeightvalues(){

iSum=0;
old_error = 0;

}

/* run_filters_and_control()
 * Author: 		Prashanth G L
 * Function: 	PID control for height stabilization
*/
int32_t pidHeightControl(){

int32_t hgt;

int32_t error = pressure - desiredPressure;
iSum = iSum + error;

hgt = (pHeight*((error + ((iSum>>3) / (3 * P_I) ) + ((P_D * (error - old_error))>>3)/3)))>>6;
old_error = error;

return hgt;
}