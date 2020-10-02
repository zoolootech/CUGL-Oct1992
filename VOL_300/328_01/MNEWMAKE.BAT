ECHO OFF
CLS
REM MICROSOFT C version - for building windows library from scratch.
REM
REM This .BAT file compiles the windows routines and places them in
REM a library. You can specify model, and text-only vs text/graphics
REM
REM Note that this .bat file can't be used to update the library 
REM    because the microsoft librarian halts if you specify + on an old module
REM 

REM see if we're building TEXT or graphics libraries
    set d = error
    if (%1)  == (T) set d=/DTEXTONLY
    if (%1)  == (t) set d=/DTEXTONLY
    if (%1)  == (G) set d=
    if (%1)  == (g) set d=
    if (%d%) == (error) goto :HELPEM

REM validate choice of model, define symbols for compiler
    if NOT (%2) == (T) goto :TRYS
		set mdl=__TINY__
		goto :OK
:TRYS
    if NOT (%2) == (S) goto :TRYC
		set mdl=__SMALL__
		goto :OK
:TRYC
    if NOT (%2) == (C) goto :TRYM
		set mdl=__COMPACT__
		goto :OK
:TRYM
    if NOT (%2) == (M) goto :TRYL
		set mdl=__MEDIUM__
		goto :OK
:TRYL
    if NOT (%2) == (L) goto :HELPEM
		set mdl=__LARGE__
		goto :OK
:HELPEM
ECHO This batch file 'makes' the windows libraries.
ECHO the first parameter should be
ECHO            T= TEXTONLY  --or-- G= graphics
ECHO the second parameter should be the memory model
ECHO            T, S, M, C, L, H
ECHO ...
ECHO ...
ECHO library not made.
goto :QUIT


:OK
ECHO making windows libraries.
ECHO the following warning messages are acceptable:
ECHO      superfluous & on structure or array
ECHO      short/long mismatch... conversion supplied
ECHO ...
ECHO on
  	erase w*.obj
   	echo COMPILING window routines      > errors.lst
   	cl /A%2 /Os /c  %d% /D%mdl% w*.c 	>>errors.lst
   	dir %wn%.obj | dir2bat -+@ &        > objlst.lst 
	REM microsoft c librarian needs a few blank lines at end of file.
	type mblank.bat						>>objlst.lst
	type mblank.bat						>>objlst.lst
	type mblank.bat						>>objlst.lst
	echo PLACING MODULES IN LIBRARY     >>errors.lst
	lib  mw%1%2.lib @objlst.lst			>>errors.lst	
   	echo off
   
   
    erase objlst.lst
   	erase w*.obj

ECHO library updated. compiler messages in errors.lst

:QUIT

set p=
set wn=
set mdl=

