rem batch file to link for microsoft C object codes
rem stack size must be given as indicated
rem paths may be accordingly midified

set lib=d:\msc\lib

link /F /PAC /STACK:16384 %1+mvidh.lib;
