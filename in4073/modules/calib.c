#include "calib.h"
#include "in4073.h"

int16_t buffCalPhi[CAL_WINDOW];
int16_t buffCalTheta[CAL_WINDOW];
int16_t buffCalPsi[CAL_WINDOW];
int16_t dmpCalPhi = 0;
int16_t dmpCalTheta = 0;
int16_t dmpCalPsi = 0;

int16_t buffCalSp[CAL_WINDOW];
int16_t buffCalSq[CAL_WINDOW];
int16_t buffCalSr[CAL_WINDOW];
int16_t calSp = 0;
int16_t calSq = 0;
int16_t calSr = 0;

int16_t calIndex = 0;
int8_t isCalibrated = 0;

/* startDmpCal()
 * State: 	S3_CAL
 * Author: 	Prashanth G L
 * Action: 	Initializes MPU6050 sensor calibration. 
*/
void startDmpCal() {
	
	calIndex = 0;

	for(int i = 0; i < CAL_WINDOW; i++) {
		buffCalPhi[i] = 0;
		buffCalTheta[i] = 0;
		buffCalPsi[i] = 0;
		buffCalSp[i] = 0;
		buffCalSq[i] = 0;
		buffCalSr[i] = 0;
	}

	dmpCalPhi = 0;
	dmpCalTheta = 0;
	dmpCalPsi = 0;
	calSp = 0;
	calSq = 0;
	calSr = 0;

}

/* calibrateDmp()
 * State: 	S3_CAL
 * Author: 	Prashanth G L
 * Action: 	Stores last read Euler angles calculated for calibration. 
*/
void calibrateDmp() {

	buffCalPhi[calIndex] = phi;
	buffCalTheta[calIndex] = theta;
	buffCalPsi[calIndex] = psi;
	buffCalSp[calIndex] = sp;
	buffCalSq[calIndex] = sq;
	buffCalSr[calIndex] = sr;

	calIndex = (calIndex + 1) % CAL_WINDOW;
}

/* finishDmpCal()
 * State: 	S3_CAL
 * Author: 	Prashanth G L
 * Action: 	Calculates accumulated MPU6050 sensor calibration. 
*/
void finishDmpCal() {

	int32_t totDmpCalPhi = 0;
	int32_t totDmpCalTheta = 0;
	int32_t totDmpCalPsi = 0;
	int32_t totCalSp = 0;
	int32_t totCalSq = 0;
	int32_t totCalSr = 0;

	for(int i = 0; i < CAL_WINDOW; i++) {
		totDmpCalPhi   += buffCalPhi[i];
		totDmpCalTheta += buffCalTheta[i];
		totDmpCalPsi   += buffCalPsi[i];
		totCalSp += buffCalSp[i];
		totCalSq += buffCalSq[i];
		totCalSr += buffCalSr[i];
	}

	dmpCalPhi =   (int16_t) (totDmpCalPhi   / CAL_WINDOW);
	dmpCalTheta = (int16_t) (totDmpCalTheta / CAL_WINDOW);
	dmpCalPsi =   (int16_t) (totDmpCalPsi   / CAL_WINDOW);
	calSp = (int16_t) (totCalSp / CAL_WINDOW);
	calSq = (int16_t) (totCalSq / CAL_WINDOW);
	calSr = (int16_t) (totCalSr / CAL_WINDOW);

	isCalibrated = 1;
}