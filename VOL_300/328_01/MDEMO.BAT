REM this batch file makes all the demo programs 
REM for MICROSOFT C
REM for QUICK C: change the cl command to qcl to get the quick c compiler.
REM              demomenu won't compile under quick c because of a bug in it's 
REM 			 	preprocessor - doesn't do 'stringizing' correctly.

if exist %lib%\graphics.lib  goto  :havelib
	echo Modify this bat file to specify the microsoft c graphics library
	echo or place the path to the graphics library in the environment.
	echo .
	echo demo programs not compiled.
	goto :done
:havelib 
cl /J /AM  demomous.c mwgm.lib  %lib%\graphics.lib

cl /J /AM  demoform.c mwgm.lib  %lib%\graphics.lib

cl /J /AM  demomenu.c mwgm.lib	%lib%\graphics.lib

cl /J /AM  demomacr.c mwgm.lib	%lib%\graphics.lib

cl /J /AM  demoheap.c mwgm.lib	%lib%\graphics.lib

cl /J /AM  demopage.c mwgm.lib	%lib%\graphics.lib 

cl /J /AM  menuw.c    mwgm.lib	%lib%\graphics.lib

ECHO all done.
:done
