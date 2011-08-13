#include "udp.h"
#include "error.h"
#include "confprocess.h"

#define SERV_PORT 20202

int 
main(int argc, char** argv )
{

  if( argc < 2 )
  {
     fprintf(stderr, "Too few arguments\n");
     err_quit(true);
  }
 
  do_udp( "0.0.0.0", 20202, LOGGED, 1);

  return 0;
}
