tdata.obj      :  tdata.c
   cl /c /DMSC tdata.c

tdata.exe : tdata.obj
 link tdata,,,uclib.lib /NOE/ST:14000/CO
