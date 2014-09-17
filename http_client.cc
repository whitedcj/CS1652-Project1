/* UNCOMMENT FOR MINET 
 * #include "minet_socket.h"
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

#define BUFSIZE 1024

int main(int argc, char * argv[]) {
    char * server_name = NULL;
    int server_port    = -1;
    char * server_path = NULL;
    char * req         = NULL;
    bool ok            = false;
    
    struct hostent *hp;
    fd_set read_fd;
    struct timeval timeout;
    
    /*parse args */
    if (argc != 5) {
	fprintf(stderr, "usage: http_client k|u server port path\n");
	exit(-1);
    }

    server_name = argv[2];
    server_port = atoi(argv[3]);
    server_path = argv[4];

    req = (char *)malloc(strlen("GET  HTTP/1.0\r\n\r\n") 
			 + strlen(server_path) + 1);  

    /* initialize */
    if (toupper(*(argv[1])) == 'K') { 
	/* UNCOMMENT FOR MINET 
	 * minet_init(MINET_KERNEL);
         */
    } else if (toupper(*(argv[1])) == 'U') { 
	/* UNCOMMENT FOR MINET 
	 * minet_init(MINET_USER);
	 */
    } else {
	fprintf(stderr, "First argument must be k or u\n");
	exit(-1);
    }
  
    /* make socket */
    int sd=socket(AF_INET,SOCK_STREAM,0);
  
    /* get host IP address  */
    if ((hp = gethostbyname(server_name)) == NULL) {
      printf("Could not find host %s\n", server_name);
    }
  
    /* set address */
    struct sockaddr_in sa;
    memset(&sa, 0 ,sizeof(sa));
    sa.sin_port = htons(server_port); //1500
    memcpy(&sa.sin_addr.s_addr, hp->h_addr, hp->h_length);
    sa.sin_family = AF_INET;
  
    /* connect to the server socket */
    if (connect(sd, (struct sockaddr *)&sa, sizeof(sa))<0) {
      printf("Failed connect\n"); 
    }
  
    printf("Connected\n");
    fflush(stdout);
  
    /* send request message */
    sprintf(req, "GET /%s HTTP/1.0\r\n\r\n", server_path);
    send(sd, req, strlen(req), 0);
  
    /* set up timeout */
    timeout.tv_sec = 10;
    timeout.tv_usec = 500000;
  
    /* wait till socket can be read. */
    FD_ZERO(&read_fd);
    FD_SET(sd, &read_fd);
    int rc = select(sd+1, &read_fd, NULL, NULL, &timeout);
  
    /* check response header code */
    char header[12];
    int n = read(sd, header, 12);
    char * code = new char[3];
    memcpy(code, header[10], 3);
    
    int responseCode = atoi(code);
    if(responseCode == 200) //OK
    {
    	printf("200 OK\n");
    }
    else
    {
    	print("%i\n", responseCode);
    }

    /* print first part of response: header, error code, etc. */

    /* second read loop -- print out the rest of the response: real web content */

    /*close socket and deinitialize */
    shutdown(sd, 0);
    free(req);
  
    printf("Closed socket\n");
    fflush(stdout);
    if (ok) {
	return 0;
    } else {
	return -1;
    }
}
