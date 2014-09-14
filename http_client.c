#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
//#include <netdb.h>

int main (int argc, const char* argv[])
{
  //Parse args
  char* server_name = argv[2];
  printf("Server: %s\n", server_name);
  int server_port = atoi(argv[3]);
  printf("Port: %i\n", server_port);
  char* server_path = argv[4];
  printf("Path: %s\n", server_path);
  
  //Create and connect to socket
  int sock=socket(AF_INET,SOCK_STREAM,0);
  
  if(sock<0)
    perror("Socket failed");
  
  printf("Socket: %i", sock);
  
  struct sockaddr_in sa;
  memset(&sa, 0 ,sizeof(sa));
  sa.sin_port = htons(server_port); //1500
  sa.sin_addr.s_addr = htonl(gethostbyname(server_name));
  sa.sin_family = AF_INET;
  
  connect(sock, (struct sockaddr *)&sa, sizeof(sa));
  
  printf("Connected\n");
  
  //Read from server
  //char * bufout = ”Hello”;
  //char bufin[80];
  //write(sock, bufout, strlen(bufout) + 1);
  //int n = read(sock, bufin, 80);
  
  //Close socket
  close(sock);
  
  printf("Closed socket\n");
  
  
  return 0;
}
