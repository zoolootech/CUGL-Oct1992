@rem name:    ccldos.cmd
@rem purpose: Compile c program
@echo off
set prog1=egrep
if exist %prog1%.c goto doit
echo Usage: %0 CPROGRAM
echo CPROGRAM must have no extension
echo CPROGRAM must exist in current directory
goto exit
:doit
erase %prog1%.lst
erase %prog1%.exe
set include=c:\include2;c:\include2\sys
set lib=c:\lib2
set tmp=c:\tmp
@echo on
cl -c -AL -W2 regexp.c
cl -c -AL -W2 regsub.c
cl -c -AL -W2 regerror.c
cl -c -AL -W2 %prog1%.c
link /st:16384 %prog1%+regexp+regsub+regerror,/noi,%prog1%,llibce+os2, ;
@echo off
erase %prog1%.lst
erase %prog1%.map
set include=
set lib=
set tmp=
erase %prog1%.obj
:exit
