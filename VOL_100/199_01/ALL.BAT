: all bat - 05/03/87
: tm and beep from norton utilities 3.0
: td - use ram disk, drive d: - see c88 compiler notes
: see elink for compiling with exec function
: looks like exec.o must be in same subdirectory as ged during linking
:
tm start /n
:
cc ged   -nDS  td
if errorlevel 1 goto stop
cc ged1  -nDS  td
if errorlevel 1 goto stop
cc ged2  -nDS  td
if errorlevel 1 goto stop
cc ged3  -nDS  td
if errorlevel 1 goto stop
cc ged4  -nDS  td
if errorlevel 1 goto stop
cc ged5  -nDS  td
if errorlevel 2 goto stop
cc ged6  -nDS  td
if errorlevel 1 goto stop
cc ged7  -nDS  td
if errorlevel 1 goto stop
cc ged8  -nDS  td
if errorlevel 1 goto stop
cc ged9  -nDS  td
if errorlevel 1 goto stop
cc ged10 -nDS  td
if errorlevel 1 goto stop
cc ged11 -nDS  td
if errorlevel 1 goto stop
asm88 gedio td
if errorlevel 1 goto stop
:
bind  -f./elnk.dat
:
tm stop
:
:stop
:
beep


