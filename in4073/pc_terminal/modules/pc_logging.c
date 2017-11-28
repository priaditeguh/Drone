#include "pc_logging.h"

/* parsePacketLog()
 * Author: 		Priadi
 * Function: 	Parse one packet of data (timestamp, sax, say, saz, sp, sq and sr)
*/

void parsePacketLog()
{
  system_time = (rxBuff[0] << 24)  | (rxBuff[1] << 16) | (rxBuff[2] << 8) | rxBuff[3];

  sax = (rxBuff[4] << 8) | rxBuff[5];
  say = (rxBuff[6] << 8) | rxBuff[7];
  saz = (rxBuff[8] << 8) | rxBuff[9];

  sp = (rxBuff[10] << 8) | rxBuff[11];
  sq = (rxBuff[12] << 8) | rxBuff[13];
  sr = (rxBuff[14] << 8) | rxBuff[15];
}

/* storeLogInTxt(FILE *file)
 * Author: 		Priadi
 * Function: 	Store one packet of data (timestamp, sax, say, saz, sp, sq and sr) in one line of text file
*/

void storeLogInTxt(FILE *file)
{
  fprintf(file,"%d %d %d %d %d %d %d\n",system_time,sax,say,saz,sp,sq,sr);
}

