# TC20.MAK for The CTRL+C LIBRARY - MAKEFILE
# Borland Turbo-C Ver. 2.0 

DRIVE   = c:

MODEL   = s           # <-----change this for different memory model

CLIB    = $(DRIVE)\$(MODEL)tc20cl.lib  graphics.lib
INCLUDE = $(DRIVE)\tc20\include

target:  cldemo.exe bootdemo.exe

cldemo.exe:  
   tcc -m$(MODEL) -DTC20 -I$(INCLUDE) -L$(CLIB) cldemo.c

bootdemo.exe:
   tcc -m$(MODEL) -DTC20 -I$(INCLUDE) -L$(CLIB) bootdemo.c

