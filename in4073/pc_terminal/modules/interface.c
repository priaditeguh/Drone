
#include "interface.h"

Screen screen = SCREEN_INTRO;
int minY, maxY, minX, maxX;
int yFrame, xFrame;

WINDOW *winFrame;
WINDOW *winGyro;
WINDOW *winAcc;
WINDOW *winDmp;
WINDOW *winEtc;
WINDOW *winMode;
WINDOW *winRotors;
WINDOW *winLrpy;
WINDOW *winMsg;

#define SQRE_H		10								// Squaare heigth
#define SQRE_W		2 * SQRE_H						// Square width
#define SQRE_S		4								// Space between squares
#define FRAME_W		(SQRE_W * 5 + SQRE_S * 6)		// Frame width
#define FRAME_H		45								// Frame heigth
#define MSG_COUNT	(FRAME_H - 29)					// Number of messages stored

void introScreen();
void continueScreen();
void mainScreen();
void updateModeDisp();
void updateMotorDisp();
void updateValuesDisp();
void dispMsg(const char* format, ... );
void dispVal(DisplayVal id, int16_t val);

char messagesBuff[MSG_COUNT][FRAME_W];
char waitForUser = 1;

/* introAlarm(int sig)
 * Author: 		Sergio Soto
 * Function: 	Initializes the signal alarm and goes to the next screen on the UI
 * IN:			int sig - the value for alarm
*/

void  introAlarm(int sig) {

	signal(SIGALRM, SIG_IGN);          	/* ignore this signal       */
	screen = SCREEN_CONTINUE;
	continueScreen();
}

/* initInterface()
 * Author: 		Sergio Soto
 * Function: 	Initializes the interface
*/


void initInterface() {

	signal(SIGALRM, introAlarm);
	alarm(1);

	initscr();
	cbreak();
	noecho();
	keypad(stdscr, TRUE);
	getbegyx(stdscr, minY, minX);
	getmaxyx(stdscr, maxY, maxX);
	yFrame = (maxY - FRAME_H) / 2;
	xFrame = (maxX - FRAME_W) / 2;

	if(has_colors()) {
		start_color();
		init_pair(1, COLOR_RED, COLOR_BLACK);
		init_pair(2, COLOR_YELLOW, COLOR_BLACK);
		init_pair(3, COLOR_GREEN, COLOR_BLACK);
		init_pair(4, COLOR_BLUE, COLOR_BLACK);
	}

	introScreen();
}


/* closeInterface()
 * Author: 		Sergio Soto
 * Function: 	Closes the interface
*/
void closeInterface() {

	endwin();
}

