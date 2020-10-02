# Generic make for a single module program.
#
.SUFFIXES
OBJPATH =o\ 
.PATH.obj=o
DEBUG = -Zi -Od


#
# This rule says a .OBJ file should be created by
# using the MS C (version 5.0) compiler on a .C file.
#
.c.obj	:
	cl -c -Zp -AS -Gs -Fo$(OBJPATH)$*.obj $<

all	        : est.exe

est.exe         : est.obj
		link $(OBJPATH)est;

