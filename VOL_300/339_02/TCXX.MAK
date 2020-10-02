# TCXX.MAK for The CTRL+C LIBRARY - MAKEFILE
# Borland Turbo-C Ver. 2.0 

DRIVE   = c:

MODEL   = s         # <-----change this for different memory model

CLIB    = $(DRIVE)\$(MODEL)tcxxcl.lib  graphics.lib
INCLUDE = $(DRIVE)\tcxx\include

target:  cldemo.exe bootdemo.exe

cldemo.exe:  
   tcc -m$(MODEL) -DTCXX -I$(INCLUDE) -F$(CLIB) cldemo.c

bootdemo.exe:
   tcc -m$(MODEL) -DTCXX -I$(INCLUDE) -F$(CLIB) bootdemo.c

