#ifndef DATA_LOG_H_
#define DATA_LOG_H_

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "in4073.h"

#define TOTAL_LOG_BYTES 16
#define FLASH_LAST_ADDRESS 0x01FFFF
#define FLASH_START_ADDRESS 0x000000

uint8_t flag_memory_full;
uint8_t packet_log[TOTAL_LOG_BYTES];
bool flag_logging_done;

void data_log_init();
bool log_one_packet(uint32_t* address);
bool read_log_data(uint32_t start_address);
void write_log_data();
#endif // DATA_LOG
