CS1652-Project1
===============

Cody Whited and Vikram Patwardhan
cjw51
vvp3

Included Files:
http_client.cc
http_server1.cc
http_server2.cc

Known Issues:
When http_server1.cc is first compiled and run, it does not respond to the first connection. Upon entering a carriage return to stdin on the server side, it will interpret that as the GET request and return 404 NOT FOUND to the server side itself. However after manually closing the client side connection, future connections to the server will function normally. 
