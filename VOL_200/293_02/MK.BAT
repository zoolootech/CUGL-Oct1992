if "%1" == "" cl /AC /FPi87 %FILE%
if "%1" == "" goto exit
if exist %1 cl /AC /FPi87 %1
if exist %1 goto exit
if not exist %1 cl /AC /FPi87 %1.c
if not exist %1 goto exit
:exit