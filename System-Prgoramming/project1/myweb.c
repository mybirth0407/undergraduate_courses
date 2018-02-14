#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<netdb.h>
#include<signal.h>
#include<fcntl.h>

#define CONNMAX 1000
#define BYTES 1024

extern char **environ;
char *ROOT; 
int listenfd, clientfd;
void error(char *);
void startServer(char *);
void respond();

int main(int argc, char* argv[])
{
        struct sockaddr_in clientaddr;
        socklen_t addrlen; 
        char c;
                    
        char PORT[6];
        ROOT = getenv("PWD"); 
        strcpy(PORT,"10000");

        while ((c = getopt (argc, argv, "p:r:")) != -1) 
         switch (c)
         {
             case 'r':
                ROOT = malloc(strlen(optarg)); 
                strcpy(ROOT,optarg);
                break;
             case 'p':
                strcpy(PORT,optarg);
                break;
             case '?':
                fprintf(stderr,"Wrong arguments given!!!\n");
                exit(1);
             default:
                exit(1);
         }
        
        printf("Server started at port no. %s%s%s with root directory as %s%s%s\n","\033[92m",PORT,"\033[0m","\033[92m",ROOT,"\033[0m");
		
        clientfd=-1;

        startServer(PORT); 

        addrlen = sizeof(clientaddr);

        clientfd = accept (listenfd, (struct sockaddr *) &clientaddr, &addrlen); 

        if (clientfd<0)
             error ("accept() error");
        else
        		respond();

        return 0;
}


void startServer(char *port)
{
        struct addrinfo hints, *res, *p; 

        memset (&hints, 0, sizeof(hints)); 
        hints.ai_family = AF_INET; 
        hints.ai_socktype = SOCK_STREAM; 
        hints.ai_flags = AI_PASSIVE;

        if (getaddrinfo( NULL, port, &hints, &res) != 0)
        {
           perror ("getaddrinfo() error");
           exit(1);
        }

        for (p = res; p!=NULL; p=p->ai_next)
        {
           listenfd = socket (p->ai_family, p->ai_socktype, 0); 
           if (listenfd == -1) continue;
           if (bind(listenfd, p->ai_addr, p->ai_addrlen) == 0) break;
        }

        if (p==NULL)
        {
           perror ("socket() or bind()");
           exit(1);
        }

        freeaddrinfo(res);

        if ( listen (listenfd, 1000000) != 0 )
        {
           perror("listen() error");
           exit(1);
        }
}

void respond()
{
        char mesg[99999], *reqline[3], data_to_send[BYTES], path[99999];
		//char html[99999];
        int rcvd, fd, bytes_read;
		int i=0;
		
        memset( (void*)mesg, (int)'\0', 99999 );
        rcvd=read(clientfd, mesg, 99999);

        if (rcvd<0)    
           fprintf(stderr,("recv() error\n"));
        else if (rcvd==0)   
           fprintf(stderr,"Client disconnected upexpectedly.\n");
        else    
        {
           printf("%s", mesg);
           reqline[0] = strtok (mesg, " \t\n"); 
           if ( strncmp(reqline[0], "GET\0", 4)==0 )
           {
                reqline[1] = strtok (NULL, " \t"); 
                reqline[2] = strtok (NULL, " \t\n");
                if ( strncmp( reqline[2], "HTTP/1.0", 8)!=0 && strncmp( reqline[2], "HTTP/1.1", 8)!=0 )
                {
                        write(clientfd, "HTTP/1.0 400 Bad Request\n", 25);
                }
                else
                {
                        char *html_s = "<html><head><title>Title</title></head><body>";
                        char *html_e = "</body></html>";
                        char *html_main = "Environment variables print !!!";

						//sprintf(html,"%s %s %s\n", html_s,html_main,html_e);

						write(clientfd, "HTTP/1.0 200 OK\n\n", 17);
						write(clientfd, html_main, strlen(html_main));
						write(clientfd, "\n\n", strlen("\n\n"));

						for(i=0; environ[i]!=NULL; i++){

							write(clientfd, environ[i], strlen(environ[i]));
							write(clientfd, "\n", strlen("\n"));
						}
				}
			}
         }

         shutdown (clientfd, SHUT_RDWR);      
         close(clientfd);
         clientfd=-1;
}
