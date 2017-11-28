
/* uart-sm.c
 * Communication protocol main functionality, shared between
 * the board and the terminal
 */

#include "uart-sm.h"

// Max amount of tries to resend a failed message
uint8_t maxTries = MAX_TX_ATTEMPTS;
// Char received buffer
volatile char rxChar = 0;
// Message type
int msgTypeRx = 0;
// Payload length
uint8_t plLength = 0;
// Payload position
uint8_t plPosition = 0;
// Checksum for Rx message
uint8_t chksmRx = 0;
// Payload buffer rx
uint8_t rxBuff[BUFFER_SIZE];
// Initial state: IDLE
State stateUart = STATE_UART_WAIT;

// Last message that was attempted to send
#ifdef BUILD_PC
	TxMsgPc lastTxMsg;
#elif defined BUILD_ES
	TxMsgEs lastTxMsg;
#endif
// Checksum for Tx message
uint8_t chksmTx;
// Payload buffer tx
uint8_t txBuff[BUFFER_SIZE];

// Machine state transitions {action}
Transition uartTable[STATES_UART_COUNT] = {
	/*STATE_UART_WAIT*/		/*STATE_UART_RX_TYPE*/		/*STATE_UART_RX_DATA*/		/*STATE_UART_RX_CHKSM*/
	{start_rx},				{receive_type},				{receive_data},				{receive_chksm}
};

/* setrxByte(char c)
 * Author: 		Sergio Soto
 * Function: 	Puts char in rxByte and performs UART (!) state machine action
 * IN:			char c - the char received
*/
void setrxByte(uint8_t c) {


	#ifdef DEBUG_MSG_ASCII									// When debugging, convert ASCII to numbers
		rxChar = c - '0';									// Store received char
	#else
		rxChar = c;											// Store received char
	#endif

	#ifdef DEBUG_RAW_MSG
	uint8_t p = (c < ' ') ? ' ' : c;
	dispMsg("\t\t<- %c: %02x", p, c & 0xff);
	#endif

	uartTable[stateUart].action();							// Perform UART action

}

/* start_rx()
 * Author: 	Sergio Soto
 * State: 	STATE_UART_WAIT
 * Action: 	Validate whether a new packet is being received, a sent packet was acknowledged or it wasn't.
 * 			On new packet, moves to STATE_UART_RX_TYPE state and waits for the message type.
 *			On acknowledge failure, attempt to resend the last message if max tries haven't been reached.
*/
void start_rx() {

	if(rxChar == '~'){											// Start nibble of packet received
		stateUart = STATE_UART_RX_TYPE;							// Change state expecting payload
		chksmRx = 0;											// Reset checksum

	}else if(rxChar == '?' && maxTries > 0){					// Sent packet was corrupted, tries left
		maxTries -= 1;
		resendMessage();										// Try re-sending the last message

	}else if(rxChar == '?'){									// Sent packet was corrupted, no tries left
		// Panic! send a special message to help the device find start byte again in the next message
		sendSpecialMsg();
		resendMessage();
		maxTries = MAX_TX_ATTEMPTS;
	}
}