/* introScreen()
 * Author: 		Sergio Soto
 * Function: 	Display introduction screen
*/
void introScreen() {

	int y = (maxY - 28) / 2;
	int xDrone = (maxX - 50) / 2;
	int xLogo = (maxX - 130) / 2;

	mvprintw(y + 0, xDrone, "          `/osyso:`             :+syso/.          \n");
	mvprintw(y + 1, xDrone, "        -hmo:-.-/smy.        `smy/-..:odd:        \n");
	mvprintw(y + 2, xDrone, "       /N+         oN:      .my`        :Ns       \n");
	mvprintw(y + 3, xDrone, "      `Ns   /dNd-   hm      hm   .hNm+   /M-      \n");
	mvprintw(y + 4, xDrone, "      .M+   sMMMd`  sN      dd  `yMMMh   :M:      \n");
	mvprintw(y + 5, xDrone, "       ym.   .:+NNs-/+      :o.oNNs:-   `dd       \n");
	mvprintw(y + 6, xDrone, "        oNo-    `sMMms:.``.:odMMh.    .+my`       \n");
	mvprintw(y + 7, xDrone, "         `+yddddh.:NMMMMMMMMMMM+`yddddh+.         \n");
	mvprintw(y + 8, xDrone, "                   /MMMMMMMMMMs                   \n");
	mvprintw(y + 9, xDrone, "                   `MMMMMMMMMM-                   \n");
	mvprintw(y + 10, xDrone, "                   /MMMMMMMMMMs                   \n");
	mvprintw(y + 11, xDrone, "         `+yddddh.:NMMMMMMMMMMM+`hddddh+.         \n");
	mvprintw(y + 12, xDrone, "        oNs-    `sNMms/.``.:odMMh.    .+my`       \n");
	mvprintw(y + 13, xDrone, "       ym.   -:+NNy-/+      :o.oNNs:-   `dd       \n");
	mvprintw(y + 14, xDrone, "      .M+   sMMMd`  sN      dd  `yMMMh   :M:      \n");
	mvprintw(y + 15, xDrone, "      `Ns   :dmh-   hm      hm   .hmd+   /M-      \n");
	mvprintw(y + 16, xDrone, "       /N+         oN:      .my`        :Ns       \n");
	mvprintw(y + 17, xDrone, "        -hmo:-.-/smy.        `smy/-..:odd:        \n");
	mvprintw(y + 18, xDrone, "          `/osyso:`             :+syso/.          \n");

	attron(COLOR_PAIR(1));
	mvprintw(y + 20, xLogo, " _________  _______   ________  ___  ___  ___  ___       ________          ________  ________  ________  ________   _______      \n");
	mvprintw(y + 21, xLogo, "|\\___   ___\\\\  ___ \\ |\\   __  \\|\\  \\|\\  \\|\\  \\|\\  \\     |\\   __  \\        |\\   ___ \\|\\   __  \\|\\   __  \\|\\   ___  \\|\\  ___ \\     \n");
	mvprintw(y + 22, xLogo, "\\|___ \\  \\_\\ \\   __/|\\ \\  \\|\\  \\ \\  \\\\\\  \\ \\  \\ \\  \\    \\ \\  \\|\\  \\       \\ \\  \\_|\\ \\ \\  \\|\\  \\ \\  \\|\\  \\ \\  \\\\ \\  \\ \\   __/|    \n");
	mvprintw(y + 23, xLogo, "     \\ \\  \\ \\ \\  \\_|/_\\ \\  \\\\\\  \\ \\  \\\\\\  \\ \\  \\ \\  \\    \\ \\   __  \\       \\ \\  \\ \\\\ \\ \\   _  _\\ \\  \\\\\\  \\ \\  \\\\ \\  \\ \\  \\_|/__  \n");
	mvprintw(y + 24, xLogo, "      \\ \\  \\ \\ \\  \\_|\\ \\ \\  \\\\\\  \\ \\  \\\\\\  \\ \\  \\ \\  \\____\\ \\  \\ \\  \\       \\ \\  \\_\\\\ \\ \\  \\\\  \\\\ \\  \\\\\\  \\ \\  \\\\ \\  \\ \\  \\_|\\ \\ \n");
	mvprintw(y + 25, xLogo, "       \\ \\__\\ \\ \\_______\\ \\_____  \\ \\_______\\ \\__\\ \\_______\\ \\__\\ \\__\\       \\ \\_______\\ \\__\\\\ _\\\\ \\_______\\ \\__\\\\ \\__\\ \\_______\\\n");
	mvprintw(y + 26, xLogo, "        \\|__|  \\|_______|\\|___| \\__\\|_______|\\|__|\\|_______|\\|__|\\|__|        \\|_______|\\|__|\\|__|\\|_______|\\|__| \\|__|\\|_______|\n");
	mvprintw(y + 27, xLogo, "                               \\|__|                                                                                             \n");
	attroff(COLOR_PAIR(1));
	refresh();
}

/* continueScreen()
 * Author: 		Sergio Soto
 * Function: 	Display continue screen (message to user to move to the next screen)
*/
void continueScreen() {

	int y = (maxY - 28) / 2;
	int xPress = (maxX - 28) / 2;
	mvprintw(y + 30, xPress, "Press any key to continue...\n");
	refresh();
	timeout(-1);
	getch();
	waitForUser = 0;
	screen = SCREEN_MAIN;
	mainScreen();
}

