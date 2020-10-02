cc1 %1
if errorlevel 1 goto done
cc2 %1
if errorlevel 1 goto done
cc3 -n %1
if errorlevel 1 goto done
cc4 %1
if errorlevel 1 goto done
:done
