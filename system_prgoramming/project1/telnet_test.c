#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

#define  BUFF_SIZE   99999

int   main( int argc, char **argv)
{
      int   client_socket;

      struct sockaddr_in   server_addr;

      char   buff[BUFF_SIZE];
      char url_address[100];

      client_socket  = socket( PF_INET, SOCK_STREAM, 0);

      if( -1 == client_socket)
        {
            printf( "socket 생성 실패\n");
            exit( 1);
        }
      printf("url_address is %s \n", argv[1]);
      printf("port number is %s \n", argv[2]);
    
      memset( &server_addr, 0, sizeof( server_addr));
      server_addr.sin_family     = AF_INET;
      server_addr.sin_port       = htons(atoi(argv[2]));
      server_addr.sin_addr.s_addr= inet_addr(argv[1]);

      if( -1 == connect( client_socket, (struct sockaddr*)&server_addr, sizeof( server_addr) ) )
      {
         printf( "접속 실패\n");
         exit( 1);
      }
      memset(buff, 0x00, BUFF_SIZE);
      sprintf(buff, "%s\n", "GET //index.html");
      write(client_socket, buff, strlen(buff));
      read(client_socket, buff, BUFF_SIZE);
      printf("data is %s \n", buff);
                                       
     return 0;
}
