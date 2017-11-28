#ifndef ES_UART_TX_H_
#define ES_UART_TX_H_

typedef enum TxMsgEs_t {
	MSG00_STATUS_ANS,			// 00:	Answer status to terminal
	MSG01_QUIT_ANS,				// 01:	Answer program termination
	MSG02_CURR_STATE,			// 02:	Inform terminal of current state
	MSG03_INV_STATE_TR,			// 03: 	Inform invalid state transition error
	MSG04_DISP_MOTOR,			// 04: 	Send motor status
	MSG05_SENSOR_DATA,			// 05: 	Gyro, Accel and DMP data values
	MSG06_ADC_DATA,				// 06: 	Temp, Pressure and Battery data values
	MSG07_STRING,				// 07:  Sending a string
	MSG08_TIMER_VALUE,			// 08: 	Sending timer values (for profiling)
	MSG09_ROLLPITCHYAW,			// 09:	Send Roll Pitch Yaw Lift
	MSG10_P_CONSTANTS,			// 10: 	Send the values of the P constants
	MSG11_SEND_LOG_DATA, 		// 11:  Send one instance logged data from flash memory to PC
	MSG12_NOTIFY_WRITE_FLASH, 	// 12:  Notify PC after finish writing data into flash memory
	MSG13_STOP_JS,				// 13: 	Inform terminal to stop sending Joystick message
	MSG14_START_JS,				// 14:  Inform terminal to start sending Joystick message
	TOTAL_ES_MESSAGES
} TxMsgEs;

void sendMessageEs(TxMsgEs msgType);


#endif /* ES_UART_TX_H_ */
