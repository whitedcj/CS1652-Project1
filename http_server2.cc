#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/select.h>
#include <unistd.h>
#include "minet_socket.h"

#define BUFSIZE 1024
#define BACKLOG 15
int handle_connection(int sock) {

    int ok = 0;
    int start_fname = 0;
    int i, n, j, amount = 0;
    int size_of_file = 0;
    char read_buf[BUFSIZE];
    char write_buf[BUFSIZE];
    char *fname;
    char ch;
    FILE *fp;
    
    const char * ok_response_f = "HTTP/1.0 200 OK\r\n"	\
	"Content-type: text/plain\r\n"			\
	"Content-length: %d \r\n\r\n";
 
    const char * notok_response = "HTTP/1.0 404 FILE NOT FOUND\r\n"	\
	"Content-type: text/html\r\n\r\n"			\
	"<html><body bgColor=black text=white>\n"		\
	"<h2>404 FILE NOT FOUND</h2>\n"
	"</body></html>\n";
    
    /* first read loop -- get request and headers*/
    n = minet_read(sock, read_buf, BUFSIZE);
    /* parse request to get file name */
    printf("n is %d\n", n);
    j = 0;
    for(i = 0; i < n; i++) {
    	printf("i = %d\t", i);
    	printf("buf i is %c and start_fname is %d\n", read_buf[i], start_fname);
        if (read_buf[i] != ' ' && start_fname != 0) {
            j++;
        } else if (read_buf[i] == ' ' && start_fname == 0) {
       		if (read_buf[i+1] == 47) {
                start_fname = i+2;
                i++;
                printf("i is %d, buf i is %c and start_fname is %d and j is %d\n", i, read_buf[i], start_fname, j);
            } else {
                start_fname = i+1;    
            }
        } else if (read_buf[i] == ' ' && start_fname != 0) {
            break;
        } else {
        	continue;
        }
    }
    printf("j is %d\n", j);
    /* Assumption: this is a GET request and filename contains no spaces*/
    printf("read request");
    /* try opening the file */
    fname = (char *)malloc(sizeof(char) * j);
    strncpy(fname, read_buf+start_fname, j);
    printf("file name is %s\n", fname);
    fp = fopen(fname, "r");



    if (!fp) {
        printf("couldn't open file. it doesn't exist");
        ok = 0;

    } else {
        printf("could open the file, so send the information");
        ok = 1;
    }
    /* send response */
    if (ok) {
	/* send headers */
		fseek(fp, 0, SEEK_END);
    	size_of_file = ftell(fp);
    	fseek(fp, 0, SEEK_SET);
    	sprintf(write_buf, ok_response_f, size_of_file);
    	
        amount = 0;
        n = 0;
        while (amount < strlen(write_buf)) {
            n = minet_write(sock, write_buf + amount, strlen(write_buf) - amount);
            if(n < 0) {
                perror("Send failed");
                exit ( EXIT_FAILURE );
            }        
            amount += n;
        }
	/* send file */
        printf("size of readbuf is %d\n", sizeof(read_buf));
		
        n = 0;
		while(ch = fgetc(fp)){
			write_buf[n++] = ch;
			if (n>=1023 || feof(fp)) {
				amount = 0;
		        n = 0;
		        while (amount < strlen(write_buf)) {
		            n = send(sock, write_buf + amount, strlen(write_buf) - amount, 0);
		            if(n < 0) {
		                perror("Send failed");
		                exit ( EXIT_FAILURE );
		            }        
		            amount += n;
		        }				
		        n = 0;
		        memset(write_buf, 0, sizeof(write_buf));
		        if (feof(fp)) break;
			}
		}

    } else {
	// send error response
        strcpy(write_buf, notok_response);
        amount = 0;
        n = 0;
        while (amount < strlen(write_buf)) {
            n = minet_write(sock, write_buf + amount, strlen(write_buf) - amount);
            if(n < 0) {
                perror("Send failed");
                exit ( EXIT_FAILURE );
            }        
            amount += n;
        }
    }
    
    if (fp) fclose(fp);
    close(sock);
    free(fname);
    /* close socket and free space */
  
    if (ok) {
	   return 0;
    } else {
	   return -1;
    }


}

int main (int argc, char** argv) {

	int init_sock, conn_sock, amt = 0;
	int read_list[BACKLOG];
	int size_of_list = 0;
	int port, stack_type;
	int n, i;
	int con_size = sizeof(struct sockaddr_in);
	struct sockaddr_in addr;
	struct sockaddr_in client_addr;
	char buf[1024];	//to be deleted
	char read_buf[1024];
	char write_buf[1024];
	char *fname;
	FILE *fp;
	fd_set rfds;
    struct timeval tv;
    int ret;
    int max = -1;

	if (argc < 3) {
		//replace with better error print. like print to stderr
		printf("Usage: http_server1 k|u port\n");
		exit(0);
	}
	minet_init(MINET_KERNEL);
	stack_type = atoi(argv[1]);
	port = atoi(argv[2]);
	
	if ((init_sock = minet_socket(SOCK_STREAM)) < 0) {
		perror("Socket failed");
		//find out what this means below
		exit ( EXIT_FAILURE );
	}
	
	memset(&addr , 0, sizeof(addr));
	addr.sin_family = AF_INET ;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = INADDR_ANY ;
	
	if(minet_bind(init_sock, (struct sockaddr_in *)&addr) < 0) {
		perror("Bind failed");
		exit( EXIT_FAILURE );
	}
	
	if(minet_listen(init_sock , 10) < 0) {
		perror("Listen failed");
		exit( EXIT_FAILURE );
	}
	read_list[size_of_list++] = init_sock;
	//socket, address of client, size of address
	while(1 == 1) {
  		
		FD_ZERO(&rfds);
		printf("size of list is %d\n", size_of_list);
		max = -1;
		for (i = 0; i < size_of_list; i++) {
			printf("i is %d\tread list is %d\n", i, read_list[i]);
			if (max < read_list[i]) {
				max = read_list[i];
			}
			FD_SET(read_list[i], &rfds);
		}

		tv.tv_sec = 5;
		tv.tv_usec = 0;

		ret = minet_select(max + 1, &rfds, NULL, NULL, &tv);
		if (ret == -1) {
			printf("select returned -1");
		} else if (ret == 0) {
			printf("timeout");
		} else {
			printf("select returned\n");
			for (i = 0; i < size_of_list; i++) {
				if (FD_ISSET(read_list[i], &rfds) != 0) {
					printf("selected one is %d\n", read_list[i]);
					if (read_list[i] == init_sock) {
						if((conn_sock = minet_accept(init_sock, (struct sockaddr_in *)&client_addr)) < 0) {
							perror("Accept failed");
							exit( EXIT_FAILURE );
						} else {
							read_list[size_of_list++] = conn_sock;
						}
					} else {
						n = handle_connection(read_list[i]);				
						for (n = i; n < size_of_list - 1; n++) {
							read_list[n] = read_list[n+1];
						}
						size_of_list--;
					}
				}
			}
		}
	}

	minet_close(init_sock);
	
	return 0;
}
