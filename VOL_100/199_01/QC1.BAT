: qc1.bat - 11/29/87 - compile a single file
:
tm start /n
:
%1cl  /c  /DMSC  /Zi  /Od  %2   %3
:
tm  stop
