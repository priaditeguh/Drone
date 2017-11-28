#ifndef UART_SM_H_
#define UART_SM_H_

// TODO: Do this in a cleaner way in makefile
#ifdef BUILD_PC
	#include "pc-uart-rx.h"
	#include "pc-uart-tx.h"
	#include "es-uart-tx.h"
	#include "pc_terminal.h" 
	#include "interface.h"
#elif defined BUILD_ES
	#include "es-uart-rx.h"
	#include "es-uart-tx.h"
	#include "pc-uart-tx.h"
	#include "in4073.h"
#endif
#include <inttypes.h>

// UART state machine states count
#define STATES_UART_COUNT 		4
// Maximum number of tries to send a message
#define MAX_TX_ATTEMPTS		5
// Message buffer max length
#define BUFFER_SIZE      50 // 20


// Valid States
typedef enum State_t {
	STATE_UART_WAIT,				// Idle, waiting for '~'
	STATE_UART_RX_TYPE,				// Data Type received
	STATE_UART_RX_DATA,				// Expecting Data
	STATE_UART_RX_CHKSM				// Expecting Checksum
} State;

extern uint8_t txBuff[BUFFER_SIZE];
extern uint8_t rxBuff[BUFFER_SIZE];

#ifdef BUILD_PC
	extern TxMsgPc lastTxMsg;
#elif defined BUILD_ES
	extern TxMsgEs lastTxMsg;
#endif
void setrxByte(uint8_t c);
void start_rx();
void receive_type();
void receive_data();
void receive_chksm();

void sendChar(uint8_t c);
#ifdef BUILD_PC
void sendPacket(TxMsgPc msgType, int length);
#elif defined BUILD_ES
void sendPacket(TxMsgEs msgType, int length);
#endif
void sendStartNibble();
void sendAck();
void sendNack();
void resendMessage();
void sendSpecialMsg();


#endif /* UART_SM_H_ */
