#include "minet_socket.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

#define BUFSIZE 1024

int main(int argc, char * argv[])
{
    char * server_name = NULL;
    int server_port    = -1;
    char * server_path = NULL;
    char * req         = NULL;
    bool ok            = false;
    
    struct hostent *hp;
    fd_set read_fd;
    struct timeval timeout;
    
    /*parse args */
    if (argc != 5)
    {
	fprintf(stderr, "usage: http_client k|u server port path\n");
	exit(-1);
    }

    server_name = argv[2];
    server_port = atoi(argv[3]);
    server_path = argv[4];

    req = (char *)malloc(strlen("GET  HTTP/1.0\r\n\r\n") 
			 + strlen(server_path) + 1);  

    /* initialize */
    if (toupper(*(argv[1])) == 'K')
    {
	minet_init(MINET_KERNEL);
    } else if (toupper(*(argv[1])) == 'U')
    { 
	minet_init(MINET_USER);
    } else
    {
	fprintf(stderr, "First argument must be k or u\n");
	exit(-1);
    }
  
    /* make socket */
    int sd=socket(AF_INET,SOCK_STREAM,0);
  
    /* get host IP address  */
    if ((hp = gethostbyname(server_name)) == NULL) 
    {
	fprintf(stderr, "Could not find host %s\n", server_name);
	exit(-1);
    }
  
    /* set address */
    struct sockaddr_in sa;
    memset(&sa, 0 ,sizeof(sa));
    sa.sin_port = htons(server_port); //1500
    memcpy(&sa.sin_addr.s_addr, hp->h_addr, hp->h_length);
    sa.sin_family = AF_INET;
  
    /* connect to the server socket */
    if (connect(sd, (struct sockaddr *)&sa, sizeof(sa))<0)
    {
	fprintf(stderr, "Failed connect\n");
	exit(-1);
    }
  
    /* send request message */
    sprintf(req, "GET %c%s HTTP/1.0\r\n\r\n", server_path[0] == '/' ? '' : '/', server_path);
    send(sd, req, strlen(req), 0);
  
    /* set up timeout */
    timeout.tv_sec = 10;
    timeout.tv_usec = 500000;
  
    /* wait till socket can be read. */
    FD_ZERO(&read_fd);
    FD_SET(sd, &read_fd);
    if(select(sd+1, &read_fd, NULL, NULL, &timeout) == -1)
    {
	fprintf(stderr, "Failed connect\n"); 
	exit(-1);
    }
  
    /* check response header code */
    char header[12];
    if(read(sd, header, 12) <= 0)
    {
    	fprintf(stderr, "Failed to read header\n");
	exit(-1);
    }
    int responseCode = atoi(header+9);
    
    /* read socket */
    int res;
    char c[1], block[4];
    if(responseCode == 200) //OK
    {
    	/* read header*/
    	do
    	{
    		res = read(sd, c, 1);
    		
    		/* check for end of header \r\n\r\n */ 
    		if(*c == '\r')
    		{
    			if(read(sd, block, 3) < 0)
    			{
    				fprintf(stderr, "Could not read block in header\n");
				exit(-1);
    			}
    			
    			block[3] = '\0';
    				
    			if(strcmp(block, "\n\r\n") == 0)
    			{
    				break;
    			}
    		}
    	} while(res > 0);
    	
    	/* print out the rest of the response: real web content */
    	do
    	{
    		res = read(sd, c, 1);
    		if(res > 0)
    			printf("%c", c[0]);
    	} while(res > 0);
    }
    else
    {
    	/* print header and response data */
    	printf("%s", header);
    	do
    	{
    		res = read(sd, c, 1);
    		if(res > 0)
    			printf("%c", c[0]);
    	} while(res > 0);
    }
    printf("\nFinished content\n");

    /*close socket and deinitialize */
    shutdown(sd, 0);
    printf("Shutdown\n");
    free(req);
    printf("Free'd req\n");
  
    fflush(stdout);
    if (ok) {
	return 0;
    } else {
	return -1;
    }
}
