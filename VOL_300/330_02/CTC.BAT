rem Batch-file to make Turbo C versions of the CTask kernel
rem and support routines.
tmake -fctask.tc
if errorlevel 1 goto exit
tmake -fctsup.tc
:exit
