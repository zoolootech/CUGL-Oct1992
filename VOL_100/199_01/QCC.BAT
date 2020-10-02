: qcc.bat - 12/01/87 -  mass compile for microsoft c
:
erase err.lst
:
tm  start /C1  /n
:
%1cl  /c  /DMSC  /Zi  /Od   %2   ged.c    >>  err.lst
%1cl  /c  /DMSC  /Zi  /Od   %2   ged1.c   >>  err.lst
%1cl  /c  /DMSC  /Zi  /Od   %2   ged2.c   >>  err.lst
%1cl  /c  /DMSC  /Zi  /Od   %2   ged3.c   >>  err.lst
%1cl  /c  /DMSC  /Zi  /Od   %2   ged4.c   >>  err.lst
%1cl  /c  /DMSC  /Zi  /Od   %2   ged5.c   >>  err.lst
%1cl  /c  /DMSC  /Zi  /Od   %2   ged6.c   >>  err.lst
%1cl  /c  /DMSC  /Zi  /Od   %2   ged7.c   >>  err.lst
%1cl  /c  /DMSC  /Zi  /Od   %2   ged8.c   >>  err.lst
%1cl  /c  /DMSC  /Zi  /Od   %2   ged9.c   >>  err.lst
%1cl  /c  /DMSC  /Zi  /Od   %2   ged10.c  >>  err.lst
%1cl  /c  /DMSC  /Zi  /Od   %2   ged11.c  >>  err.lst
:
tm  stop  /C1
