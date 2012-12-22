# makefile for pudge

TARGETS = server1 server2 client test_hdbapi test_protocol test_networkserver test_networkclient

objects1 = dbserver_pthread.o hdbapi.o network.o protocol.o hash.o
objects2 = dbserver_advance.o hdbapi.o network.o protocol.o hash.o
objects3 = dbclient.o hdbapi.o network.o protocol.o

all: server1 server2 client

server1: $(objects1)
	gcc -o server1 $(objects1) -ltokyocabinet

server2: $(objects2)
	gcc -o server2 $(objects2) -ltokyocabinet

client: $(objects3)
	gcc -o client $(objects3) -ltokyocabinet
#test: test_hdbapi.o test_protocol.o test_networkserver.o test_networkclient.o hdbapi.o #protocol.o network.o
#	gcc -o test_hdbapi test_hdbapi.o hdbapi.o -ltokyocabinet
#	./test_hdbapi
#	gcc -o test_protocol test_protocol.o protocol.o
#	./test_protocol
#	gcc -o test_networkserver test_networkserver.o network.o
#	gcc -o test_networkclient test_networkclient.o network.o
#	./test_networkserver &
#	sleep 1
#	./test_networkclient
#	@printf '*****Test done.*****\n'

.c.o:
	gcc -c $<

clean:
	rm $(TARGETS) *.o
