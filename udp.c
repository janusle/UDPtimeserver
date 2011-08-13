#include "udp.h"
#include "error.h"

extern int errno;


static char*
getip( SAI* servaddr )
{
  static char st[TMPLEN];

  inet_ntop( AF_INET, &(servaddr->sin_addr), st, INET_ADDRSTRLEN );

  return st;

}


int 
Socket( int family, int type, int protocol )
{
  int sockfd;
  sockfd = socket( family, type, protocol );
  if( sockfd < 0 )
  {
     err_quit(false);
  }
  
  return sockfd;
}


static int 
checkdata( binarydata *data )
{
   if( data->second <=59 &&
       data->minute <=59 &&
       data->hour <=23 &&
       data->day >=1 && data->day <=31 &&
       data->month >=1 && data->month <=12 &&
       data->year >=1 && data->year <=9999 
     )
   {
      return true;
   }

   return false;
}


int 
receive( int sockfd, void *data, SAI* sock_addr)
{
   FILE *lfd; 
   binarydata *ptr;
   int n, addrlen;

   ptr = (binarydata*)data;

   n = recvfrom( sockfd, data, NUMOFBYTES ,0 ,(SA*)sock_addr, &addrlen  );
 
   /* for test */
   /*
   printf("Recv: %d\n", n);
   */


   if( n != NUMOFBYTES )
      return FAILURE;

   if( ptr->mesgType != MAGICNUM  )    
      return FAILURE;  
  
   if( memcmp( (const char*)ptr->timezone, "AEST", TIMEZONELEN) != 0 )
      return FAILURE;


   lfd = fopen(RECVLOG, "a"); 


   fprintf(lfd, "%x\n%x\n%d\n%d\n%d\n%d\n%d\n%d\n%c%c%c%c\n",
             ptr->mesgType, ptr->status, ptr->second,
             ptr->minute, ptr->hour, ptr->day, 
             ptr->month, ptr->year, ptr->timezone[0],
             ptr->timezone[1], ptr->timezone[2],ptr->timezone[3]);


     /*for test*/
     /*
     printf("%x\n%x\n%d\n%d\n%d\n%d\n%d\n%d\n%c%c%c%c\n",
             ptr->mesgType, ptr->status, ptr->second,
             ptr->minute, ptr->hour, ptr->day, 
             ptr->month, ptr->year, ptr->timezone[0],
             ptr->timezone[1], ptr->timezone[2],ptr->timezone[3]);
     */
   fclose(lfd);


   if( checkdata(ptr) == false )
      return FAILURE;

   return SUCCESS; 
}


static char*
printtime( binarydata *data )
{
  static char tmp[TMPLEN];
  sprintf( tmp, "%d:%d:%d %d-%d-%d %c%c%c%c\n",
            data->hour, data->minute, data->second,
            data->day, data->month, data->year,
            data->timezone[0], data->timezone[1],
            data->timezone[2], data->timezone[3]);
  return tmp;


}


static int 
settimeout( int fd, int sec )
{
  struct timeval t;
  fd_set rest;

  t.tv_sec = sec;
  t.tv_usec = 0;
  
  /*FD_ZERO(&rest);*/

  /*bzero(&rest, sizeof(rest));*/
  FD_SET(fd, &rest);
  return select(fd+1, &rest, NULL,NULL,&t);
}


int 
getreply( int sockfd, int logged, int timeout )
{
   binarydata data; 
   FILE *lfd;
   SAI *sock_addr;  
   
   sock_addr = (SAI*)malloc(sizeof(SAI));

   bzero( sock_addr, sizeof(SAI) );

   if( settimeout( sockfd, timeout ) <= 0 )
   {
     fprintf(stdout, "timeclient: timeout-no reply\n");
     lfd = fopen(RECVLOG, "a");
     fprintf(lfd,"timeclient: timeout-no reply\n\n");
     fclose(lfd);
     return FAILURE;
   }

   if ( receive( sockfd, &data, sock_addr) == FAILURE )
   {
       
     lfd = fopen(RECVLOG, "a");
     fprintf(lfd,"timeclient: reply from %s:%d is invalid\n\n\n", getip(sock_addr),
             sock_addr->sin_port);
     fclose(lfd);
  

     /* log */
     if( logged )
      fprintf(stderr,"timeclient: reply from %s:%d is invalid\n", getip(sock_addr),
             sock_addr->sin_port);


     return FAILURE;
   }


  
   
   lfd = fopen(RECVLOG, "a");
   fprintf(lfd,"timeclient: reply from %s:%d is good\n", getip(sock_addr),
             sock_addr->sin_port);

   /* log */
   fprintf(lfd,"%s\n", printtime(&data));    
   fclose(lfd);

   if( logged )
   {
    fprintf(stderr,"timeclient: reply from %s:%d is good\n", getip(sock_addr),
             sock_addr->sin_port);
    fprintf(stderr,"%s", printtime(&data));    
   } 

   return SUCCESS;
}


