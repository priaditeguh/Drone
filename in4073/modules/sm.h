#ifndef SM_H_
#define SM_H_

#include "in4073.h"
#include "calib.h"
#include "control.h"


// Valid transition outcomes
typedef enum TransValid_t {
	TRANS_INVALID,
	TRANS_NONE,	
	TRANS_OK		
} TransValid;

// Valid States
typedef enum StateEs_t { 
	S0_SAF,							// S0 - Safe mode
	S1_PAN,							// S1 - Panic mode
	S2_MAN,							// S2 - Manual mode
	S3_CAL,							// S3 - Calibration mode
	S4_YAW,							// S4 - Yaw-control mode
	S5_FUL,							// S5 - Full-control mode
	S6_RAW,							// S6 - Raw mode
	S7_HGT,							// S7 - Height-control mode
	S8_WIR,							// S8 - Wireless mode
	NUM_STATES_ES,
} StateEs;

// Name holder for actions (to avoid dereferencing everywhere)
typedef TransValid (*ActionEs)();

// A transition occurs when moving from one state to another
typedef struct TransitionEs_t {
	// StateEs nextState;				// Next state in the SM
	ActionEs action;					// Action executed on state transition
} TransitionEs;

StateEs smCurrState;

TransValid switchState(StateEs smNewState);

#endif /* SM_H_ */
