echo off
echo LATTICE: Large memory model
if exist %1 goto syntax
\lc\lc1 =30000 >%1.err -mp-n -i\lc\ -i\lc\p\ %1.c
if errorlevel 1 goto err1
echo LATTICE: Phase 1 processing complete for %1
del %1.err
\lc\lc2 -k2 %1
goto quit
:err1
echo LATTICE: Errors detected in %1
echo          Merging errors into %1.C source file
errmerge %1
del %1.err
set EXIT=%1
goto quit
:syntax
echo LATTICE: A file EXIST error for %1
echo          Rerun without file extent.
:quit
rr
set EXIT=%1
goto quit
:sy