/* mainScreen()
 * Author: 		Sergio Soto
 * Function: 	Display Main screen with all the data grabbed from the fricking drone
*/
void mainScreen() {

	clear();
	refresh();

	winFrame = newwin(FRAME_H, FRAME_W, yFrame, xFrame);
	wborder(winFrame, '#', '#', '#', '#', '#', '#', '#', '#');
	wrefresh(winFrame);

	winGyro = newwin(SQRE_H, SQRE_W, yFrame + SQRE_S / 2, xFrame + SQRE_S + (SQRE_S + SQRE_W) * 0);
	wborder(winGyro, '|', '|', '=', '-', '+', '+', '+', '+');
	const char *titGyro = "Gyroscope";
	mvwprintw(winGyro, 1, (SQRE_W - strlen(titGyro)) / 2 + 1, titGyro);
	mvwhline(winGyro, 2, 1, '=', SQRE_W - 2);
	mvwhline(winGyro, 5, 1, '-', SQRE_W - 2);
	mvwhline(winGyro, 7, 1, '-', SQRE_W - 2);
	wrefresh(winGyro);

	winAcc = newwin(SQRE_H, SQRE_W, yFrame + SQRE_S / 2, xFrame + SQRE_S + (SQRE_S + SQRE_W) * 1);
	wborder(winAcc, '|', '|', '=', '-', '+', '+', '+', '+');
	const char *titAcc = "Accelerometer";
	mvwprintw(winAcc, 1, (SQRE_W - strlen(titAcc)) / 2 + 1, titAcc);
	mvwhline(winAcc, 2, 1, '=', SQRE_W - 2);
	mvwhline(winAcc, 5, 1, '-', SQRE_W - 2);
	mvwhline(winAcc, 7, 1, '-', SQRE_W - 2);
	wrefresh(winAcc);

	winDmp = newwin(SQRE_H, SQRE_W, yFrame + SQRE_S / 2, xFrame + SQRE_S + (SQRE_S + SQRE_W) * 2);
	wborder(winDmp, '|', '|', '=', '-', '+', '+', '+', '+');
	const char *titDmp = "DMP";
	mvwprintw(winDmp, 1, (SQRE_W - strlen(titDmp)) / 2 + 1, titDmp);
	mvwhline(winDmp, 2, 1, '=', SQRE_W - 2);
	mvwhline(winDmp, 5, 1, '-', SQRE_W - 2);
	mvwhline(winDmp, 7, 1, '-', SQRE_W - 2);
	wrefresh(winDmp);

	winEtc = newwin(SQRE_H, SQRE_W, yFrame + SQRE_S / 2, xFrame + SQRE_S + (SQRE_S + SQRE_W) * 3);
	wborder(winEtc, '|', '|', '=', '-', '+', '+', '+', '+');
	const char *titOther = "Other";
	mvwprintw(winEtc, 1, (SQRE_W - strlen(titOther)) / 2 + 1, titOther);
	mvwhline(winEtc, 2, 1, '=', SQRE_W - 2);
	mvwhline(winEtc, 5, 1, '-', SQRE_W - 2);
	mvwhline(winEtc, 7, 1, '-', SQRE_W - 2);
	wrefresh(winEtc);

	winLrpy = newwin(9, SQRE_W, yFrame + SQRE_S / 2 + SQRE_H + SQRE_S / 4, xFrame + SQRE_S + (SQRE_S + SQRE_W) * 4);
	wborder(winLrpy, '|', '|', '=', '-', '+', '+', '+', '+');
	mvwhline(winLrpy, 2, 1, '-', SQRE_W - 2);
	mvwhline(winLrpy, 4, 1, '-', SQRE_W - 2);
	mvwhline(winLrpy, 6, 1, '-', SQRE_W - 2);
	wrefresh(winLrpy);

	winMode = newwin(SQRE_H, SQRE_W, yFrame + SQRE_S / 2, xFrame + SQRE_S + (SQRE_S + SQRE_W) * 4);
	wborder(winMode, '|', '|', '-', '-', '+', '+', '+', '+');

	winRotors = newwin(9, FRAME_W - 3 * SQRE_S - SQRE_W, yFrame + SQRE_S / 2 + SQRE_H + SQRE_S / 4, xFrame + SQRE_S);
	wborder(winRotors, '|', '|', '=', '-', '+', '+', '+', '+');
	const char *titRotors = "Rotors";
	mvwprintw(winRotors, 1, 2, titRotors);
	mvwhline(winRotors, 2, 1, '=', FRAME_W - 3 * SQRE_S - SQRE_W - 2);
	mvwprintw(winRotors, 3, 2, "              0                                   500                                 1k");

	winMsg = newwin(FRAME_H - 3 * SQRE_S / 2 - SQRE_H - 9,
							FRAME_W - 2 * SQRE_S,
							yFrame + SQRE_S / 2 + SQRE_H + 2 * SQRE_S / 4 + 9,
							xFrame + SQRE_S);
	wborder(winMsg, '|', '|', '=', '-', '+', '+', '+', '+');
	const char *titMsg = "Messages";
	mvwprintw(winMsg, 1, 2, titMsg);
	mvwhline(winMsg, 2, 1, '=', FRAME_W - 2 * SQRE_S - 2);
	wrefresh(winMsg);

	updateModeDisp();
	updateMotorDisp();
	updateValuesDisp();
	dispMsg("****************************");
	refresh();
}

