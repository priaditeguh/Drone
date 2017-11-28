#ifndef DRONE_STATE_H_
#define DRONE_STATE_H_

#include <inttypes.h>

typedef enum DisplayVal_t {
	DISP_LIFT,
	DISP_ROLL,
	DISP_PITCH,
	DISP_YAW,
	DISP_AE0,
	DISP_AE1,
	DISP_AE2,
	DISP_AE3,
	DISP_P,
	DISP_Q,
	DISP_R,
	DISP_AX,
	DISP_AY,
	DISP_AZ,
	DISP_PHI,
	DISP_THE,
	DISP_PSI,
	DISP_BATT,
	DISP_TEMP,
	DISP_PRESS,
	DISP_MODE,
} DisplayVal;

int16_t liftBuff;
int16_t rollBuff;
int16_t pitchBuff;
int16_t yawBuff;
int8_t fireButton;
int16_t aeBuff[4];

int16_t pBuff;
int16_t qBuff;
int16_t rBuff;

int16_t axBuff;
int16_t ayBuff;
int16_t azBuff;

int16_t phiBuff;
int16_t theBuff;
int16_t psiBuff;

int16_t battBuff;
int16_t tempBuff;
int32_t pressBuff;

char modeBuff;

char boardCurrState;
char boardNextState;

#endif /* DRONE_STATE_H_ */
