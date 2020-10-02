# QC25.MAK for The CTRL+C LIBRARY - MAKEFILE
# Microsoft Quick-C Ver. 2.5

DRIVE   = c:

MODEL   = S         # <-----change this for different memory model

CLIB    = $(DRIVE)\$(MODEL)qc25cl.lib  graphics.lib
INCLUDE = $(DRIVE)\qc25\include

target:  cldemo.exe bootdemo.exe

cldemo.exe:  
   qcl -A$(MODEL) -DQC25 -I$(INCLUDE) $(CLIB) cldemo.c

bootdemo.exe:
   qcl -A$(MODEL) -DQC25 -I$(INCLUDE) $(CLIB) bootdemo.c


