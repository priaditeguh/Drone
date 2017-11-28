#ifndef CONTROL_H_
#define CONTROL_H_

typedef enum ManCtrl_t {
	LIFT_UP,
	LIFT_DOWN,
	ROLL_UP,
	ROLL_DOWN,
	PITCH_UP,
	PITCH_DOWN,
	YAW_UP,
	YAW_DOWN
} ManCtrl;

void manUpdate(ManCtrl ctrl);
void manJSupdate();
void initializeHeightvalues();
int32_t pidHeightControl();


int16_t lift;
int16_t roll;
int16_t pitch;
int16_t yaw;

// Control constants
int16_t pYaw;
int16_t p1;
int16_t p2;
int16_t pHeight;
int8_t P_I;
int8_t P_D;

int32_t iSum; 
int32_t old_error;
#endif /* CONTROL_H_ */
