#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>
#include <stdint.h>
typedef struct ID {
    uint8_t flag;
    uint32_t clientIP;
    uint16_t clientPort;
    uint16_t clientID;
    time_t inTime;
}ID;
ID idspool[65536];
static uint16_t Nid = 0;
uint16_t applyid(uint32_t clientIP, uint16_t clientPort, uint16_t clientID);
void deleteid(uint16_t serverid);