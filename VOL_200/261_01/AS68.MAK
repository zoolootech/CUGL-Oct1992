# as68.mak - make file for as68 program
# (note - this is setup for the Polytron "PolyMake")
# 03-03-87 srd: create - setup for codeview debuggable

# Explicitly state target
#
as68 : as68.exe

# General rule for making an object file from a .C file
#
.c.obj :
	cl -DLINT_ARGS -AS -Od -Ze -Zi -Zp -Fo$@ $< -c >>as68.err

# Where the .obj's and .lib's are supposed to live
#
.PATH.obj = bin
.PATH.lib = c:\lib

# List of all the libraries used...
#
LIBS = slibce.lib


# List of all object files
#
OBJS =\
as68.obj \
aserr.obj \
asetc.obj \
asinit.obj \
asmt.obj \
bc.obj \
bc1.obj \
bc2.obj \
lib68.obj \
pop.obj \
psdo.obj \
psdo1.obj \
psdo2.obj \
symtbl.obj


# Link modules to create as68.exe
#
as68.exe : $(OBJS) $(LIBS)
 	link <@<
$[s," +\n",$[f,$(.PATH.obj),$(OBJS)]]
$* /NOD /MAP /CO
$*
$(LIBS)
< >as68.lst


