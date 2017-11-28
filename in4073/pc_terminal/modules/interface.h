#ifndef INTERFACE_H__
#define INTERFACE_H__

#include "drone-state.h"
#include <stdarg.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <ncurses.h>
#include <string.h>
#include <inttypes.h>

typedef enum Screen_t {
	SCREEN_INTRO,
	SCREEN_CONTINUE,
	SCREEN_MAIN
} Screen;
         
char waitForUser;
         
void initInterface();
void closeInterface();
int readCommand();
void dispMsg(const char* format, ... );
void dispVal(DisplayVal id, int16_t val);
void dispValP(DisplayVal id, uint32_t val);

#endif /* INTERFACE_H__ */