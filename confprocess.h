#include<string.h>
#include<stdlib.h>
#include<stdio.h>
#include<ctype.h>
#include "error.h"

#define LINENUM 200
#define ARRAYLEN 2
#define LEN 150

#define ADDRESS 0
#define PORT 1
#define PRINT_MSG 2
#define SUP_TIMEOUT 3

#define TIMES "3"
#define TIMEOUT "3"
#define CONFLEN 4
#define OPTION 0
#define VALUE 1
#define ON 1
#define OFF 0

char** explode( char* line );

void* Malloc( size_t size );

void init( char* filename, char* config[] );




