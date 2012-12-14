# makefile for pudge

TARGETS = server client

objects1 = dbserver.o hdbapi.o network.o protocol.o
objects2 = dbclient.o hdbapi.o network.o protocol.o

all: server client

server: $(objects1)
	gcc -o server $(objects1) -ltokyocabinet

client: $(objects2)
	gcc -o client $(objects2) -ltokyocabinet

.c.o:
	gcc -c $<

clean:
	rm server client *.o
