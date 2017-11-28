#ifndef LOGGING_PC_
#define LOGGING_PC_

#include "pc_terminal.h"
#include "../comm-prot/sm/uart-sm.h"
#include <stdio.h>
#include <inttypes.h>

#define TOTAL_LOG_BYTES 16

uint32_t system_time;
int16_t sp;
int16_t sq;
int16_t sr;
int16_t sax;
int16_t say;
int16_t saz;

void parsePacketLog();
void storeLogInTxt(FILE *file);

#endif
