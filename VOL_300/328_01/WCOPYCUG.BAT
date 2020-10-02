@echo off
ECHO Copy windows files for public domain submission.
ECHO parameter 1: drive, default=a: (must specify letter, colon)
ECHO parameter 2: switch for xcopy  (specify /d:mm-dd-yy)
set dr=A:
if NOT (%1) == () set dr=%1

for %%a in ( A: a: b: B: C: c: ) do if (%dr%)==(%%a) goto :OK

ECHO ILLEGAL DRIVE SPECIFICATION ( a: b: or c: only )
goto :quit

:OK

echo PLACE SOURCE DISK IN DRIVE, PRESS ANY KEY
ECHO REMOVED FOR CUG SUBMISSION
REM PAUSE

REM xcopy w*.c			%dr% %2
REM xcopy w*.bat 		%dr% %2
REM xcopy makehx.c 		%dr% %2
REM xcopy txt2mcr.c 	%dr% %2
REM xcopy dir2bat.c 	%dr% %2
REM xcopy *.h	    	%dr% %2
REM xcopy headers.dsk   %dr% %2
REM xcopy *.bat		 	%dr% %2

echo PLACE DISK FOR DEMO and DOC FILES IN DRIVE, PRESS ANY KEY
PAUSE
xcopy submit.dsk	%dr% %2
xcopy header.dsk  	%dr% %2
xcopy *.doc     	%dr% %2

xcopy window.h   	%dr% %2
xcopy heapdbg.c 	%dr% %2
xcopy demo*.c 		%dr% %2
xcopy demo*.h*  	%dr% %2
xcopy makehx.exe  	%dr% %2
xcopy txt2mcr.exe	%dr% %2
xcopy dir2bat.exe   %dr% %2
xcopy dir2bat.com   %dr% %2
xcopy t*.bat 		%dr% %2
xcopy demo*.prj 	%dr% %2
xcopy m*.bat 		%dr% %2

echo PLACE DISK for LIBRARY FILES in drive, press any key
PAUSE
xcopy w*.lib 		%dr% %2
xcopy mw*.lib 		%dr% %2

:QUIT
set dr=
exit

