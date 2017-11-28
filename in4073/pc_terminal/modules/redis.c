#include "redis.h"

/* setRedisConnection()
 * Author:      Priadi
 * Function:    Initiating the connection with local database
*/
void setRedisConnection()
{
	const char *hostname = "127.0.0.1";
    int port = 6379;

    struct timeval timeout = { 1, 500000 }; // 1.5 seconds
    context = redisConnectWithTimeout(hostname, port, timeout);
    if (context == NULL || context->err) {
        if (context) {
            dispMsg("Connection error: %s", context->errstr);
            redisFree(context);
        } else {
            dispMsg("Connection error: can't allocate redis context");
        }
        exit(1);
    }
}

/* sendMotorRedis()
 * Author:      Priadi
 * Function:    Updating motor value in database
*/
void sendMotorRedis(int16_t m0, int16_t m1, int16_t m2, int16_t m3)
{
    // send motor value to Redis
    reply = redisCommand(context,"SET AE0 %d", m0);
    freeReplyObject(reply);
    reply = redisCommand(context,"SET AE1 %d", m1);
    freeReplyObject(reply);
    reply = redisCommand(context,"SET AE2 %d", m2);
    freeReplyObject(reply);
    reply = redisCommand(context,"SET AE3 %d", m3);
    freeReplyObject(reply);
}

/* sendSensorRedis()
 * Author:      Priadi
 * Function:    Updating sensor value in database
*/
void sendSensorRedis(int16_t phi, int16_t theta, int16_t psi, int16_t sp, int16_t sq, int16_t sr, int16_t sax, int16_t say, int16_t saz)
{
    // send sensor data to Redis
    reply = redisCommand(context,"SET PHI %d", phi);
    freeReplyObject(reply);
    reply = redisCommand(context,"SET THETA %d", theta);
    freeReplyObject(reply);
    reply = redisCommand(context,"SET PSI %d", psi);
    freeReplyObject(reply);
    reply = redisCommand(context,"SET SP %d", sp);
    freeReplyObject(reply);
    reply = redisCommand(context,"SET SQ %d", sq);
    freeReplyObject(reply);
    reply = redisCommand(context,"SET SR %d", sr);
    freeReplyObject(reply);
    reply = redisCommand(context,"SET SAX %d", sax);
    freeReplyObject(reply);
    reply = redisCommand(context,"SET SAY %d", say);
    freeReplyObject(reply);    
    reply = redisCommand(context,"SET SAZ %d", saz);
    freeReplyObject(reply);   
}

/* sendAdcRedis()
 * Author:      Priadi
 * Function:    Updating battery,temperature and pressure value in database
*/
void sendAdcRedis(int16_t battery, int16_t temperature, int32_t pressure)
{
    // send battery temperature and pressure to Redis
    reply = redisCommand(context,"SET BATT %d", battery);
    freeReplyObject(reply);
    reply = redisCommand(context,"SET TEMP %d", temperature);
    freeReplyObject(reply);    
    reply = redisCommand(context,"SET PRESS %d", pressure);
    freeReplyObject(reply);  
}

/* sendRPYLRedis()
 * Author:      Priadi
 * Function:    Updating Roll,Pitch,Yaw,Lift value in database
*/
void sendRPYLRedis(int16_t roll, int16_t pitch, int16_t yaw, int16_t lift)
{
        // send Roll Pitch Yaw Lift to Redis
    reply = redisCommand(context,"SET ROLL %d", rollBuff);
    freeReplyObject(reply);
    reply = redisCommand(context,"SET PITCH %d", pitchBuff);
    freeReplyObject(reply);    
    reply = redisCommand(context,"SET YAW %d", yawBuff);
    freeReplyObject(reply);  
    reply = redisCommand(context,"SET LIFT %d", liftBuff);
    freeReplyObject(reply);  
}

/* sendModeRedis()
 * Author:      Priadi
 * Function:    Updating board current state in database
*/
void sendModeRedis(char currentState)
{
    // send current mode to Redis
    reply = redisCommand(context,"SET MODE %d", currentState);
}