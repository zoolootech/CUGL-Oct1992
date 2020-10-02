rem Batch file to make all sample applications for Turbo C
tmake %1 %2 %3 %4 %5 -fres.tc
if errorlevel 1 goto exit
tmake %1 %2 %3 %4 %5 -fminres.tc
if errorlevel 1 goto exit
tmake %1 %2 %3 %4 %5 -fsnap.tc
if errorlevel 1 goto exit
tmake %1 %2 %3 %4 %5 -fspawn.tc
if errorlevel 1 goto exit
tmake %1 %2 %3 %4 %5 -fmspawn.tc
if errorlevel 1 goto exit
tmake %1 %2 %3 %4 %5 -ftest.tc
if errorlevel 1 goto exit
tmake %1 %2 %3 %4 %5 -ftprt.tc
if errorlevel 1 goto exit
tmake %1 %2 %3 %4 %5 -ftsio.tc
if errorlevel 1 goto exit
tmake %1 %2 %3 %4 %5 -ftestmod.tc
:exit
