#ifndef CALIB_H_
#define CALIB_H_

#include <inttypes.h>

#define CAL_WINDOW	50

int16_t dmpCalPhi;
int16_t dmpCalTheta;
int16_t dmpCalPsi;
int16_t calSp;
int16_t calSq;
int16_t calSr;

int8_t isCalibrated;

void startDmpCal();
void calibrateDmp();
void finishDmpCal();

#endif /* CALIB_H_ */