#
# Makefile for mkptypes. Edit the lines below to suit your tastes; the default
# is for my computer (Atari ST running the gcc 1.37); a Unix configuration is
# also provided.

#CC = cc
#PROG = mkptypes
#CFLAGS = -O

CC = gcc
PROG = mkptypes.ttp
CFLAGS = -mshort -O

$(PROG) : mkptypes.c mkptypes.h
	$(CC) $(CFLAGS) -o $(PROG) mkptypes.c

clean:
	rm -f mkptypes.o

realclean: clean
	rm -f $(PROG) report core
