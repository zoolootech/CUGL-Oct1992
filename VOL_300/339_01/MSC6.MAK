# MSC6.MAK for The CTRL+C LIBRARY - MAKEFILE
# Microsoft Ver. 6.0

MODEL   = S     # <-----change this for different memory model

CLIB    = $(DRIVE)\$(MODEL)msc6cl.lib  graphics.lib
INCLUDE = $(DRIVE)\msc6\include

target:  cldemo.exe bootdemo.exe

cldemo.exe:  
   cl -A$(MODEL) -DMSC6 -I$(INCLUDE) $(CLIB) cldemo.c

bootdemo.exe:
   cl -A$(MODEL) -DMSC6 -I$(INCLUDE) $(CLIB) bootdemo.c
