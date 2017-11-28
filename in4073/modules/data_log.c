#include "data_log.h"

/* data_log_init()
 * Author: 		Priadi
 * Function: 	Initialize flags for data logging
*/
void data_log_init()
{
	// NOTE : Total Flash memory 131072 Bytes

	flag_memory_full = 0;
	flag_logging_done = 1;
}

/* log_one_packet(uint32_t* address)
 * Author: 		Priadi
 * Function: 	Log one packet of data (timestamp,sax, say, saz, sp, sq, sr)
 * IN:			uint32_t* address, 32 bit starting address to log the data to flash memory
*/
bool log_one_packet(uint32_t* address)
{

	// make sure and the remaining memory fit for a packet data to be stored
	if( (FLASH_LAST_ADDRESS - *address < TOTAL_LOG_BYTES) )
	{
		flag_memory_full = 1;
		*address = FLASH_START_ADDRESS;

		// tell PC that the logging is done
		flag_logging_done = 1;
		sendMessageEs(MSG12_NOTIFY_WRITE_FLASH);

		return true;
	}

	// store system time, sax, say, saz, sp, sq and sr
	packet_log[0] = (system_time & 0xFF000000) >> 24;
	packet_log[1] = (system_time & 0x00FF0000) >> 16;
	packet_log[2] = (system_time & 0x0000FF00) >> 8;
	packet_log[3] = system_time & 0x000000FF;
	packet_log[4] = (sax & 0xFF00) >> 8;
	packet_log[5] = sax & 0x00FF;
	packet_log[6] = (say & 0xFF00) >> 8;
	packet_log[7] = say & 0x00FF;
	packet_log[8] = (saz & 0xFF00) >> 8;
	packet_log[9] = saz & 0x00FF;
	packet_log[10] = (sp & 0xFF00) >> 8;
	packet_log[11] = sp & 0x00FF;
	packet_log[12] = (sq & 0xFF00) >> 8;
	packet_log[13] = sq & 0x00FF;
	packet_log[14] = (sr & 0xFF00) >> 8;
	packet_log[15] = sr & 0x00FF;

/* Just for debuggin purpose */
	// packet_log[8] = (ae[0] & 0xFF00) >> 8;
	// packet_log[9] = ae[0] & 0x00FF;
	// packet_log[10] = (ae[1] & 0xFF00) >> 8;
	// packet_log[11] = ae[1] & 0x00FF;
	// packet_log[12] = (ae[2] & 0xFF00) >> 8;
	// packet_log[13] = ae[2] & 0x00FF;
	// packet_log[14] = (ae[3] & 0xFF00) >> 8;
	// packet_log[15] = ae[3] & 0x00FF;
	// packet_log[14] = (theta & 0xFF00) >> 8;
	// packet_log[15] = theta & 0x00FF;

/* write one packet to flash */
	while(!flash_write_bytes(*address,&packet_log[0],TOTAL_LOG_BYTES));
	*address += TOTAL_LOG_BYTES;
	return true;
}

/* read_log_data(uint32_t start_address)
 * Author: 		Priadi
 * Function: 	Read full flash data in terms of a packet(timestamp,sax, say, saz, sp, sq, sr) and send to PC
 * IN:			uint32_t* start_address, 32 bit starting address to read the flash data
*/
bool read_log_data(uint32_t start_address)
{
	uint32_t address = start_address;

	while (address < FLASH_LAST_ADDRESS - TOTAL_LOG_BYTES)
	{
		for(int i=0;i<TOTAL_LOG_BYTES;i++)
		{
			flash_read_bytes(address++,&txBuff[i],TOTAL_LOG_BYTES);
		}
		sendMessageEs(MSG11_SEND_LOG_DATA);
	}

	return true;
}

/* write_log_data()
 * Author: 		Priadi
 * Function: 	Change flags to indicate the flash data logging is done 
*/
void write_log_data()
{
	flash_chip_erase();
	flag_memory_full = 0;
	flag_logging_done = 0;
}
