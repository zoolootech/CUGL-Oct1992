echo off
cls
echo This batch file installs the Shell and its associated files on a hard
echo disk (presumed drive C).   It will create a directory off the root called
echo \SHBIN and place them there, then it will boot the Shell.   You will want
echo to eventually place SH.EXE and CONFIG.SH in a directory on the Path for
echo regular use.   If you do NOT want to continue this, press Ctrl-C or
echo Ctrl-Break, otherwise...
pause 
cls
echo  Making the directory: C:\SHBIN 
md c:\shbin
echo Making the log directory: C:\SHBIN\LOGS
md c:\shbin\logs
echo Now using cp to copy the utilities into C:\SHBIN
cp *.* c:\shbin
pause
cls
echo The Shell is now installed on the Hard disk, press Ctrl-Break if you do
echo NOT want to start the Shell, otherwise...
pause
sh -m menu;c:;help
