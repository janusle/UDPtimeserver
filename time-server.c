#include "udp.h"
#include "error.h"
#include "confprocess.h"


int 
main(int argc, char** argv )
{
  
  if( argc < 2 )
  {
     fprintf(stderr, "Too few arguments\n");
     err_quit(true);
  }
  

  return 0;
}
