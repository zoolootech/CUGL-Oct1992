lc -ms -i\lc\ -i\lc\s\ YAM%1 >err%1
if errorlevel 1 goto err
llyam
goto done
:err
type err%1
:done
