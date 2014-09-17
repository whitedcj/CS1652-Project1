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


#define BUFSIZE 1024

int main(int argc, char * argv[]) {

    char * server_name = NULL;
    int server_port    = -1;
    char * server_path = NULL;
    char * req         = NULL;
    bool ok            = false;
    struct hostent *hp;

    /*parse args */
    if (argc != 5) {
	fprintf(stderr, "usage: http_client k|u server port path\n");
	exit(-1);
    }
    
    printf("Parsing args");

    server_name = argv[2];
    server_port = atoi(argv[3]);
    server_path = argv[4];
    
    printf("Parsed args");

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

   //Parse args
  //char* server_name = argv[2];
  //printf("Server: %s\n", server_name);
  //int server_port = atoi(argv[3]);
  //printf("Port: %i\n", server_port);
  //char* server_path = argv[4];
  //printf("Path: %s\n", server_path);
  
  /* make socket */
  int sock=socket(AF_INET,SOCK_STREAM,0);
  
  printf("Socket: %i", sock);
  fflush(stdout);
  
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
  if (connect(sock, (struct sockaddr *)&sa, sizeof(sa))<0) {
    printf("Failed connect\n"); 
  }
  
  printf("Connected\n");
  fflush(stdout);
  
  /* send request message */
  sprintf(req, "GET %s HTTP/1.0\r\n\r\n", server_path);
  
  /* wait till socket can be read. */
  /* Hint: use select(), and ignore timeout for now. */
  
  //Read from server
  //char * bufout = ”Hello”;
  //char bufin[80];
  //write(sock, bufout, strlen(bufout) + 1);
  //int n = read(sock, bufin, 80);
  
  //Close socket
  
  
    

    /* first read loop -- read headers */

    /* examine return code */   

    //Skip "HTTP/1.0"
    //remove the '\0'

    // Normal reply has return code 200

    /* print first part of response: header, error code, etc. */

    /* second read loop -- print out the rest of the response: real web content */

    /*close socket and deinitialize */
    shutdown(sock, 0);
  
    printf("Closed socket\n");
    fflush(stdout);
    if (ok) {
	return 0;
    } else {
	return -1;
    }
}
