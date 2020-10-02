if x%1==x set f1=csubrs
if NOT x%1==x set f1=%1
if x%2==x set f2=%f1%
if NOT x%2==x set f2=%2
if x%3==x set f3=c:\c\
if NOT x%3==x set f3=%3
if NOT exist %f1%.arc goto noarc
if exist %f2%.s echo %f2%.s exists, press ENTER to continue or ^C to abort
if exist %f2%.s pause 
echo .      >error.lst
echo .      >>error.lst
echo The following routines had errors and were not added to the library: >>error.lst
echo      (see ERROR.LST if you wish further processing)  >>error.lst
echo .      >>error.lst
rem
rem build bat file to compile c subroutines
rem
echo echo off >temp.bat
echo arc e %f1% %%1.c >>temp.bat
echo echo %%1.c ">>error.lst" >>temp.bat
echo %f3%c88 %%1 -i%f3% ">>error.lst" >>temp.bat
rem
echo erase %%1.c >>temp.bat
echo if NOT errorlevel 1 echo %%1 ">>temp.inp" >>temp.bat
chg " . <temp.bat >sxc.bat
rem
rem build bat file to assemble assembly subroutines
rem
echo echo off >temp.bat
echo arc e %f1% %%1.a >>temp.bat
echo echo %%1.a ">>error.lst" >>temp.bat
echo %f3%asm88 %%1 -i%f3% ">>error.lst" >>temp.bat
rem
echo erase %%1.a >>temp.bat
echo if NOT errorlevel 1 echo %%1 ">>temp.inp" >>temp.bat
chg " . <temp.bat >sxa.bat
rem
rem build bat file to drive compile c subroutines
rem
echo echo off >temp1.bat
arc l %f1% *.c | grep @.c | trunc . >temp.csr
chg ^.*$ @%%comspec@%%#@/csxc@.bat#& <temp.csr >>temp1.bat
rem
rem build bat file to drive assemble assembly subroutines
rem
echo echo off >temp2.bat
arc l %f1% *.a | grep @.a | trunc . >temp.csr
chg ^.*$ @%%comspec@%%#@/csxa@.bat#& <temp.csr >>temp2.bat
rem
rem compile all c and assembly subroutines using created stuff
rem
%comspec% /ctemp1.bat
%comspec% /ctemp2.bat
rem
rem build input file for lib88 to build the *.s library
rem
copy temp.inp temp2.inp
echo -o%f2% >>temp2.inp
echo -p%f2%.map >>temp2.inp
rem
rem link it all into a new library
rem
%f3%lib88 -ftemp2.inp
rem
rem Show which routines had errors
rem
more <error.lst 
rem
rem erase all the temporary files
rem
echo To erase the temporary files, press ENTER, else press ^C....
pause 
erase sxc.bat
erase sxa.bat
erase temp.cs?
erase temp?.bat
erase temp2.inp
chg ^[A-Z0-9_]* xel#&@.o <temp.inp >temp1.bat
%comspec% /ctemp1.bat
erase temp.inp
erase temp1.bat
goto end
:noarc
echo %f1%.arc does not exist, aborting....
:end
set f1=
set f2=
