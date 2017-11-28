/* 
Variable declaration and initialization required for the user interface
*/

#include "drone-state.h"
#include <inttypes.h>

int16_t liftBuff = 0;
int16_t rollBuff = 0;
int16_t pitchBuff = 0;
int16_t yawBuff = 0;
int8_t fireButton=0;
int16_t aeBuff[4] = {0, 0, 0, 0};

int16_t pBuff = 0;
int16_t qBuff = 0;
int16_t rBuff = 0;

int16_t axBuff = 0;
int16_t ayBuff = 0;
int16_t azBuff = 0;

int16_t phiBuff = 0;
int16_t theBuff = 0;
int16_t psiBuff = 0;

int16_t battBuff = 0;
int16_t tempBuff = 0;
int32_t pressBuff = 0;

char modeBuff = 0;

// Displayed data:
char boardCurrState = 0;
char boardNextState = 0;
