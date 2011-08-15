#include "udp.h"
#include "error.h"
#include "confprocess.h"

#define SERV_PORT 20202

int 
main(int argc, char** argv )
{
  char* config[CONFLEN];
  int logged, sup_timeout, port;

  if( argc < 2 )
  {
     fprintf(stderr, "Too few arguments\n");
     err_quit(true);
  }

  init( argv[1] , config );

  port = atoi( config[PORT] );
  if( port == 0 )
  {
    fprintf(stderr, "invalid port\n");
    exit(-1);
  }

  if( strcmp( config[PRINT_MSG], "ON" ) == 0)
    logged = LOGGED;
  else
    logged = UNLOGGED;
  
  if( strcmp( config[SUP_TIMEOUT], "ON") == 0)
    sup_timeout = ON;
  else
    sup_timeout = OFF;

  do_udp( config[ADDRESS], port, logged, sup_timeout );  

  
  /*
  int i;
  for( i=0; i<CONFLEN; i++)
  {
    printf("%s\n", config[i] );

  }
  i*/
  /*do_udp( "0.0.0.0", 20202, LOGGED, 0);*/

  return 0;
}
