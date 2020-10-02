rem use: d:qedcomp %compdrv %srcdrv %tempdrv
rem e.g.:  b:qedcomp %c %b %d
rem goto resume
%1:c88 %2:qed. d%1 t%3
if errorlevel 1 goto stop
%1:c88 %2:qed1. d%1 t%3
if errorlevel 1 goto stop
%1:c88 %2:qed2. d%1 t%3
if errorlevel 1 goto stop
%1:c88 %2:qed3. d%1 t%3
if errorlevel 1 goto stop
%1:c88 %2:qed4. d%1 t%3
if errorlevel 1 goto stop
%1:c88 %2:qed5. d%1 t%3
if errorlevel 1 goto stop
%1:c88 %2:qed6. d%1 t%3
if errorlevel 1 goto stop
:resume
%1:c88 %2:qed7. d%1 t%3
if errorlevel 1 goto stop
%1:c88 %2:qed8. d%1 t%3
if errorlevel 1 goto stop
%1:c88 %2:qed9. d%1 t%3
if errorlevel 1 goto stop
%1:c88 %2:qed10. d%1 t%3
if errorlevel 1 goto stop
%1:c88 %2:qed11. d%1 t%3
if errorlevel 1 goto stop
%1:c88 %2:qedibmpc. d%1 t%3 t%3
if errorlevel 1 goto stop
rem %2:qedbind
:stop
