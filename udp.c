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
   if( data->second == 0 &&
       data->minute == 0 &&
       data->hour == 0 &&
       data->day == 0 && 
       data->month == 0 && 
       data->year == 0 &&
       data->status == 0x52
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
   int n;
   unsigned addrlen;
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


int 
getrequest( int sockfd, SAI* sock_addr, int logged )
{
   binarydata data; 
   FILE *lfd;

   if ( receive( sockfd, &data, sock_addr) == FAILURE )
   {
       
     lfd = fopen(RECVLOG, "a");
     fprintf(lfd,"timeserver: invalid request from %s:%d \n\n\n", 
             getip(sock_addr),
             sock_addr->sin_port);
     fclose(lfd);
  

     /* log */
     if( logged )
      fprintf(stderr,"timeserver: invalid request from %s:%d\n", 
             getip(sock_addr),
             sock_addr->sin_port);


     return FAILURE;
   }

 
   lfd = fopen(RECVLOG, "a");
   fprintf(lfd,"timeserver: valid request from %s:%d\n", 
             getip(sock_addr),
             sock_addr->sin_port);

   /* log */
   fclose(lfd);

   if( logged )
   {
    fprintf(stderr,"timeserver: valid request from %s:%d\n", 
             getip(sock_addr),
             sock_addr->sin_port);
   } 

   return SUCCESS;
}


void
Bind( int sockfd, SA* sockaddr, socklen_t len )
{
   if( bind( sockfd, sockaddr, len ) == -1 )
   {
      err_quit(false); 
   }
}


void
Inet_pton( int family, char*strptr, void *addrptr)
{
   if( inet_pton( family, (const char*)strptr,
                  addrptr ) == -1 )
   {
      err_quit(false);
   }
}

int 
serverinit( char* address, int port )
{
  SAI servaddr;
  int sockfd;

  sockfd = Socket(AF_INET, SOCK_DGRAM, 0);
  bzero(&servaddr, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  Inet_pton(AF_INET, address, &servaddr.sin_addr);
  servaddr.sin_port = htons(port);
  
  Bind( sockfd, (SA*)&servaddr, sizeof(servaddr));
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


static int 
randomint( int range )
{
  srand((unsigned)time(NULL));
  return rand()%range;
}


static int 
gentime( binarydata* req )
{
  int ran;
  time_t timer;
  struct tm* t;

  ran = randomint( RANNUM );
  switch(ran)
  {
    case 0:
      /* valid data */
      req->second = 56;
      req->minute = 34;
      req->hour = 12;
      req->day = 13;
      req->month = 12;
      req->year = 2011;
      break;
    case 1:
      /* invalid data */
      req->second = 89;
      req->minute = 78;
      req->hour = 26;
      req->day = 45;
      req->month = 14;
      req->year = 2006;
      break;
    case 2:
      /* correct time-of-day*/
      timer = time(NULL);
      t = localtime(&timer);
      if( t == NULL )
      {
         err_quit(false);
      }
      req->second = (unsigned char)t->tm_sec;
      req->minute = (unsigned char)t->tm_min;
      req->hour = (unsigned char)t->tm_hour;
      req->day = (unsigned char)t->tm_mday;
      req->month = (unsigned char)t->tm_mon;
      req->year = (unsigned char)t->tm_year;
      break;
    case 3:
      /* time out */
      return -1;
      break;
    default:
      fprintf(stderr,"Fail to generate random number");
      err_quit(false);
  }
  return 0;
}


int 
reply( int sockfd, SAI* sock_addr, int logged )
{
   binarydata req;
   FILE *lfd;

   /* init reply data */
   bzero( &req, sizeof(req) );
   req.mesgType = MAGICNUM;
   req.status = REPLY;
   memcpy( req.timezone, "AEST", TIMEZONELEN);

   /* if return value is -1, ignore request */
   if ( gentime( &req ) == -1 )
     return SUCCESS;

   if( logged )
    fprintf(stderr,"timeclient: replying to %s:%d\n", 
            getip((SAI*)sock_addr),
            ((SAI*)sock_addr)->sin_port);


   lfd = fopen(SENDLOG, "a");
   fprintf(lfd, "timeclient: replying to %s:%d\n", 
           getip((SAI*)sock_addr),
           ((SAI*)sock_addr)->sin_port);

   fclose(lfd);
   return senddata( sockfd, &req, sizeof(req),  (SA*)sock_addr,
             sizeof(*sock_addr) );

}


void
Reply( int sockfd, SAI* sock_addr, int logged )
{
   if( reply( sockfd, sock_addr, logged ) == -1 )
   {
      err_quit(false); 
   }
}


void 
do_udp( char* address, int port, int logged, int sup_timeout )
{
  int sockfd;
  SAI *sock_addr;   
  sockfd = serverinit(address, port); 

  sock_addr = (SAI*)malloc(sizeof(SAI));
  bzero( sock_addr, sizeof(SAI) );

  while(true)
  {
     if( getrequest( sockfd, sock_addr, logged ) != FAILURE )  
     {
        Reply( sockfd, sock_addr, logged ); 
     }
  }
  /*
  for( i=0; i<times; i++ )
  {
    sockfd = clientinit( hostname, port, &sock_addr );
    Request( sockfd, sock_addr, logged );
    if( getreply( sockfd, logged ,timeout) == SUCCESS ) 
      break;
  }
  */
}


