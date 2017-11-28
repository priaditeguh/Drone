#ifndef PC_UART_TX_H_
#define PC_UART_TX_H_

typedef enum TxMsgPc_t {
	MSG00_STATUS,				// 00:	Requests board status
	MSG01_QUIT,					// 01:	Program termination
	MSG02_CHANGE_STATE,			// 02:	Request board state change
	MSG03_JOYSTICK,				// 03:	Send Joystick values
	MSG04_INC_LIFT, 			// 04:	Increment Lift (with keyboard)
	MSG05_DEC_LIFT, 			// 05:	Decrement Lift (with keyboard)
	MSG06_INC_ROLL, 			// 06:	Increment Roll (with keyboard)
	MSG07_DEC_ROLL, 			// 07:	Decrement Roll (with keyboard)
	MSG08_INC_PITCH, 			// 08:	Increment Pitch (with keyboard)
	MSG09_DEC_PITCH, 			// 09:	Decrement Pitch (with keyboard)
	MSG10_INC_YAW, 				// 10:	Increment Yaw (with keyboard)
	MSG11_DEC_YAW, 				// 11:	Decrement Yaw (with keyboard)
	MSG12_INC_PY, 				// 12:	Increment Pyaw (with keyboard)
	MSG13_DEC_PY, 				// 13:	Decrement Pyaw (with keyboard)
	MSG14_INC_P1, 				// 14:	Increment P1 (with keyboard)
	MSG15_DEC_P1, 				// 15:	Decrement P1 (with keyboard)
	MSG16_INC_P2, 				// 16:	Increment P2 (with keyboard)
	MSG17_DEC_P2, 				// 17:	Decrement P2 (with keyboard)
	MSG18_WRITE_LOG, 			// 12: Start logging into flash memory
	MSG19_READ_LOG, 			// 13: Send the logging data from flash to PC
	MSG20_INC_PHEIGHT,
	MSG21_DEC_PHEIGHT,
	MSG22_INC_P_I,
	MSG23_DEC_P_I,
	MSG24_INC_P_D,
	MSG25_DEC_P_D,
	TOTAL_PC_MESSAGES
} TxMsgPc;

void sendMessagePc(TxMsgPc msgType);

#endif /* PC_UART_TX_H_ */
