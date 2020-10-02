rem  Compile batch file for XDIR using ECO-C
cc -ed -wd: -nl util secure zdir display ydir
cc -ed -wd: -nl xdir ctlr getapage misc
masm testprt;
lib xdlib +util +secure +zdir +display +ydir +ctlr +getapage +testprt +misc;
link xdir,,nul,xdlib+\util\bin\ecos+\util\bin\ecoc;
