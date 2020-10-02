break on
echo ===========: >>lcc.out
echo New compile: >>lcc.out
:loopcc
if "%1"=="++" goto dolink
if "%1"=="" goto done
echo %1: >>lcc.out
lc1  %1 -a -c -d -n -mS  -ic:\lc\ >>lcc.out
if errorlevel 1 goto bustcc
lc2 %1 >>lcc.out
if errorlevel 1 goto bustcc
goto nextloop
:bustcc
if exist %1.obj del %1.obj
:nextloop
shift
goto loopcc
:dolink
shift
if exist %1.exe del %1.exe
link C+%1%2+error,%1,nul,LCM+LC >>lcc.out
if not exist %1.exe goto bust
goto done
:bust
if exist %1.exe del %1.exe
echo unsuccessful compile >>lcc.out
:done
