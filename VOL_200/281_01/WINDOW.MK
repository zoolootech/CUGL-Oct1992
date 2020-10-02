#  Make file for window.c

window.obj  :  window.c
   cl /AL /DMSC /c window.c

window.exe : window.obj
   link window,,,uclrg.lib
