ALL: client server1 server2 server3

client: http_client.cc
	c++ -o http_client http_client.cc -lnsl

server1: http_server1.cc
	c++ -o http_server1 http_server1.cc -lnsl

server2: http_server2.cc
	c++ -o http_server2 http_server2.cc -lnsl

server3: http_server3.cc
	c++ -o http_server3 http_server3.cc -lnsl

clean: 
	rm -f *.o http_client http_server1 http_server2 http_server3
