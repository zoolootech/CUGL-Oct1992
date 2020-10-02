rename csubrs.s xlib.s
lib88 %1 xlib.s -ocsubrs -pcsubrs.map
more <csubrs.map
pause .. if map is ok, press ENTER, else press ^C
del %1.o
del xlib.s
