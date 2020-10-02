echo off
rem Batch file to run explod with a delay parameter corresponding to the
rem machine speed.  Users who know better can just run with a command
rem of "explod -dM <data files>", where M is a delay factor from 0-50.
rem ======================================================================

echo For operation at the proper speed, you should run this program
echo using the command "run N" where N is a number from the following list, 
echo corresponding to the speed of your machine:
echo     1. 4.77 MHz XT
echo     2. 8 MHz XT
echo     3. 8 MHz '186
echo     4. 6 MHz AT
echo     5. 8 MHz AT
echo     6. 10 MHz AT
echo     7. 12 MHz AT
echo     8. 16 MHz AT (or 386)
echo     9. 20 MHz AT (or 386)
echo    10. 25 MHz AT (or 386)
echo .
echo During the fireworks display, you can press ESCAPE to exit this program.
echo .

if %1X==X goto unknown
if %1==1 goto xt477
if %1==2 goto xt8
if %1==3 goto xat8
if %1==4 goto at6
if %1==5 goto at8
if %1==6 goto at10
if %1==7 goto at12
if %1==8 goto at16
if %1==9 goto at20
if %1==10 goto at25
if %1==N goto dummy
echo The number that you gave was not on the list!  Use a number from 1 to 10.
goto exit

:unknown
echo You didn't give a number, so I'm assuming number 5.
pause
goto at8

:xt477
explod -s5 r*.dat k*.dat sp*.dat t*.dat p*.dat d*.dat o*.dat s*.dat c*.dat
goto exit

:xt8
explod -s7 r*.dat k*.dat sp*.dat t*.dat p*.dat d*.dat o*.dat s*.dat c*.dat
goto exit

:xat8
explod r*.dat k*.dat sp*.dat t*.dat p*.dat d*.dat o*.dat s*.dat c*.dat
goto exit

:at6
explod -d5 r*.dat k*.dat sp*.dat t*.dat p*.dat d*.dat o*.dat s*.dat c*.dat
goto exit

:at8

goto exit

:at10
explod -d15 r*.dat k*.dat sp*.dat t*.dat p*.dat d*.dat o*.dat s*.dat c*.dat
goto exit

:at12
explod -d20 r*.dat k*.dat sp*.dat t*.dat p*.dat d*.dat o*.dat s*.dat c*.dat
goto exit

:at16
explod -d30 r*.dat k*.dat sp*.dat t*.dat p*.dat d*.dat o*.dat s*.dat c*.dat
goto exit

:at20
explod -d40 r*.dat k*.dat sp*.dat t*.dat p*.dat d*.dat o*.dat s*.dat c*.dat
goto exit

:at25
explod -d50 r*.dat k*.dat sp*.dat t*.dat p*.dat d*.dat o*.dat s*.dat c*.dat
goto exit

:dummy
echo To run this program, do not use the command "run N" literally.  Use a
echo number instead of "N".  For example, use "run 2" or "run 7".

:exit
