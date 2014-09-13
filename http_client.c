#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>

int main (int argc, const char* argv[])
{
  //Parse args
  char * server_name = argv[2];
  int server_port = atoi(argv[3]);
  char * server_path = argv[4];
  
  //Create and connect to socket
  int sock=socket(AF_INET,SOCK_STREAM,0);
  
  struct sockaddr_in sa;
  memset(&sa, 0 ,sizeof(sa));
  sa.sin_port = htons(server_port); //1500
  sa.sin_addr.s_addr = htonl(gethostbyname(server_name));
  sa.sin_family = AF_INET;
  
  connect(sock, (struct sockaddr *)&sa, sizeof(sa));
  
  //Read from server
  //char * bufout = ”Hello”;
  //char bufin[80];
  //write(sock, bufout, strlen(bufout) + 1);
  //int n = read(sock, bufin, 80);
  
  //Close socket
  close(sock);
  
  return 0;
}
