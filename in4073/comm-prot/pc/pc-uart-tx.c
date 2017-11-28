
/* pc-uart-tx.c
 * Messages to be sent to the board
 */

#include "pc-uart-tx.h"
#include "uart-sm.h"
#include <inttypes.h>
#include "pc_terminal.h"
#include "drone-state.h"
#include "pc-js-rx.h"
#include "pc_logging.h"

#include "interface.h"


void splitSI16(int16_t c, int index);
void splitSI32(int32_t c, int index);

/* sendMessage(TxMsg msgType)
 * Author: 		Prashanth G L
 * Function: 	Transmits a message via UART to the board
 * IN:			TxMsg msgType - The message to be transmitted
*/
void sendMessagePc(TxMsgPc msgType){

	if(msgType < TOTAL_PC_MESSAGES) {				// Validate message
		lastTxMsg = msgType;						// Store last message attempted to send
		switch (msgType){

			case MSG00_STATUS:
			case MSG01_QUIT:
				sendPacket(msgType, 0);
				break;
			case MSG02_CHANGE_STATE:
				txBuff[0] = boardNextState;
				sendPacket(msgType, 1);
				break;

			case MSG03_JOYSTICK:			
				splitSI16(jsInputRoll, 	0);
				splitSI16(jsInputPitch,	2);
				splitSI16(jsInputYaw, 	4);
				splitSI16(jsInputLift, 	6);
				txBuff[8]=fireButton;
				sendPacket(msgType, 9);
				break;
			case MSG04_INC_LIFT:
			case MSG05_DEC_LIFT:
			case MSG06_INC_ROLL:
			case MSG07_DEC_ROLL:
			case MSG08_INC_PITCH:
			case MSG09_DEC_PITCH:
			case MSG10_INC_YAW:
			case MSG11_DEC_YAW:
			case MSG12_INC_PY:
			case MSG13_DEC_PY:
			case MSG14_INC_P1:
			case MSG15_DEC_P1:
			case MSG16_INC_P2:
			case MSG17_DEC_P2:
			case MSG20_INC_PHEIGHT:
			case MSG21_DEC_PHEIGHT:
			case MSG22_INC_P_I:
			case MSG23_DEC_P_I:
			case MSG24_INC_P_D:
			case MSG25_DEC_P_D:
				sendPacket(msgType, 0);
				break;
			case MSG18_WRITE_LOG:
				sendPacket(msgType, 0);
				break;
			case MSG19_READ_LOG:
				sendPacket(msgType, 0);
				break;
			
			case TOTAL_PC_MESSAGES:					// Only including this to avoid the warning [-Wswitch]
				break;
			default:
				break;
		}
	} else {
	}
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

/* splitSI16(int16_t c, int index)
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
