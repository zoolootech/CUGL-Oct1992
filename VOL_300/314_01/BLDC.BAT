copy *.h \include
cl /c /Gs mnpintrf.c mnpllvl.c mnpdrvr.c mnpevent.c mnpmisc.c
masm async,async;
masm suspend,suspend;
masm fcscalc,fcscalc;
masm setpar,setpar;
masm timer,timer;
masm portio,portio;
erase mnp.lib
lib @bldlib
erase mnpintrf.obj
erase mnpllvl.obj
erase mnpdrvr.obj
erase mnpevent.obj
erase mnpmisc.obj
erase async.obj
erase suspend.obj
erase fcscalc.obj
erase setpar.obj
erase timer.obj
erase portio.obj
