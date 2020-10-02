# MSC5.MAK for The CTRL+C LIBRARY - MAKEFILE
# Microsoft Ver. 5.1

DRIVE   = c:

MODEL   = S      # <-----change this for different memory model

CLIB    = $(DRIVE)\$(MODEL)msc5cl.lib  graphics.lib
INCLUDE = $(DRIVE)\tc20\include

target:  cldemo.exe bootdemo.exe

cldemo.exe:  
   cl -A$(MODEL) -DMSC5 -I$(INCLUDE) $(CLIB) cldemo.c

bootdemo.exe:
   cl -A$(MODEL) -DMSC5 -I$(INCLUDE) $(CLIB) bootdemo.c

