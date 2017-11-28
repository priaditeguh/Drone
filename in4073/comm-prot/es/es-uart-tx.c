
/* es-uart-tx.c
 * Messages to be sent to the terminal
 */

#include "es-uart-tx.h"
#include "uart-sm.h"
#include "sm.h"
#include "in4073.h"
#include "control.h"
#include "data_log.h"

void stringCpy(char * inString);
void splitSI16(int16_t c, int index);
void splitSI32(int32_t c, int index);

char *stringToPC = "Battery Low!";
/* sendMessage(TxMsg msgType)
 * Author: 		Sergio Soto
 * Function: 	Transmits a message via UART to the board
 * IN:			TxMsg msgType - The message to be transmitted
*/
void sendMessageEs(TxMsgEs msgType){
	//uint16_t uns;
	if(msgType < TOTAL_ES_MESSAGES) {				// Validate message
		lastTxMsg = msgType;						// Store last message attempted to send
		switch (msgType){

			case MSG00_STATUS_ANS:
			case MSG01_QUIT_ANS:
				sendPacket(msgType, 0);
				break;
			case MSG02_CURR_STATE:
				txBuff[0] = smCurrState;
				sendPacket(msgType, 1);
				break;
			case MSG03_INV_STATE_TR:
				sendPacket(msgType, 0);
				break;
			case MSG04_DISP_MOTOR:
				splitSI16(ae[0], 0);
				splitSI16(ae[1], 2);
				splitSI16(ae[2], 4);
				splitSI16(ae[3], 6);
				sendPacket(msgType, 8);
				break;
			case MSG05_SENSOR_DATA:
				splitSI16(phi - dmpCalPhi, 	  	0);
				splitSI16(theta - dmpCalTheta,	2);
				splitSI16(psi - dmpCalPsi, 	  	4);
				splitSI16(sp - calSp, 			6);
				splitSI16(sq - calSq, 			8);
				splitSI16(sr - calSr, 			10);
				splitSI16(sax, 					12);
				splitSI16(say, 					14);
				splitSI16(saz, 					16);
				sendPacket(msgType, 18);
				break;
			case MSG06_ADC_DATA:
				splitSI16(bat_volt, 			0);
				splitSI16((int16_t)temperature,	2);	// Can be truncated
				//splitSI32(pressure, 			4);
				splitSI16((int16_t)(pressure - 101325),	4);
				sendPacket(msgType, 6);
				break;
			case MSG07_STRING:
				stringCpy(stringToPC);
				sendPacket(msgType, 20);
				break;
			case MSG08_TIMER_VALUE:
				splitSI32(timeSpent, 0);
				sendPacket(msgType, 4);
				break;
			case MSG09_ROLLPITCHYAW:
				splitSI16(roll, 	0);
				splitSI16(pitch,	2);
				splitSI16(yaw, 		4);
				splitSI16(lift, 	6);
				sendPacket(msgType, 8);
				break;
			case MSG10_P_CONSTANTS:
				splitSI16(pYaw,	0);
				splitSI16(p1, 	2);
				splitSI16(p2, 	4);
				splitSI16(pHeight, 6);
				txBuff[8] = P_I;
				txBuff[9] = P_D;
				sendPacket(msgType, 10);
				break;
			case MSG11_SEND_LOG_DATA:
				sendPacket(msgType,TOTAL_LOG_BYTES);
				break;
			case MSG12_NOTIFY_WRITE_FLASH:
				sendPacket(msgType,0);
				break;
			case MSG13_STOP_JS:
				sendPacket(msgType,0);
				break;	
			case MSG14_START_JS:
				sendPacket(msgType,0);	
				break;	
			case TOTAL_ES_MESSAGES:					// Only including this to avoid the warning [-Wswitch]
				break;
			default:
				break;
		}
	} else {
	}
}

/* stringCpy(char * inString)
 * Author: 		Prashanth G L
 * Function: 	Puts the string in the txBuff
 * IN:			char * inString - Pointer to the string
*/
void stringCpy(char * inString){

	for (int i = 0; i < 19; ++i)
	{
		txBuff[i] = inString[i];
	}
	txBuff[19] = '\0';

}

/* splitSI16(int16_t c, int index)
 * Author: 		Sergio Soto
 * Function: 	Splits and stores a signed int16 in the tx buffer plus sign
 * IN:			int16_t c - The integer to be split
 *              int index - The starting index in the buffer to store the value
*/
void splitSI16(int16_t c, int index) {

	txBuff[0 + index] = (uint8_t) ((c      ) & 0xff);
	txBuff[1 + index] = (uint8_t) ((c >> 8 ) & 0xff);   
}

/* splitSI32(int32_t c, int index)
 * Author: 		Prashanth G L
 * Function: 	Splits and stores a signed int32 in the tx buffer plus sign
 * IN:			int32_t c - The integer to be split
 *              int index - The starting index in the buffer to store the value
*/
void splitSI32(int32_t c, int index) {

	txBuff[0 + index] = (uint8_t) ((c      ) & 0xff);
	txBuff[1 + index] = (uint8_t) ((c >> 8 ) & 0xff);
	txBuff[2 + index] = (uint8_t) ((c >> 16) & 0xff);
	txBuff[3 + index] = (uint8_t) ((c >> 24) & 0xff);
}
