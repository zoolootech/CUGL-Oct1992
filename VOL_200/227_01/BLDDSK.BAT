echo off
echo *** creating CUG227
echo *** please insert disk in drive A:
pause
copy allglob.h     A: >nul
copy basnames.h    A: >nul
copy bios.c        A: >nul
copy bios.h        A: >nul
copy blddsk.bat    A: >nul
copy btrigtab.c    A: >nul
copy dos.mac       A: >nul
copy ega.c         A: >nul
copy ega.h         A: >nul
copy egaa.asm      A: >nul
copy graf*.c       A: >nul
copy graf.s        A: >nul
copy graf.txt      A: >nul
copy graphics.h    A: >nul
copy herc.c        A: >nul
copy herc.h        A: >nul
copy herca.asm     A: >nul
copy readme.doc    A: >nul
copy stdfont.h     A: >nul
copy trigtab.h     A: >nul
copy xxhr.c        A: >nul
copy xxhr.h        A: >nul
if not exist A:graf.s   echo A:graf.s missing!
if not exist A:graf.txt echo A:graf.txt missing!
label A:CUG227
dir A:
echo *** disk creation complete
