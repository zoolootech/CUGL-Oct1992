rem Batch-file to make Turbo C versions of the CTask kernel
rem and support routines.
rem Large model
tmake -fctaskh.tc
if errorlevel 1 goto exit
tmake -fctsuph.tc
:exit