int 
clientinit( char* hostname , char* port ,SAI** sock_addr )
{
  struct addrinfo hints, *res;
  int sockfd;

  bzero(&hints, sizeof(hints));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_DGRAM;
  
  if( getaddrinfo( hostname, port, &hints, &res) != 0 )
  {
     fprintf(stderr, "Cannot resolve hostname\n");
     err_quit(false);
  }

  *sock_addr = (SAI*)res->ai_addr;

  sockfd = Socket( AF_INET, SOCK_DGRAM, 0);
  
  return sockfd;

}


int 
senddata( int fd, void *data,int size ,
      const struct sockaddr *servaddr, socklen_t addrlen)
{
     FILE *lfd; 
     binarydata *ptr;
     /*for test*/

     ptr = (binarydata*)data;


     if ( (sendto( fd, data, NUMOFBYTES, 0, servaddr, addrlen ))  < size )
     {
       return FAILURE; 
     }

     /* for test 
     printf("Send: %d\n", n);
     */

    lfd = fopen(SENDLOG, "a");
    fprintf(lfd, "%x\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%c%c%c%c\n\n",
               ptr->mesgType, ptr->status, ptr->second,
               ptr->minute, ptr->hour, ptr->day, 
               ptr->month, ptr->year, ptr->timezone[0], ptr->timezone[1],
               ptr->timezone[2], ptr->timezone[3]);
    fclose(lfd);

       /* for test */
       /*
       printf("Send data: \n");
       fprintf(stdout, "%x\n%x\n%d\n%d\n%d\n%d\n%d\n%d\n%c%c%c%c\n\n",
               ptr->mesgType, ptr->status, ptr->second,
               ptr->minute, ptr->hour, ptr->day, 
               ptr->month, ptr->year, ptr->timezone[0], ptr->timezone[1],
               ptr->timezone[2],ptr->timezone[3]);
       */
  
     return SUCCESS;
}


int 
request( int sockfd, SAI* sock_addr, int logged )
{
   binarydata req;
   FILE *lfd;

   /* init request data */
   bzero( &req, sizeof(req) );
   req.mesgType = MAGICNUM;
   req.status = 0x52;
   memcpy( req.timezone, "AEST", TIMEZONELEN);
 

   if( logged )
    fprintf(stderr,"timeclient: request to %s:%d\n", getip((SAI*)sock_addr),
             ((SAI*)sock_addr)->sin_port);


   lfd = fopen(SENDLOG, "a");
   fprintf(lfd, "timeclient: request to %s:%d\n", getip((SAI*)sock_addr),
               ((SAI*)sock_addr)->sin_port);

   fclose(lfd);
   return senddata( sockfd, &req, sizeof(req),  (SA*)sock_addr,
             sizeof(*sock_addr) );

}


void
Request( int sockfd, SAI* sock_addr, int logged )
{
   if( request( sockfd, sock_addr, logged ) == -1 )
   {
      err_quit(false); 
   }
}


void 
do_udp( char* hostname, char* port, int logged, int times, int timeout)
{
  int sockfd, i;
  SAI* sock_addr;

  for( i=0; i<times; i++ )
  {
    sockfd = clientinit( hostname, port, &sock_addr );
    Request( sockfd, sock_addr, logged );
    if( getreply( sockfd, logged ,timeout) == SUCCESS ) 
      break;
  }

}


