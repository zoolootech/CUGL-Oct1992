


base=C:\tc\lib

lif.exe: lif.obj  menu.obj
   tlink /x lif.obj $(base)\c0c.obj menu.obj,lif.exe,,$(base)\graphics.lib $(base)\cc.lib

lif.obj: lif.c
   tcc -c -DTC -d -f- -G -K -mc -N- -O -w -Z -1 -I\etc;incl -LC:\tc\lib;lib -w-pro  -w-rch lif.c

menu.obj: menu.c
   tcc -c -DTC -d -f- -G -K -mc -N- -O -w -Z -1 -I\etc;incl -LC:\tc\lib;lib -w-pro  -w-rch menu.c
