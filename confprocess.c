#include "confprocess.h"


char**
explode( char* line )
{
  static char* array[ARRAYLEN];
  int i, size;

  array[0] = (char*)Malloc( sizeof(char) * LEN );
  array[1] = (char*)Malloc( sizeof(char) * LEN );

  size = strlen(line);
  for( i=0; !isspace(line[i]) && i<size ;i++)
  ;

  memcpy( array[0], line, i );

  for(; isspace(line[i]) && i<size; i++)
  ;

  strcpy( array[1], line+i);

  /*
  strcpy(array[0], strtok( line, " " ));
  strcpy(array[1], strtok( NULL, " " ));
  */ 
  return array;
}


void*
Malloc( size_t size )
{
  void *tmp;

  if( (tmp=malloc( size)) == NULL )
  {
     err_quit(false); 
  }
  
  return tmp;

}


static void 
trim( char* line )
{
   char *st;
   int size, i;
   
   size = strlen(line);

   /* skip space */
   for(i=0; i<size && isspace(line[i]); i++)
   ;

   /* all the items in the line is space */
   if( i >= size )
    line[0] = '\0';
   
   /* cut the space in the front */
   st = (char*)Malloc( sizeof(char)*LEN); 
   strcpy( st, line+i );
   
   size = strlen(st);
  
   
   for( i = size-1; isspace(st[i]) && i>=0 ; i-- ) 
   ;     
   
   st[i+1] = '\0';
   

   strcpy(line, st);
}


static char* 
getl( char *line, FILE* fd )
{
   char c; 
   int i;
    
   i=0;
   while( ( ( c = fgetc(fd)) != '\n' &&  c != '\r' ) && c != EOF )
   {
      line[i++] = c;
   }

   if( c == EOF )
     return NULL;

   line[i] = '\0';
  
   trim( line );    

   return line;
}


void
init( char* filename , char* config[] )
{
  FILE* fd;
  char *line, **tmp;
  int i;

  line = (char*)Malloc( sizeof(char) * LINENUM);

  fd = fopen(filename, "r");
  if( fd == NULL )
  {
     err_quit(false); 
  }

  for(i=0; i<CONFLEN; i++)
    config[i] = NULL;

  while( (getl(line,  fd ) != NULL) )
  {

    if( isalpha(line[0]) )
    {

      tmp = explode( line );
      if( strcmp( "address", tmp[OPTION] ) == 0 )
      {
        if( config[ADDRESS] == NULL ) 
        {
          config[ADDRESS] = 
            (char*)Malloc( sizeof(char) * strlen( tmp[VALUE] ));
        }

        strcpy( config[ADDRESS], tmp[VALUE] );
      }


      if( strcmp( "port", tmp[OPTION] ) == 0 )
      {
        
        if( config[PORT] == NULL ) 
        {
          config[PORT] = 
            (char*)Malloc( sizeof(char) * strlen( tmp[VALUE] ));
        }

        strcpy( config[PORT], tmp[VALUE] );
      }

      if( strcmp( "print_message_details", tmp[OPTION] ) == 0 )
      { 
        
        if( config[PRINT_MSG] == NULL ) 
        {
          config[PRINT_MSG] = 
            (char*)Malloc( sizeof(char) * strlen( tmp[VALUE] ));
        }

        strcpy( config[PRINT_MSG], tmp[VALUE] );
      }
     
      if( strcmp( "support_timeout" , tmp[OPTION] ) == 0 )
      {

        if( config[SUP_TIMEOUT] == NULL ) 
        {
          config[SUP_TIMEOUT] = 
            (char*)Malloc( sizeof(char) * strlen( tmp[VALUE] ));
        }

        strcpy( config[SUP_TIMEOUT], tmp[VALUE] );
      }

      /*
      printf("%s %s\n", tmp[OPTION], tmp[VALUE]);
      */
    }
  }

  /* set default value */
  if( config[ADDRESS] == NULL || config[PORT] == NULL )
  {
    fprintf(stderr, "Bad format in config file\n");
    exit(-1);
  }


  if( config[PRINT_MSG] == NULL )
  {
    config[PRINT_MSG] = (char*)Malloc( sizeof(char)*LEN );
    strcpy( config[PRINT_MSG], "ON");
  }


  if( config[SUP_TIMEOUT] == NULL )
  {
    config[SUP_TIMEOUT] = (char*)Malloc( sizeof(char)*LEN );
    strcpy( config[SUP_TIMEOUT], "ON");
  }


  fclose(fd); 
}

