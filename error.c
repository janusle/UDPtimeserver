#include "error.h"

void 
err_quit(bool u)
{
  if( errno != 0 )  
   perror("time-server"); 

  if( u )
  {
    usage();
  }

  exit(-1);
}

void 
usage()
{
  fprintf(stderr, "Usage: time-server configfile\n"); 
}
