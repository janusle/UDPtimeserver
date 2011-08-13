#include<string.h>
#include<stdlib.h>
#include<stdio.h>
#include<ctype.h>
#include "error.h"

#define LINENUM 200
#define ARRAYLEN 2
#define LEN 150

#define SERVER_ADDRESS 0
#define SERVER_NAME 1
#define SERVER_PORT 2
#define PRINT_MSG 3
#define REQ_COUNT 4
#define REQ_TIMEOUT 5

#define TIMES "3"
#define TIMEOUT "3"
#define CONFLEN 6
#define OPTION 0
#define VALUE 1

char** explode( char* line );

void* Malloc( size_t size );

void init( char* filename, char* config[] );




