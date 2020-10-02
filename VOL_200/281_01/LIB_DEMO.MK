lib_demo.obj	  :  lib_demo.c
   cl /c /DMSC lib_demo.c

lib_demo.exe : lib_demo.obj
 link lib_demo,,,uclib.lib /NOE/ST:14000/CO
