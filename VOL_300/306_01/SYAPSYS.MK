# make file for Turbo C++ or Microsoft C

.c.obj :
#    cl -c $*.c
    tcc -c $*.c

all : timetest.exe parity.exe

timetest.exe : timetest.obj synapsys.obj
#    cl timetest.obj synapsys.obj
    tcc timetest.obj synapsys.obj

parity.exe : parity.obj synapsys.obj
#    cl parity.obj synapsys.obj
    tcc parity.obj synapsys.obj

timetest.obj : timetest.c synapsys.h

parity.obj : parity.c synapsys.h

synapsys.obj : synapsys.c synapsys.h