/* receive_type()
 * Author: 		Sergio Soto
 * State: 	STATE_UART_RX_TYPE
 * Action: 	Validates the message type and stores the message lenght expected to be received.
 * 			Adds the type nibble to the checksum.
 *			If the type is valid, moves to STATE_UART_RX_DATA state and waits for the payload.
 *			If the message has no payload, acknowledges the message and returns to standby
*/
void receive_type() {

	#ifdef BUILD_PC
	if(rxChar < TOTAL_ES_MESSAGES) {							// Validate message type
	#elif defined BUILD_ES
	if(rxChar < TOTAL_PC_MESSAGES) {							// Validate message type
	#endif
		chksmRx += rxChar;										// Adds type to checksum
		msgTypeRx = (int)rxChar;								// Store message type
		plLength = msgTable[msgTypeRx].length;					// Store the length
		plPosition = plLength;									// Set the offset of the payload

		if(plPosition > 0) {									// If the message has payload
			stateUart = STATE_UART_RX_DATA;						// Switch state to wait for payload

		} else {												// If the message has no payload
			#ifdef DEBUG_MSG_NO_CHECKSUM						// When debugging protocol, skip checksum
				msgTable[msgTypeRx].trans.action();				// Execute action directly
				stateUart = STATE_UART_WAIT;					// Go back to waiting state
				#ifndef DEBUG_MSG_NO_ACK
					sendAck();
				#endif
			#else
				stateUart = STATE_UART_RX_CHKSM;				// Move state to wait for checksum
			#endif
		}

	} else {													// Message type was invalid
		stateUart = STATE_UART_WAIT;							// Discard packet and go back to wait
		#ifndef DEBUG_MSG_NO_ACK
			sendAck();
		#endif
	}
}

/* recieve_data()
 * Author: 		Sergio Soto
 * State: 	STATE_UART_RX_DATA
 * Action: 	Stores received payload data into buffer
*/
void receive_data() {

	rxBuff[plLength - plPosition] = rxChar;						// Store data into buffer
	plPosition -= 1;											// Update position pointer
	chksmRx += rxChar;											// Add data to checksum

	if(plPosition > 0) {										// If more data is expected
		stateUart = STATE_UART_RX_DATA;							// Stay in current state

	} else {													// If last char of payload was received
		#ifdef DEBUG_MSG_NO_CHECKSUM							// When debugging protocol, skip checksum
			msgTable[msgTypeRx].trans.action();					// Perform action
			stateUart = STATE_UART_WAIT;						// Move to waiting state
			#ifndef DEBUG_MSG_NO_ACK
				sendAck();
			#endif
		#else
			stateUart = STATE_UART_RX_CHKSM;					// Move state to wait for checksum
		#endif
	}
}

/* recieve_chksm()
 * Author: 		Sergio Soto
 * State: 	STATE_UART_RX_CHKSM
 * Action: 	Calculates the checksum.
 * If correct, acknowledges message. Otherwise sends a no-acknowledge message back to the board
*/
void receive_chksm() {

	chksmRx -= rxChar;											// Calculate checksum difference

	#ifndef DEBUG_MSG_NO_CHECKSUM								// When NOT debugging protocol, validate checksum
		if(chksmRx == 0){										// Checksum valid
			#ifndef DEBUG_MSG_NO_ACK
				sendAck();
			#endif
			msgTable[msgTypeRx].trans.action();					// Perform action

		} else {												// Checksum not valid
			#ifndef DEBUG_MSG_NO_ACK
				sendNack();
			#endif
		}
	#endif														// No #else, since this state should be unreachable when debugging

	stateUart = STATE_UART_WAIT;
}


/* sendPacket(char type, int length)
 * Author: 		Sergio Soto
 * Function: 	Transmits a message via UART to the board
 * IN:			TxMsg msgType - The message to be transmitted
 * IN: 			int length - the length of the payload
*/
#ifdef BUILD_PC
void sendPacket(TxMsgPc msgType, int length){
#elif defined BUILD_ES
void sendPacket(TxMsgEs msgType, int length){
#endif
	sendStartNibble();							// Send start nibble to indicate start of message
	sendChar((uint8_t)msgType);					// Send the message type
	chksmTx += (uint8_t)msgType;					// Add type to checksum
	int index = 0;

	while(index < length) {						// Send char by char of payload to the board
		sendChar(txBuff[index]);
		chksmTx += txBuff[index];
		index += 1;
	}

	#ifndef DEBUG_MSG_NO_CHECKSUM
		sendChar(chksmTx);						// Send checksum
	#endif
}

/* sendSpecialMsg()
 * Author: 		Prashanth G L
 * Function: 	Transmits the special message, 20 chars of % symbol so that the receiver 
 				decodes the start byte of next message
*/

void sendSpecialMsg()  
{
	int i;
	for (i=0;i<(BUFFER_SIZE+4);i++){
		sendChar('%');
	}
}


/* sendStartNibble()
 * Author: 		Sergio Soto
 * Function: 	Transmits the start nibble, '~' as the start of a message
*/
void sendStartNibble(){

	sendChar('~');
	chksmTx = 0;
}

/* sendNack()
 * Author: 		Sergio Soto
 * Function: 	Request the last message received to be retransmitted
*/
void sendNack() {

	#ifdef BUILD_PC
		dispMsg("nack");
	#endif
	sendChar('?');
}

/* resendMessage()
 * Author: 		Sergio Soto
 * Function: 	Re-transmits the last sent message via UART to the board
*/
void resendMessage() {

	#ifdef BUILD_PC
		sendMessagePc(lastTxMsg);
		dispMsg("resend");
	#elif defined BUILD_ES
		sendMessageEs(lastTxMsg);
	#endif
}

/* sendChar()
 * Author: 		Sergio Soto
 * Function: 	Transmits a single character using the correct interface
 * IN: 			char c - The character to be sent
*/
void sendChar(uint8_t c) {
	#ifdef BUILD_PC
		#ifdef DEBUG_RAW_MSG
			uint8_t p = (c < ' ') ? ' ' : c;
			dispMsg("%c: %02x ->", p, c & 0xff);
		#endif
		rs232_putchar(c);
	#elif defined BUILD_ES
		uart_put(c);
	#endif
}
