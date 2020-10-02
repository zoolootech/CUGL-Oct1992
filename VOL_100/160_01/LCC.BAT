break on
echo =========== >>lcc.out
echo New compile: >>lcc.out
echo %1: >>lcc.out
lc1  %1 -a -c -d -n -mS  -ic:\lc\ >>lcc.out
if errorlevel 1 goto bustcc
lc2 %1 >>lcc.out
if errorlevel 1 goto bustcc
goto dolink
:bustcc
if exist %1.obj del %1.obj
goto bust
:dolink
if exist %1.exe del %1.exe
link C+%1+error,%1,nul,LCM+LC >>lcc.out
if not exist %1.exe goto bust
echo successful compile
goto done
:bust
echo unsuccessful compile >>lcc.out
echo unsuccessful compile
:done
