cc09 test.c test.asm
if errorlevel 1 goto nocomp
as6809 -los test
aslink -fp test.lnk
pod test.s19
goto exit
:nocomp
echo Compiler Failure
:exit
