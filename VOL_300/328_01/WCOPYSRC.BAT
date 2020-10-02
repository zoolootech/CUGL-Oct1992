@echo off
ECHO this batch file copies SOURCE files for windows to A: B: or C:
ECHO parameter 1: drive, default=a: (must specify letter, colon)
ECHO parameter 2: switch for xcopy  (specify /d:mm-dd-yy)
set dr=A:
if NOT (%1) == () set dr=%1

if (%1) == (A:) goto :OK
if (%1) == (a:) goto :OK
if (%1) == (B:) goto :OK
if (%1) == (b:) goto :OK
if (%1) == (C:) goto :OK
if (%1) == (c:) goto :OK
ECHO ILLEGAL DRIVE SPECIFICATION ( a: b: or c: only )
goto :quit

:OK

echo PLACE SOURCE DISK IN DRIVE, PRESS ANY KEY
PAUSE

xcopy w*.c			%dr% %2
xcopy makehx.c 		%dr% %2
xcopy txt2mcr.c 	%dr% %2
xcopy dir2bat.c 	%dr% %2
xcopy *.h*	    	%dr% %2
xcopy header.src    %dr%readme %2
xcopy *.doc 		%dr% %2
xcopy demo*.c		%dr% %2
xcopy demo*.prj		%dr% %2
xcopy *.bat			%dr% %2 



:QUIT
set dr=
exit

