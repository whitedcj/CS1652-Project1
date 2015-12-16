CS1652-Project1
===============

Cody Whited and Vikram Patwardhan
cjw51
vvp3

Known Issues:
When http_server1.cc is first compiled and run, it does not respond to the first connection. Upon entering a carriage return to stdin on the server side, it will interpret that as the GET request and return 404 NOT FOUND to the server side itself. However after manually closing the client side connection, future connections to the server will function normally.   

Usage:
Designed for use with minet (http://pdinda.org/minet/), which allows server and client programs to use a user-implemented version of TCP.     
"./http_server# k|u port_number" runs different versions of the server, where    
'k' uses the kernel TCP implementation and 'u' forces the user implementation,    
'port_number' specifies a port on which to listen for requests    

"./http_client k|u server port path" gets and prints a file from a server, where      
'k' uses the kernel TCP implementation and 'u' forces the user implementation,    
'server' is the address of the server,    
'port' is the port number of the server,    
'path' is the path and filename for the file to be retrieved

