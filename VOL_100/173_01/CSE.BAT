echo off
echo LATTICE: Small memory model
if exist %1 goto syntax
\lc\lc1 >%1.err  -mS -n -i\lc\ -i\lc\s\ %1.c
if errorlevel 1 goto err1
echo LATTICE: Phase 1 processing complete for %1
del %1.err
\lc\lc2 -y -k8086 %1
goto quit
:err1
echo LATTICE: Errors detected in %1
echo          Merging errors into %1.C source file
errmerge %1
del %1.err
goto quit
:syntax
echo LATTICE: A file EXIST error for %1
echo          Rerun without file extent.
:quit
 %1
del %1.err
goto quit
:syntax
echo L