#ifndef REDIS_H_
#define REDIS_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hiredis.h"
#include "pc_terminal.h"

redisContext *context;
redisReply *reply;

void setRedisConnection();
void sendMotorRedis(int16_t m0, int16_t m1, int16_t m2, int16_t m3);
void sendSensorRedis(int16_t phi, int16_t theta, int16_t psi, int16_t sp, int16_t sq, int16_t sr, int16_t sax, int16_t say, int16_t saz);
void sendAdcRedis(int16_t battery, int16_t temperature, int32_t pressure);
void sendRPYLRedis(int16_t roll, int16_t pitch, int16_t yaw, int16_t lift);
void sendModeRedis(char currentState);

#endif // REDIS
