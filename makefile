# makefile for pudge

objects = main.o hdbapi.o

all: pudge

pudge: $(objects)
	gcc -o pudge $(objects) -ltokyocabinet

.c.o:
	gcc -c $<

clean:
	rm pudge *.o