/* updateModeDisp()
 * Author: 		Sergio Soto
 * Function: 	Update the display if the mode of the drone changes
*/

void updateModeDisp() {

	wattron(winMode, COLOR_PAIR(3));

	switch(modeBuff) {
		case 0:
			mvwprintw(winMode, 1, 1, "        ___       ");
			mvwprintw(winMode, 2, 1, "       / _ \\      ");
			mvwprintw(winMode, 3, 1, "      | | | |     ");
			mvwprintw(winMode, 4, 1, "      | | | |     ");
			mvwprintw(winMode, 5, 1, "      | |_| |     ");
			mvwprintw(winMode, 6, 1, "       \\___/      ");
			mvwprintw(winMode, 8, 1, "       Safe       ");
			break;
		case 1:
			mvwprintw(winMode, 1, 1, "       __         ");
			mvwprintw(winMode, 2, 1, "      /_ |        ");
			mvwprintw(winMode, 3, 1, "       | |        ");
			mvwprintw(winMode, 4, 1, "       | |        ");
			mvwprintw(winMode, 5, 1, "       | |        ");
			mvwprintw(winMode, 6, 1, "       |_|        ");
			mvwprintw(winMode, 8, 1, "      Panic       ");
			break;
		case 2:
			mvwprintw(winMode, 1, 1, "       ___        ");
			mvwprintw(winMode, 2, 1, "      |__ \\       ");
			mvwprintw(winMode, 3, 1, "         ) |      ");
			mvwprintw(winMode, 4, 1, "        / /       ");
			mvwprintw(winMode, 5, 1, "       / /_       ");
			mvwprintw(winMode, 6, 1, "      |____|      ");
			mvwprintw(winMode, 8, 1, "      Manual      ");
			break;
		case 3:
			mvwprintw(winMode, 1, 1, "       ____       ");
			mvwprintw(winMode, 2, 1, "      |___ \\      ");
			mvwprintw(winMode, 3, 1, "        __) |     ");
			mvwprintw(winMode, 4, 1, "       |__ <      ");
			mvwprintw(winMode, 5, 1, "       ___) |     ");
			mvwprintw(winMode, 6, 1, "      |____/      ");
			mvwprintw(winMode, 8, 1, "    Calibration   ");
			break;
		case 4:
			mvwprintw(winMode, 1, 1, "       _  _       ");
			mvwprintw(winMode, 2, 1, "      | || |      ");
			mvwprintw(winMode, 3, 1, "      | || |_     ");
			mvwprintw(winMode, 4, 1, "      |__   _     ");
			mvwprintw(winMode, 5, 1, "         | |      ");
			mvwprintw(winMode, 6, 1, "         |_|      ");
			mvwprintw(winMode, 8, 1, "     Yaw-control  ");
			break;
		case 5:
			mvwprintw(winMode, 1, 1, "       _____      ");
			mvwprintw(winMode, 2, 1, "      | ____|     ");
			mvwprintw(winMode, 3, 1, "      | |__       ");
			mvwprintw(winMode, 4, 1, "      |___ \\      ");
			mvwprintw(winMode, 5, 1, "       ___) |     ");
			mvwprintw(winMode, 6, 1, "      |____/      ");
			mvwprintw(winMode, 8, 1, "    Full-control  ");
			break;
		case 6:
			mvwprintw(winMode, 1, 1, "         __       ");
			mvwprintw(winMode, 2, 1, "        / /       ");
			mvwprintw(winMode, 3, 1, "       / /_       ");
			mvwprintw(winMode, 4, 1, "      | '_ \\      ");
			mvwprintw(winMode, 5, 1, "      | (_) |     ");
			mvwprintw(winMode, 6, 1, "       \\___/      ");
			mvwprintw(winMode, 8, 1, "      Raw mode    ");
			break;
		case 7:
			mvwprintw(winMode, 1, 1, "       ______     ");
			mvwprintw(winMode, 2, 1, "      |____  |    ");
			mvwprintw(winMode, 3, 1, "          / /     ");
			mvwprintw(winMode, 4, 1, "         / /      ");
			mvwprintw(winMode, 5, 1, "        / /       ");
			mvwprintw(winMode, 6, 1, "       /_/        ");
			mvwprintw(winMode, 8, 1, "  Height-control  ");
			break;
		case 8:
			mvwprintw(winMode, 1, 1, "        ___       ");
			mvwprintw(winMode, 2, 1, "       / _ \\      ");
			mvwprintw(winMode, 3, 1, "      | (_) |     ");
			mvwprintw(winMode, 4, 1, "       > _ <      ");
			mvwprintw(winMode, 5, 1, "      | (_) |     ");
			mvwprintw(winMode, 6, 1, "       \\___/      ");
			mvwprintw(winMode, 8, 1, "      Wireless    ");
			break;
	}

	wattroff(winMode, COLOR_PAIR(3));
	wrefresh(winMode);
}

