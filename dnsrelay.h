#ifndef DNSRELAY_H
#define DNSRELAY_H
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include <string.h>
#include"table.h"
#include"ID.h"
#ifdef _WIN32
#include <WinSock2.h>
#pragma comment(lib,"ws2_32.lib")
WSADATA wsaData;
SOCKET Sock;
struct sockaddr_in inaddr, server;
#endif
#define BUFSIZE 1024
int addr_len = sizeof(struct sockaddr_in);
uint16_t PORT = 53;
static char DNS_IP[16] = "192.168.110.1";
//DNS报文头部
typedef struct HEADER
{
    unsigned id : 16;    
    unsigned rd : 1;     
    unsigned tc : 1;     
    unsigned aa : 1;     
    unsigned opcode : 4; 
    unsigned qr : 1;     
    unsigned rcode : 4;  
    unsigned cd : 1;     
    unsigned ad : 1;     
    unsigned z : 1;      
    unsigned ra : 1;     
    uint16_t qdcount;    
    uint16_t ancount;    
    uint16_t nscount;    
    uint16_t arcount;    
}HEADER;

typedef struct Question
{
    char qname[256];
    uint16_t qtype;
    uint16_t qclass;
}Question;

typedef struct ResourceRecord
{
    uint16_t name;
    uint16_t type;
    uint16_t class;
    uint16_t ttl;
    uint16_t rd_length;
    uint32_t rdata;
}ResourceRecord;
static const int A_NAME_OFFSET = 0xC00C;
static const uint16_t A_TYPE = 1;
static const uint16_t IN_CLASS = 1;
static const uint16_t A_RDLENGTH = 4;

#endif // !DNSRELAY_H
void init_socket();
char* getquestion(Question* question, char buf[]);
uint16_t get16bit(uint8_t** buf);
void get_debug(int argc, char* argv[]);
int debug_level = 1; 
