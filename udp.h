#include<stdio.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<sys/types.h>
#include<sys/uio.h>
#include<unistd.h>
#include<strings.h>
#include<string.h>
#include<netdb.h>
#include<sys/select.h>

#include "error.h"

#define CONNECTED 1
#define DISCONNECTED 0
#define LOGGED 1
#define UNLOGGED 0
#define NUMOFBYTES 16
#define SUCCESS 0
#define FAILURE -1
#define MAGICNUM 0xA3F0
#define REPLY 0xB4
#define REQUEST 0x52
#define TIMEZONELEN 4
#define SENDLOG "sendlog"
#define RECVLOG "recvlog"
#define TMPLEN 30

typedef struct 
{
  uint16_t mesgType;
  unsigned char status;
  unsigned char second;
  unsigned char minute;
  unsigned char hour;
  unsigned char day;
  unsigned char month;
  uint32_t year;
  unsigned char timezone[4];
}binarydata; 


typedef struct sockaddr SA;

typedef struct sockaddr_in SAI;


int Socket( int family, int type, int protocol);

int senddata( int fd, void *data, int size,
             const struct sockaddr *servaddr, socklen_t addrlen);

int clientinit( char* hostname, char* port, SAI** sock_addr);

int receive( int sockfd, void *data , SAI* sock_addr);

int request( int sockfd, SAI* sock_addr, int logged);

void Request( int sockfd, SAI* sock_addr, int logged);

int getreply( int sockfd, int logged, int timeout);

void do_udp( char* hostname, char* port, int logged, int times, int timeout);


