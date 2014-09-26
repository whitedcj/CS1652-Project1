 #include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include "minet_socket.h"

#define BUFSIZE 1024

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
    
    const char * ok_response_f = "HTTP/1.0 200 OK\r\n"  \
    "Content-type: text/plain\r\n"          \
    "Content-length: %d \r\n\r\n";
 
    const char * notok_response = "HTTP/1.0 404 FILE NOT FOUND\r\n" \
    "Content-type: text/html\r\n\r\n"           \
    "<html><body bgColor=black text=white>\n"       \
    "<h2>404 FILE NOT FOUND</h2>\n"
    "</body></html>\n";
    
    /* first read loop -- get request and headers*/
    n = minet_read(sock, read_buf, BUFSIZE);
    /* parse request to get file name */
    //this for loop calculates the size of the filename (j) and the starting index (start_fname)
    j = 0;
    for(i = 0; i < n; i++) {
        if (read_buf[i] != ' ' && start_fname != 0) {
            j++;
        } else if (read_buf[i] == ' ' && start_fname == 0) {
            if (read_buf[i+1] == 47) {
                start_fname = i+2;
                i++;
            } else {
                start_fname = i+1;    
            }
        } else if (read_buf[i] == ' ' && start_fname != 0) {
            break;
        } else {
            continue;
        }
    }
    
    /* Assumption: this is a GET request and filename contains no spaces*/
    /* try opening the file */
    fname = (char *)malloc(sizeof(char) * j);
    strncpy(fname, read_buf+start_fname, j);
    fp = fopen(fname, "r");

    if (!fp) {
        ok = 0;
    } else {
        ok = 1;
    }
    /* send response */
    if (ok) {
    /* send headers */
        //Calculate content length
        fseek(fp, 0, SEEK_END);
        size_of_file = ftell(fp);
        fseek(fp, 0, SEEK_SET);
        sprintf(write_buf, ok_response_f, size_of_file);

        amount = 0; //amount that has been written overall
        n = 0;      //amount that was just written
        //write headers to socket
        while (amount < strlen(write_buf)) {
            n = minet_write(sock, write_buf + amount, strlen(write_buf) - amount);
            if(n < 0) {
                perror("Send header failed");
                exit(-1);
            }        
            amount += n;
        }
        
        /* send file */        
        n = 0;
        //read from the file byte by byte
        //write to the socket when the buffer is full or the file is over
        while(ch = fgetc(fp)){
            write_buf[n++] = ch;
            if (n>=1023 || feof(fp)) {
                amount = 0;
                n = 0;
                while (amount < strlen(write_buf)) {
                    n = minet_write(sock, write_buf + amount, strlen(write_buf) - amount);
                    if(n < 0) {
                        perror("Send file failed");
                        exit(-1);
                    }        
                    amount += n;
                }               
                n = 0;
                //clear the write_buf
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
                exit(-1);
            }        
            amount += n;
        }
    }
    
    if (fp) fclose(fp); //close the file descriptor if it was open
    minet_close(sock);  //close the socket
    free(fname);        //free allocated space
  
    if (ok) {
       return 0;
    } else {
       return -1;
    }

}

int main (int argc, char** argv) {

    int init_sock , conn_sock , amt = 0;
    int port;
    int n, i;
    int con_size = (sizeof(struct sockaddr_in));
    struct sockaddr_in addr;
    struct sockaddr_in client_addr;
    char read_buf[1024];
    char write_buf[1024];
    char *fname;
    FILE *fp;

    if (argc < 3) {
        fprintf(stderr, "Usage: ./http_server1 k|u port_number");
        exit(-1);
    }

    /* initialize */
    if (*(argv[1]) == 'K' || *(argv[1]) == 'k') { 
        minet_init(MINET_KERNEL);
    } else if (*(argv[1]) == 'U' || *(argv[1]) == 'u') { 
        minet_init(MINET_USER);
    } else {
        fprintf(stderr, "First argument must be k or u\n");
        exit(-1);
    }

    port = atoi(argv[2]);
    
    //create initial socket
    if ((init_sock = minet_socket(SOCK_STREAM)) < 0) {
        perror("Socket creation failed");
        exit (-1);
    }
    
    //set address and port information
    memset(&addr , 0, sizeof(addr));
    addr.sin_family = AF_INET ;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY ;
    
    //bind the init_sock
    if(minet_bind(init_sock, (struct sockaddr_in *)&addr) < 0) {
        perror("Bind failed");
        exit( EXIT_FAILURE );
    }
    
    //listen for incoming connections with a backlog of 10
    if(minet_listen(init_sock , 10) < 0) {
        perror("Listen failed");
        exit( EXIT_FAILURE );
    }

    while(1 == 1) {
        //accept an incoming connection
        if (conn_sock = minet_accept(init_sock, (struct sockaddr_in *)&client_addr) < 0) {
            perror("Accept failed");
            exit(-1);
        }
        //handle the connection
        n = handle_connection(conn_sock);        
    }

    //close the socket
    minet_close(init_sock);
    
    return 0;
}
