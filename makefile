# makefile for pudge

TARGETS = server client master test_hdbapi test_protocol test_networkserver test_networkclient

objects1 = dbserver.o hdbapi.o network.o protocol.o hash.o
objects2 = dbclient.o network.o protocol.o
objects3 = master.o network.o protocol.o linklist.o

all: client server master

server: $(objects1)
	gcc -o server $(objects1) -ltokyocabinet -levent

client: $(objects2)
	gcc -o client $(objects2) -levent

master: $(objects3)
	gcc -o master $(objects3) -levent

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