@rem name:    ccldos.cmd
@rem purpose: Compile c program
@echo off
set prog=cpp1
if exist %prog%.c goto doit
echo Usage: %0 CPROGRAM
echo CPROGRAM must have no extension
echo CPROGRAM must exist in current directory
goto exit
:doit
erase %prog%.lst
erase %prog%.exe
set include=c:\include2;c:\include2\sys
set lib=c:\lib2
set tmp=c:\tmp
@echo on
cl -c -AL -W2 cpp6.c
cl -c -AL -W2 cpp5.c
cl -c -AL -W2 cpp4.c
cl -c -AL -W2 cpp3.c
cl -c -AL -W2 cpp2.c
cl -c -AL -W2 %prog%.c
@REM link %prog%,/noi,%prog%,doscalls+llibce+os2, ;
@REM link /st:4000 %prog%,/noi,%prog%,llibce+os2, ;
link /st:4096 %prog%+cpp2+cpp3+cpp4+cpp5+cpp6,cpp /noi,cpp,llibce+os2, ;
@echo off
erase %prog%.lst
erase %prog%.map
set include=
set lib=
set tmp=
erase %prog%.obj
:exit