/* updateMotorDisp()
 * Author: 		Sergio Soto
 * Function: 	Display of the motor values of the drone 
*/
void updateMotorDisp() {

	int len = FRAME_W - 3 * SQRE_S - SQRE_W - 20;
	int bar[4];

	bar[0] = len * aeBuff[0] / 1000;
	bar[1] = len * aeBuff[1] / 1000;
	bar[2] = len * aeBuff[2] / 1000;
	bar[3] = len * aeBuff[3] / 1000;

	for(int m = 0; m < 4; m++) {
		mvwprintw(winRotors, 4 + m, 2, "Rotor %d: %4d ", m, aeBuff[m]);
		waddch(winRotors, '[');
		for(int i = 0; i < bar[m]; i++) {
			waddch(winRotors, '>');
		}
		for(int i = 0; i < len - bar[m]; i++) {
			waddch(winRotors, ' ');
		}
		waddch(winRotors, ']');
	}
	wrefresh(winRotors);
}

/* updateValuesDisp()
 * Author: 		Sergio Soto
 * Function: 	Display of the sensor values of the drone 
*/
void updateValuesDisp() {

	mvwprintw(winGyro, 4, 2, "p: %13d", pBuff);
	mvwprintw(winGyro, 6, 2, "q: %13d", qBuff);
	mvwprintw(winGyro, 8, 2, "r: %13d", rBuff);

	mvwprintw(winAcc, 4, 2, "ax: %12d", axBuff);
	mvwprintw(winAcc, 6, 2, "ay: %12d", ayBuff);
	mvwprintw(winAcc, 8, 2, "az: %12d", azBuff);

	mvwprintw(winDmp, 4, 2, "phi: %11d", phiBuff);
	mvwprintw(winDmp, 6, 2, "the: %11d", theBuff);
	mvwprintw(winDmp, 8, 2, "psi: %11d", psiBuff);

	mvwprintw(winEtc, 4, 2, "Temp: %6d.%2dC", tempBuff / 100, tempBuff % 100);
	mvwprintw(winEtc, 6, 2, "Press: %7dkPa", pressBuff);

	if(battBuff > 12000) {
		wattron(winEtc, COLOR_PAIR(3));
		mvwprintw(winEtc, 8, 2, "Battery: %6dmV", battBuff);
		wattroff(winEtc, COLOR_PAIR(3));
	} else if(battBuff > 10500) {
		wattron(winEtc, COLOR_PAIR(2));
		mvwprintw(winEtc, 8, 2, "Battery: %6dmV", battBuff);
		wattroff(winEtc, COLOR_PAIR(2));
	} else {
		wattron(winEtc, COLOR_PAIR(1));
		mvwprintw(winEtc, 8, 2, "Battery: %6dmV", battBuff);
		wattroff(winEtc, COLOR_PAIR(1));
	}

	mvwprintw(winLrpy, 1, 2, "Lift: %10d", liftBuff);
	mvwprintw(winLrpy, 3, 2, "Roll: %10d", rollBuff);
	mvwprintw(winLrpy, 5, 2, "Pitch: %9d", pitchBuff);
	mvwprintw(winLrpy, 7, 2, "Yaw: %11d", yawBuff);

	wrefresh(winGyro);
	wrefresh(winAcc);
	wrefresh(winDmp);
	wrefresh(winEtc);
	wrefresh(winLrpy);
}

