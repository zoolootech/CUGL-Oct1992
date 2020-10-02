ECHO OFF
CLS
REM This .BAT file compiles the windows routines and places them in
REM a library. You can specify model, and text-only vs text/graphics
REM
REM default is all files matching w*,
REM but parameter %3-9 may name files

   set wn=w*.c
   if NOT (%3) == () set wn=%3 %4 %5 %6 %7 %8 %9 

REM see if we're building TEXT or graphics libraries
    set d = error
    if (%1)  == (t) set d=-DTEXTONLY
    if (%1)  == (g) set d=
    if (%d%) == (error) goto :HELPEM

REM validate choice of model
	for %%a in ( t s m c l h ) do goto :OK

:HELPEM
ECHO This batch file 'makes' the windows libraries.
ECHO the first parameter should be
ECHO            T= TEXTONLY  --or-- G= graphics
ECHO the second parameter should be the memory model
ECHO            T, S, M, C, L, H
ECHO ...
ECHO the third thru ninth parameter(s) is the name(s) of the file to make.
ECHO ...
ECHO May use wildcards.
ECHO ...
ECHO library not made.
goto :QUIT


:OK
ECHO making windows libraries.
ECHO on
  	if exist w*.obj erase w*.obj
   	echo COMPILING window routines      >>errors.lst
	tcc -m%2 -c  %d% %wn% 				>>errors.lst
   	dir w*.obj | dir2bat -+@ &          > objlst.lst 
	echo , lib							>>objlst.lst
	echo PLACING MODULES IN LIBRARY     >>errors.lst
	tlib  w%1%2.lib @objlst.lst			>>errors.lst	
   	echo off
   
   	if exist w*.obj erase w*.obj
	
ECHO library updated. compiler messages in errors.lst

:QUIT

set p=
set wn=
set mdl=

