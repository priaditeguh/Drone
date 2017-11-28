
/* pc-key-rx.c
 * Dictionary of commands to be received from the keyboard
 * Author: Sergio Soto
 */

 #include "uart-sm.h"
 #include "drone-state.h"
 #include <ncurses.h>

/* keyInput(int c)
 * Author: 		Sergio Soto
 * Function: 	Executes commands from keyboard input
 * IN:			int c - The int read from the keyboard
*/
void keyInput(int c) {
	switch(c) {
		case 'a':										// 'a': Lift up
			sendMessagePc(MSG04_INC_LIFT);
			break;
		case 'z':										// 'z': Lift down
			sendMessagePc(MSG05_DEC_LIFT);
			break;
		case 0x01000014:
		case KEY_RIGHT:									// '<-': Roll up
			sendMessagePc(MSG06_INC_ROLL);
			break;
		case 0x01000012:
		case KEY_LEFT:									// '->': Roll down
			sendMessagePc(MSG07_DEC_ROLL);
			break;
		case 0x01000015:
		case KEY_DOWN:									// 'up': Pitch up
			sendMessagePc(MSG08_INC_PITCH);
			break;
		case 0x01000013:
		case KEY_UP:									// 'down': Pitch down
			sendMessagePc(MSG09_DEC_PITCH);
			break;
		case 'w':										// 'q': Yaw up
			sendMessagePc(MSG10_INC_YAW);
			break;
		case 'q':										// 'w': Yaw down
			sendMessagePc(MSG11_DEC_YAW);
			break;

		case 'u':										// 'u': Pyaw constant up
			sendMessagePc(MSG12_INC_PY);
			break;
		case 'j':										// 'j': Pyaw constant down
			sendMessagePc(MSG13_DEC_PY);
			break;
		case 'i':										// 'i': P1 constant up
			sendMessagePc(MSG14_INC_P1);
			break;
		case 'k':										// 'k': P1 constant down
			sendMessagePc(MSG15_DEC_P1);
			break;
		case 'o':										// 'o': P2 constant up
			sendMessagePc(MSG16_INC_P2);
			break;
		case 'l':										// 'l': P2 constant down
			sendMessagePc(MSG17_DEC_P2);
			break;

		case 'e':										// 'e': PHeight constant up
			sendMessagePc(MSG20_INC_PHEIGHT);
			break;
		case 'E':										// 'E': PHeight constant down
			sendMessagePc(MSG21_DEC_PHEIGHT);
			break;

		case 'r':
			sendMessagePc(MSG22_INC_P_I);				// 'r': Increase P_I by one
			break;

		case 'R':
			sendMessagePc(MSG23_DEC_P_I);				// 'R': Decrease P_I by one
			break;

		case 't':
			sendMessagePc(MSG24_INC_P_D);				// 't': Increase P_D by one
			break;

		case 'T':
			sendMessagePc(MSG25_DEC_P_D);				// 'T': Decrease P_I by one
			break;

		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
			boardNextState = c - '0';
			sendMessagePc(MSG02_CHANGE_STATE);
			break;

		case '.':										// '.'	:	Request status from board
			dispMsg("Ping...");
			sendMessagePc(MSG00_STATUS);
			break;
		case KEY_END:
		case KEY_EXIT:
		case KEY_CLOSE:
		case 27:										// 'ESC':	Program termination
			dispMsg("Program terminating.");
			sendMessagePc(MSG01_QUIT);
			break;
		case 'b':
			dispMsg("Start logging");
			dispMsg("Wait.... ");
			sendMessagePc(MSG18_WRITE_LOG);
			break;
		case 'n':
			dispMsg("Send logging data from flash to PC");
			sendMessagePc(MSG19_READ_LOG);
			break;
		default:
			break;
	}
}
