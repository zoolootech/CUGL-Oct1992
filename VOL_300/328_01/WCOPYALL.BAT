@echo off
ECHO this batch file copies all the windows files to A: B: or C:
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
xcopy w*.bat 		%dr% %2
xcopy makehx.c 		%dr% %2
xcopy txt2mcr.c 	%dr% %2
xcopy dir2bat.c 	%dr% %2
xcopy *.h	    	%dr% %2
xcopy header.dsk    %dr% %2
xcopy *.bat		 	%dr% %2

echo PLACE DISK FOR DEMO and DOC FILES IN DRIVE, PRESS ANY KEY
PAUSE

xcopy header.dsk  	%dr% %2
xcopy *.doc     	%dr% %2

xcopy window.h   	%dr% %2
xcopy heapdbg.c 	%dr% %2
xcopy demo*.c 		%dr% %2
xcopy demo*.h*  	%dr% %2
xcopy dosmenu.c		%dr% %2
xcopy makehx.exe  	%dr% %2
xcopy txt2mcr.exe	%dr% %2
xcopy dir2bat.exe   %dr% %2
xcopy dir2bat.com   %dr% %2

echo PLACE DISK for TURBOC LIBRARY FILES in drive, press any key
PAUSE

xcopy header.dsk    %dr% %2
xcopy w*.lib 		%dr% %2
xcopy t*.bat 		%dr% %2
xcopy demo*.prj 	%dr% %2

echo PLACE DISK for Microsoft C LIBRARY FILES in drive, press any key
PAUSE
xcopy header.dsk    %dr% %2
xcopy mw*.lib 		%dr% %2
xcopy m*.bat 		%dr% %2

:QUIT
set dr=
exit

