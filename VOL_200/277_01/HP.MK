# hp

CFLAGS= -O
 
hp:	hp.o tekdecode.o
	cc -s hp.o tekdecode.o -o hp

tekdecode.o:	tek4.h

clean:
	rm -f *.o
