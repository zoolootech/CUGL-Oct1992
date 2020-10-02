echo off
echo C86 preprocessor beginning execution.  Module: %1
c:cc1 >%1.cc1 %2 %3 %4 %5 %1
if errorlevel 1 goto cc1fail
echo C86 syntax checker beginning execution.  Module: %1
c:cc2 >%1.cc2 %1
if errorlevel 1 goto cc2fail
echo C86 code generator beginning execution.  Module: %1
c:cc3 >%1.cc3 %1
echo C86 linker beginning execution.  Module: %1
c:ccl >%1.ccl %1 ulib a:clib
if errorlevel 1 goto cclfail
del %1.cc?
echo C86 compile and link complete.  Module: %1
goto exit
:cc1fail
echo C86 preprocessor detected error.  Module: %1
type %1.cc1
goto exit
:cc2fail
echo C86 syntax checker detected error.  Module: %1
type %1.cc2
goto exit
:cc3fail
echo C86 code generator detected error.  Module: %1
type %1.cc3
goto exit
:cclfail
echo C86 linker detected error.  Module: %1
type %1.ccl
:exit
