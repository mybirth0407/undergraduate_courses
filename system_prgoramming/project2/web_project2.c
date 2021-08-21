#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <signal.h>
#include <fcntl.h>

#define BYTES 1024
#define BUF_SIZE    9999

char *ROOT;
int listenfd, clientfd;
void error(char *);
void startServer(char *);
void respond();
void urldecode(char *, char *, char *);
inline int ishex(int);
int decode(const char *, char *);

char *html_1 = "HTTP/1.0 200 OK\n\n";
char *html_2 = "<html><head><meta http-equiv=\"Content-Type\"content=\"text/html; charset=utf-8\"></head><body><pre>";
char *html_3 = "</pre></body></html>";

char *index_1 = "<html><head><title>Form</title><meta http-equiv=\"Content-Type\" content=\"text/html;charset=EUC-KR\"></head>";
char *index_2 = "<body><form action=server_cmd method=GET >cmd : <input type=text id=i001 name=cmd /><br/>";
char *index_3 = "<input type=submit value=Send /></form></body></html>";

unsigned char index_check = 0;


int main(int argc, char* argv[])
{
    struct sockaddr_in clientaddr;
    socklen_t addrlen;
    char c;    
    
    char PORT[6];
    ROOT = getenv("PWD");
    strcpy(PORT,"10000");

    int slot=0;

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

    while (1)
    {
        addrlen = sizeof(clientfd);
        clientfd = accept (listenfd, (struct sockaddr *) &clientaddr, &addrlen);

        if (clientfd<0)
            error ("accept() error");
        else
        {
            respond();
        }

    }

    return 0;
}

//start server
void startServer(char *port)
{
    struct addrinfo hints, *res, *p;

    // getaddrinfo for host
    memset (&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    if (getaddrinfo( NULL, port, &hints, &res) != 0)
    {
        perror ("getaddrinfo() error");
        exit(1);
    }
    // socket and bind
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

    // listen for incoming connections
    if ( listen (listenfd, 1000000) != 0 )
    {
        perror("listen() error");
        exit(1);
    }
}

//client connection
void respond()
{
    char mesg[99999], *reqline[3], data_to_send[BYTES], path[99999],html[99999],*param[5], *target,*result, html2[99999];
    int rcvd, fd, bytes_read;
    char *ptr;
	char read_data[BUF_SIZE];
	char ch;
	char txt[99999];
	int i=0;

	char *html_s = "<html><head><title>Title</title></head><body>";
	char *html_e = "</body></html>";
	
	struct stat buf;

    memset( (void*)mesg, (int)'\0', 99999 );
    memset(path, 0x00, strlen(path));

    rcvd=recv(clientfd, mesg, 99999, 0);

    if (rcvd<0)    // receive error
        fprintf(stderr,("recv() error\n"));
    else if (rcvd==0)    // receive socket closed
        fprintf(stderr,"Client disconnected upexpectedly.\n");
    else    // message received
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
    		    if(index_check == 0)
        		{
	        		write(clientfd, html_1, strlen(html_1));
	        		write(clientfd, index_1, strlen(index_1));
		        	write(clientfd, index_2, strlen(index_2));
	        		write(clientfd, index_3, strlen(index_3));
	        		index_check = 1;
	        		printf("ROOT : %s \n", ROOT);
	        	} else {
	                    printf("ROOT : %s \n", ROOT);
               	        param[0] = strdup(reqline[1]); // path copy to param[0]
			//			chdir(ROOT);

            	        if (strncmp(param[0], "/server_cmd", 4) == 0) { 


							if(lstat(param[2], &buf) < 0)
								perror("lstat error");

							if(S_ISREG(buf.st_mode))
								ptr="regular";

							else if(S_ISDIR(buf.st_mode))
								ptr="dir";

							else if(S_ISCHR(buf.st_mode))
								ptr="char sp";

							else if(S_ISBLK(buf.st_mode))
								ptr="blo sp";

							else if(S_ISFIFO(buf.st_mode))
								ptr="fifo";

							else if(S_ISLNK(buf.st_mode))
								ptr="sym link";
							
							else if(S_ISSOCK(buf.st_mode))
								ptr="socket";
							
							else
								ptr="** unkown mode **";

							//sprintf(html,"%s %s %s %s ", html_s, "file type is", ptr, html_e);
							
							
							write(clientfd, "HTTP/1.0 200 OK\n\n", 17);
							write(clientfd, "file type is ", strlen("file type is "));
							write(clientfd, ptr, strlen(ptr));
							write(clientfd, "\n\n", strlen("\n\n"));

							if(ptr=="regular"){

								fd = open(param[2], O_RDONLY);
								
								while(read(fd, &ch, 1))
										write(clientfd, &ch, 1);

								close(fd);
							}


                	    	target = (char *)malloc(strlen(param[0]));
                        	result = (char *)malloc(strlen(param[0]));
                        	decode(param[0],result);
                        	printf("result %s \n",result);
                            param[1] = strtok(result, "=");
                            printf("%s \n", param[1]);
                            param[2] = strtok(NULL, "?");
                            printf("%s \n", param[2]);

							sprintf(html,"%s ", "hi");
                        }
                        else
                        {
		        	        printf("Not found\n");
        	        		write(clientfd, "HTTP/1.0 404 Not Found\n", 23);
                        }
                }
            }
        }
  }
  shutdown (clientfd, SHUT_RDWR);
  close(clientfd);
  clientfd = -1;
}






void urldecode(char *src, char *last, char *dest) {
 for(;src<=last;src++,dest++){
  if(*src=='%'){
   int code;
   if(sscanf(src+1, "%2x", &code)!=1) code='?';
   *dest = code;
   src+=2;
  }else if(*src=='+') {
   *dest = ' ';
  }else {
   *dest = *src;
  }
 }
 *(++dest) = '\0';
}


inline int ishex(int x)
{
	return	(x >= '0' && x <= '9')	||
		(x >= 'a' && x <= 'f')	||
		(x >= 'A' && x <= 'F');
}
 
int decode(const char *s, char *dec)
{
	char *o;
	const char *end = s + strlen(s);
	int c;
 
	for (o = dec; s <= end; o++) {
		c = *s++;
		if (c == '+') c = ' ';
		else if (c == '%' && (	!ishex(*s++)	||
					!ishex(*s++)	||
					!sscanf(s - 2, "%2x", &c)))
			return -1;
 
		if (dec) *o = c;
	}
 
	return o - dec;
}
 