/* dispMsg(const char* format, ... )
 * Author: 		Sergio Soto
 * Function: 	This where any text received from ES is displayed other than the parameters
 				already displayed, functions exactly like printf.
*/
void dispMsg(const char* format, ... ) {

	static int msgIndex = 0;
	static int longest = 0;
	int p;
	char msg[FRAME_W];

	va_list args;
    va_start( args, format );
    vsprintf (msg,format, args);
    va_end( args );

	strcpy(messagesBuff[msgIndex], msg);

	int length = strlen(messagesBuff[msgIndex]);
	longest = length > longest ? length : longest;

	for(int i = 0; i < MSG_COUNT; i++) {
		p = (msgIndex + i + 1) % MSG_COUNT;
		if(messagesBuff[p] != NULL) {
			int currLen = strlen(messagesBuff[p]);
			if(currLen < longest){
				mvwprintw(winMsg, i + 3, 2, "                                                                         ");  // Lazy, but it's 4:30 am
			}
			mvwprintw(winMsg, i + 3, 2, messagesBuff[p]);
		}
	}

	msgIndex = (msgIndex + 1) % MSG_COUNT;
	wrefresh(winMsg);
}

/* dispVal(DisplayVal id, int16_t val)
 * Author: 		Sergio Soto
 * Function: 	Update the display if any of the displayed values of the drone changes
 * IN:			DisplayVal id - Indicator for what to be changed
 				int16_t val -  The value of the corresponding parameter to be updated
*/
void dispVal(DisplayVal id, int16_t val) {
	switch(id) {
		case DISP_LIFT:
			liftBuff = val;
			break;
		case DISP_ROLL:
			rollBuff = val;
			break;
		case DISP_PITCH:
			pitchBuff = val;
			break;
		case DISP_YAW:
			yawBuff = val;
			break;
		case DISP_AE0:
			aeBuff[0] = val;
			break;
		case DISP_AE1:
			aeBuff[1] = val;
			break;
		case DISP_AE2:
			aeBuff[2] = val;
			break;
		case DISP_AE3:
			aeBuff[3] = val;
			updateMotorDisp();
			break;
		case DISP_P:
			pBuff = val;
			break;
		case DISP_Q:
			qBuff = val;
			break;
		case DISP_R:
			rBuff = val;
			break;
		case DISP_AX:
			axBuff = val;
			break;
		case DISP_AY:
			ayBuff = val;
			break;
		case DISP_AZ:
			azBuff = val;
			updateValuesDisp();
			break;
		case DISP_PHI:
			phiBuff = val;
			break;
		case DISP_THE:
			theBuff = val;
			break;
		case DISP_PSI:
			psiBuff = val;
			break;
		case DISP_BATT:
			battBuff = val;
			break;
		case DISP_TEMP:
			tempBuff = val;
			break;
		case DISP_PRESS:
			pressBuff = val;
			updateValuesDisp();
			break;
		case DISP_MODE:
			modeBuff = (char)val;
			updateModeDisp();
			break;
	}
}

/* dispValP(DisplayVal id, uint32_t val)
 * Author: 		Prashanth G L
 * Function: 	Update the display if pressure values of the drone changes
 				(separate function to display pressure as it cannot be displayed with 16 bit variable)
 * IN:			DisplayVal id - Indicator for what to be changed (pressure here)
 				uint32_t val -  The value of the pressure to be updated
*/
void dispValP(DisplayVal id, uint32_t val){
	if (id == DISP_PRESS)
	 {
	 	pressBuff = val;
		updateValuesDisp();
	 } 

}

/* Read the UART input */
int readCommand() {

	timeout(0);
	return getch();
}
