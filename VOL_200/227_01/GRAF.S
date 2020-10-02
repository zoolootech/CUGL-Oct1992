.tr ~ 
.SA 1
.PH "''Compatible graphics library''"
.PF "''Page \\\\nP''"
.DS C
Public Domain Compatible Graphics Library
.sp 1
The C User's Group
.sp 1
Author
Rainer Gerhards
Petronellastr. 6
D-5112 Baesweiler
West Germany
.sp 1
Library Version
2.1, 11/15/1987
.sp 1
Documentation Version
2.1 (11/15/87 16:56:50)
.sp 2
supported operating systems
MS-DOS or PC-DOS 2.0 or higher
.sp 1
supported C compilers
Lattice C 3.0
Datalight C 2.23
Borland Turbo C 1.0
[Microsoft C]
.sp 1
supported graphics cards
Hercules monochrome graphics card
enhanced graphics adapter (EGA)
IBM std. BIOS interface
Elsa PC XXHR vector graphics board
.sp 1
supported graphics printer
Nec pinwriter P6 / P7
Epson \(*TM compatible printers
.sp 1
supported plotters
--- currently none ---
.DE
.bp
.ll -10
.in +10
.fi
This document describes the compatible
graphics library for almost every (strong words, I know)
computer. Everybody is free to copy,
adapt or modify the listed sources
and documentation. It would be nice, if
you copy this acknowledgment and the authors
name and address too.
.br
If you have new ideas or improvements
please contact the author.
.ll +10
.in -10
.sp 0
.SK
.H 1 "Introduction"
.H 2 "Design history and background"
.fi
This library was first designed as a demonstration library for very fast
assembly language routines.
But after a while using it, I think that this C library is much more powerful
than the assembly language library is.
Ok, the assembly language library is somewhat faster, but it hasn't the ability to run on all well-known graphics devices for the IBM PC.
And I think it will be a horrible task to port the library to a really different
environment - an Unix machine for example.
This is a much more easy task with the C language library.
.br
So this C library builds the base of a compatible graphics system, which will
run
on a CP/M machine as well as on a big Unix system.
Currently only parts of the IBM personal computer hardware are supported,
because I own no other hardware. I hope, that other users will help to
make this library really portable and powerful.
.sp 1
While the first version implemented only the basic tasks, the current version
(2.1) has many intersting features and is greatly enhanced
above version 1.1. 
It is very nice to move from one video device to another without source-code
changes.
I also improved the speed and memory requirements of the library dramatically,
and so 
I think this version covers a lot of serious projects.
.sp 1
I hope that some of you
have some interesting ideas and algorithms and want to tell me about them.
So I hope to get a full praxis-driven library.
In addition, I hope that some of you want to port the low-level parts of the
library to different machines and graphics environments, making the library
really portable.
Everybody should feel free to contact me!
.H 2 "Current features"
.fi
The most powerful feature of the library is its compatibility and hardware
independency.
You may use the library on many machines, as long as there's driver
available.
And new drivers can be very simply added.
.sp 1
The library currently supports:
.DL
.LI
switching between different display modes (graphics/nongraphics)
.LI
drawing pixels
.LI
drawing lines
.LI
drawing polylines
.LI
drawing boxes
.LI
filling boxes with given colors (e.g. clear windows)
.LI
draw any ellipses in full or in part
.LI
paint any region on the screen with a given color. The region is defined by
a border color.
.LI
you may use a global coordinate system, freeing you from the given hardware
parameters. Also very useful in numerical applications.
.LI
print graphics screens as a hardcopy function.
This is currently limited to the Hercules graphics card and Epson compatible
printers.
.LI
clear the graphics screen.
.LI
read the color of a specified pixel.
.LI
output text in different styles with user selectable foreground and background
colors.
.LI
use a library error-code system. Errors occurred while processing graphics
Information may be printed with a function like perror.
.LI
a full turtle graphics system
.LI
different device write modes.
New drawing may be modifyed according to the old video contents (XOR them,
e. g.).
.LE
.H 2 "new features and enhancements of version 2.1"
The following new features, enhancements and bug fixes have been applied to
the library since version 2.1:
.DL
.LI
A bug in the medium-level header file has been fixed.
The EGA-mode was defined in this header file, resulting in bad coding
sequences for devices other than EGA.
This define has been moved to the right place in the EGA low-level header
file.
.br
Note that the version 1.1 distribution disk was corrupted by this bug.
.LI
A bad bug in herc.c, the C-part of the hercules low-level driver has been
detected and removed.
Mr. Decker of Nuernberg, West Germany reported my, that the given port
address in this low-level driver were incorrect.
As a result, the switch into the hercules graphic mode was unsuccessful.
I'm sorry, the Hercules emulation I use on the QuadEGA switched always
correctly to graphics mode, so I don't saw an error.
Sorry to all Hercules users who only saw a black screen!
This bug has been corrected (two addresses, dammed!).
.LI
The low-level driver for the Hercules graphics card and the EGA have been
greatly improved and speeded up.
New internal function calls have been added, resulting in a great speedup of
the whole library code.
This effect is extreme in function paint and line.
.LI
The hercules low-level driver has a much better hardcopy routine. 
The new routine optimizes the print-output before sending it to the printer,
resulting in a great decrease of print-time, if the screen is nearly blank.
The more information is presented on the graphics screen, the less the
decrease of print time will be, but I think it's really good for many
applications.
.LI
The stack space needed by function paint has been dramatically reduced.
It now requires only about 20% of the original space.
Also the processing time has been reduced.
.LI
The assembly language macros have been implemented.
Version 1.1 used the Lattice macros, which couldn't be distributed due
to copyright limitations.
The now written macro package implements all used Lattice macros in a manner,
that makes moving to a new compiler an relatively easy task.
Also some macros have been defined for this reason, which weren't implemented
in the Lattice package.
You may find the macro package also of interest for your own assembly language
routines, the documentation should be good enough to use them.
They are not documented as part of the library, because their internal
implementation is not guaranteed to be the same in the next release.
.br
Nevertheless, there are some parameters the user must set to correct assemble
the sources.
This parameters are described below.
.LI
A new set of parameters have been added to the medium-/high-level header file 
"graphics.h".
These parameters allows it the user to modify some minor configuration
parameters.
The usage of this mechanism will be described below.
.LI
The text-output system has been added.
It is currently not complete implemented, but offers a good chance to do
most text-output jobs.
.LI
The new function polyline has been added.
It allows the user to draw a complex figure or line with only one function
call.
It is able to work on a variable table which holds line coordinates.
All the coordinates are concatenated with a user-specified color.
.LI
An error system has been implemented.
Functions, which may detect error conditions return -1 if an actual error
occurred, 0 otherwise.
In case of an error return, the global variable "gr_error" may be examined
for the exact error reason.
The function "grperror" has been added to print the error message in a
human-readable form.
It is much like perror, but with increased functionality.
The error system is described in greater detail below.
.LI
A complete turtle graphics system has been added.
This system supports all of the common turtle graphics commands including
turtle positioning, moving and hiding/showing.
This package is described below in more detail.
.LI
A new, powerful feature has been added: You now can specify an so-called
write mode.
This write mode is applied to all pixel drawing operations.
It allows you to modify the display contents in respect to its old state.
E. g. the old pixel color may be XORed with the new color.
.LI
A basic-equivalence header file has been added.
This allows it user's who like the basic names for some graphics primitives
to use them, without destroying compatibility to users (and library
versions), who do not.
.br
This feature is an idea of Michael Yokoyama of Aiea, HI 96701.
.LI
Some documentation bugs have been fixed and the whole documentation has been
upgraded to reflect the latest version of the library.
.LI
The whole library is now maintained internally by Unix SCCS, so the SCCS
version numbering scheme has been adapted.
All version numbers are now starting at one.
The old version 1.1 has been renamed to 1.1.
.LI
Some minor bugs have been fixed.
.H 2 "Release compatibility"
The graphics library is upward compatible.
This means, that programs that used version 1.1 of the library will run
without modification using version 2.1 of the library.
Note that not all 2.1 functions are supported by version 1.1 of the library,
so you can not move user code from 2.1 to 1.1.
Even if it may be possible (no extended functionality used), you should always
use the latest version of the library because of bug fixes and execution speed.
.sp 1
When moving form version 1.1 to 2.1, you need not to recompile the user code
if you don't need the extended functionality.
.sp 1
Note that low-level drivers written for Version 1.1 of the library are
still functional under 2.1.
If this are assembly language drivers, they should be upgraded to use the
new macro support facility.
This easies the task to move them to other compilers.
.H 1 "Library design"
The library uses three logical parts: one medium-level, completely machine
independent
graphics kernel, a medium-level machine-independent part and one low-level
machine dependent part.
There are header (.h) files for the high- and low-level parts.
.br
The user program hasn't to know which function resides in which part of
the graphics library.
If it uses the official entry points, it must be portable to any new 
video device, if it's supported by the library.
The available entry points are listed under "User program interface".
.H 2 "High-level library part"
The high-level library part is completely hardware and operating system
independent.
It's written entirely in C.
It contains all the more complex drawing operations based on the medium-
and low-level part.
.sp 1
There's no need to recompile the high-level part for the different video
devices.
This makes it possible to use loadable device drivers, which allows
dynamically reconfiguration of the user-program for
different video devices - all at runtime without recompilation.
.sp 1
Currently this feature isn't implemented and there aren't a lot of
functions in the high-level category.
.H 2 "medium-level library part"
This part was called high-level in version 1.1.
It's written completely in C.
It includes all the graphics primitives like line drawing, area filling and
so on.
This part uses no direct hardware parameters or interfaces. 
It uses standardized interfaces to the low-level routines instead.
If you need special coding for special hardware or greater speed, you may
include this coding in the medium-level part, but use conditional compilation
to keep it portable.
.br
Though this part is hardware independent, it needs to be recompiled for 
every new video device.
The medium-level part must know some common hardware parameters like the
screen resolution.
Also there is different coding to be activated for different devices.
This ensures the best throughput by using hardware-specific features and
also ensures compatibility by also implementing a general, sometimes slower
routine.
.sp 1
To perform the recompilation successfully the medium-level routines uses the
information contained in the low-level header files.
So it's very important, that every new device\*f
.FS
device is here used as an alias for the low-level device driver functions.
.FE
defines it's hardware parameters in it's corresponding header-file.
There must also for every device a unique mode-identifier be defined. 
This identifier allows it the medium-level routines
to use special hardware features but remain compatible.
.H 2 "Low-level library part"
The low-level part is written in C and assembler.
This C-coding must use non-standard functions\*F
.FS
like hardware port I/O
.FE
to perform its work.
A recompilation of this parts with different compilers may be difficult.
Only tasks, which couldn't be performed in C are written in assembler.
These are most common only the write and read pixel functions.
As many coding as possible should be written in C!
.sp 1
The low-level functions are called very often from the medium-level routines,
especially the read and write pixel functions.
To guarantee a good throughput, these functions should be optimized.
It is a good practice, to look very sceptic to these two functions and the
whole low-level driver.
.sp 1
The low-level routines may implement as many functions as possible, but the
user program should only use the standard functions of the low-level part.
These are listed in the chapter "User program interface".
Using non-standard functions may cause incompatibilities, because some
hardware may not be able to perform specific tasks.
.br
The list of accessible low-level functions may grew in the future, but it
must be granteed that almost every current video device\*F
.FS
These are not only DOS devices - remember the other popular systems like
Apple Mac and Unix systems.
.FE
is able to perform the task.
An example for such tasks may by bytewise addressing of the video memory
- but is this identical on all video devices? I think no.
So it's a better practice to add new or optimize old high- or medium-level
functions.
They are much more flexible.
.H 2 "Stages of compatibility"
The implementation goal of this library was (and is) her great compatibility
on a wide range of machines.
Though this goal is currently reached nearly 100% in the DOS environment,
the design process showed that
there are some useful functions which can only be implemented on many, but not
all devices.
An example for such functions is the video pages system.
On systems, where is no direct access to the video memory information,
the execution time needed to read a full screen via the read pixel function
is much to large to allow a page change.
But this function is very useful and can be implemented on many video
devices.
If you strict exclude such functions, which can't be implemented an all
devices, you can't implement a graphics paging system.
But if you implement this system only on devices which support them, you
destroy the whole compatibility.
.br
So I decided to declare a class of "pseudo-portable" functions:
These functions are available on almost every video device, but return
an error value to the caller.\*F
.FS
using the now implemented error system.
.FE
This value may say the following: "This function can't be performed by
this video device!".
It's now the responsibility of the user program to make use of this 
information and react correctly.
Note that there is a great difference between this implementation philosophy
and "not to implement" the feature.
An error message may also be generated if the feature is available with the
current video devices, but there are not enough resources available to use
it (not enough memory, e. g.).
So the user program has always to look and react at the error codes.
.sp 1
Though this is a way of implementing hardware-specific features in this
library, I think it should only be used on limited tasks, which should be
common to many video devices.
There should be as less functions which use such hardware-specific features
as possible!
.H 1 "Installation of the library"
This section covers the whole installation process, which must performed
by you in order to get the library working.
To ease this task, only the pure installation process is described, not
the way to make the library suit your meets.
This task is covered in the section "Modifying the library", where useful
information about extended configuration parameters can be found.
.br
But be warned: this section was intended for use by those who want
to part the library to a new environment or make serious changes.
The information presented there is very technical.
.H 2 "Bringing up the library"
The following files are guaranteed to be on the distribution disc.
.DS CB N
File name	file description
graf.s		This documentation file in nroff
		source code format.
graf.txt	This documentation file as pure
		ASCII text.
readme.doc	Contains latest information, which
		is not included in this document.
grafturt.c	First part of the high-level library part,
		contains the turtle graphics system.
graf.c		First part of the medium-level library
		part.
graftext.c	Second part of the medium-level library
		part.
herc.c		First part of the hercules low-level
		driver
herca.asm	Second part of the hercules low-level
		driver.
ega.c		First part of the ega low-level driver.
egaa.asm	Second part of the ega low-level
		driver.
bios.c		Standard BIOS low-level driver.
xxhr.c		Driver for the Elsa PC XXHR graphics card.
btrigtab.c	Program to build trigonometric tables.
graphics.h	High-level header file.
herc.h		Low-level hercules device header file.
ega.h		Low-level ega device header file.
bios.h		Low-level BIOS header file.
xxhr.h		Low-level xxhr header file.
dos.mac		Macro file for the assembly language
		modules.
allglob.h	Header file for programs, which want to use
		the world coordinate system only.
basnames.h	Basic equivalence macros. For users who
		like basic function names.
stdfont.h	Standardfont for use by graftext.c.
trigtab.h	Precomputed trigonometric tables. Should be
		rebuild on the target machine using btrigtab.c.
blddsk.bat	MS-DOS library disk build routine.
.DE
This file list may be extended in the future.
.sp 1
The distribution disk shipped to you may contain additional object (.obj)
files.
Especially the assembly language files will come per-assembled, because
not everybody will own an assembler.
.br
But the availability of object files will be depending on the free space
on the distribution disk.
I try to copy all object files for every supported compiler and memory
model to the distribution disk.
But these files need a lot of space, so it can't be guaranteed that these
files will be on the disk.
.sp 1
Look at the file "readme.doc" for the current file organization on the
distribution disk.
.sp 1
To install, you have first to copy all the files you need from the
distribution disk to either a new floppy or your working directory on
your hard-disk.
You need at least the high- and medium-level source code and header files and
one
complete (C and ASM source and header file) low-level driver.
.br
If the objects for your compiler and memory model are on the distribution
disk, the installation is done and you can begin using the library.
If not, you have to compile the source files with your compiler.
If your compiler is not currently supported by the library, switch to
section "Modifying the library", where some useful information for the port
to a new compiler can be found.
Also switch to this section, if you want to make some configuration changes
to the library.
.br
To compile the delivered version of the library, first choose which
graphics device do you want to use.
After that, copy the corresponding
low-level header to the generic low-level header
file "graf.h".
After that, simply compile the high- and medium-level library files as listed
above.
Than compile the low-level file and assemble the assembly language
part of the low-level driver, if given.
.br
Note that you have to modify the file "dos.mac" to your requirements to
perform the assembly successful.
See the following subsection "Configuring dos.mac" for information about
this task.
.br
You should combine the resulting object files to a single library, which
simplifies the link-procedure.
.br
If you need multiple memory model support, perform the above steps for
each memory model you selected to use.
.sp 1
After doing the compilations, you may have to modify your
link-batchfiles. For example, the Lattice C compiler comes with 8 batchfiles
named LINKS, LINKD, LINKP, LINKL, LINKMS, LINKMD, LINKMP, and LINKML.
These batchfiles don't allow you to specify your own librarys. You have
to modify them like in the following example:
.DS
LINKS contains:
link /lc/s/c +%1 %2,%1,%1/map,/lc/s/lc
.sp 1
You modify this to:
link /lc/s/c +%1 %2,%1,%1/map,/lc/s/lc+"name of your graphic library"
.sp 1
Note: Slash (/) were used as path delimiter!
.DE
After you have done this, you are ready to use the library.
.sp 1
If you use the a compiler with cc-like utility (integrated compile/link),
just name your library on the command line.
It will be automatically included in the link procedure.
.H 2 "Configuring dos.mac"
The assembly routines need to know about the environment, in which they
have to execute.
They have to know about the calling compiler and the memory model used.
This environmental data is kept in file dos.mac.
.br
In order to successful reassemble the assembly language files, you have
to modify file "dos.mac" so that it represents the execution environment you
decided to use.
In order to ease this job, there are predefined tags in file "dos.mac".
The following display list is extracted from dos.mac and lists these tags 
and gives a brief description.
.DS
;
;	Define Memory-model to use
;	--------------------------
;	Equate the memory model to use to 1, all others to 0.
;
S_MODEL	equ	1
D_MODEL	equ	0
P_MODEL	equ	0
L_MODEL	equ	0

;
;	Define C-compiler to use
;	------------------------
;	Equate the label, that describes your C-compiler to his current
;	version (major version number * 10 + 1st digit of minor version
;	number). All other labels must be equated to 0.
;
LC	equ	0			; Lattice C
DLC	equ	22			; Datalight C V. 2.2x
MSC	equ	0			; Microsoft C  NOT TESTED
TC	equ	0			: Borland Turbo C
.DE
As listed above, you have to equate the memory model to use to 1, all others
to 0.
If your compiler supports different memory models, select one of the above
according to the following table:
.DS
Model	Code	Data
S	64K	64K
P	unlmtd	64K
D	64K	unlmtd
L	unlmtd	unlmtd
.DE
The equate of your compiler to his version number makes calling-sequence
changes\*F
.FS
that may occur during compiler development
.FE
transparent to the graphics library.
.br
Note that these configuration tags will be extensive error-checked and
all detected errors will be reported during assembly.
Nevertheless, you should be very carefully, when modifying these parameters,
because not all errors can be detected (e. g. wrong memory model chosed).
.sp 1
Be aware of the fact, that the MSC macros aren't tested yet.
I think the work, but I've currently no access to MSC.
So expect some trouble if you decide to use the MSC driver.
.H 1 "User program interface"
.H 2 "Header files"
The user program interface is defined in the various header (.h) files.
There are two classes of header files: First ONE (and only one) header file
defining the high- and medium-level functions and constants.
.br
This header file is named "graphics.h".
.br
Second there are many header
files defining the low-level functions and constants.
.sp 1
These low-level header files may be used in two different ways:
.br
First you may include the device dependent header files by name, e. g.
"hercgraf.h" for the Hercules graphics card.
This way has the disadvantage of missing hardware independency of the user
code. 
If you want to move to a different video device, you have first to change
the header file names in all of your sources.
.br
The second way to use the header files is to include a generic filename,
e.\ g.
"graf.h".
You may copy than the correct header file (e. g. "hercgraf.h") to this generic
file.
If you want to switch another video device, just copy the new header file to
the generic file.
There is nothing more to do, no code must be changed!
.sp 1
I think this second method is much better and should always be used.
The medium- and high-level routines use this method too\*F.
.FS
See "Modifying the library"
.FE
To aid you in using this "good"\*F
.FS
hope you think so too
.FE
method, the medium-level header file does a #include "graf.h".
If you don't want this, please do a #define NOLLHINC \*F
.FS
NO Low-Level Header file INClude
.FE
before #including the high level header file.
.H 2 "Available function calls"
The following is a brief description of the available function calls.
It is guaranteed that these functions will work in every implementation.
.br
Each function is listed with her name, parameters and a description.
These information can also be found in the sources, from where it is
extracted.
If you need more information about a particular function, look at the
sources.
There can be much more information found than in this document.
But beware of the fact, that the internal algorithms are subject to
change without notice!
.bp
.H 3 "Box function"
.VL 15
.LI name
box
.LI class
medium-level
.LI synopsis
box(x1, y1, x2, y2, color);
.nf
int x1, y1;            upper left corner
int x2, y2;            lower right corner
int color;             border color
.LI description
This function draws a border of "color" covering the
given box. The box is specified through the upper
left (x1, y1) and to lower right (x2, y2) corner. The
box itself isn't modified (e. g. must be cleared
explicitly).
.LE
.bp
.H 3 "circle function"
.VL 15
.LI name
circle
.LI class
medium-level
.LI synopsis
circle(x, y, radius, color, aspect)
.nf
int x, y;              center coordinate
int radius;            circle radius
int color;             circle color
float aspect;          aspect ratio
.fi
.LI description
This function draws a circle according to the given parameters.
Note that this function is only capable of drawing a full circle.
.LI see~also
ellipsis
.LE
.bp
.H 3 "convxco function"
.VL 15
.LI name
convxco
.LI class
medium-level
.LI synopsis
locx = convxco(x)
.nf
float x      x coordinate to convert
int locx     hardware x coordinate
.fi
.LI description
This function converts a global coordinate to a local,
hardware dependent coordinate. Its return value may be
directly passed to other functions.
.LI see~also
convyco, convxdis, convydis
.LE
.bp
.H 3 "convxdis function"
.VL 15
.LI name
convxdis
.LI class
medium-level
.LI synopsis
locxdist = convxdis(xdis)
.nf
double xdis  distance to convert
int locxdist hardware distance
.fi
.LI description
This function converts a global x distance to a local,
hardware dependent x distance.
Its return value may be directly passed to other functions.
.LI see also
convxco, convyco, convydis
.LE
.bp
.H 3 "convyco function"
.VL 15
.LI name
convyco
.LI class
medium-level
.LI synopsis
locy = convyco(y)
.nf
float y      y coordinate to convert
int locy     hardware y coordinate
.fi
.LI description
This function converts a global coordinate to a local,
hardware dependent coordinate. Its return value may be
directly passed to other functions.
.LI see~also
convxco, convxdis, convydis
.LE
.H 3 "convydis function"
.VL 15
.LI name
convydis
.LI class
medium-level
.LI synopsis
locydist = convydis(ydis)
.nf
double ydis  distance to convert
int locydist hardware distance
.fi
.LI description
This function converts a global y distance to a local,
hardware dependent y distance.
Its return value may be directly passed to other functions.
.LI see also
convxco, convyco, convxdis
.LE
.bp
.H 3 "ellipsis function"
.VL 15
.LI name
ellipsis
.LI class
medium-level
.LI synopsis
ellipsis(x, y, rx, ry, ws, we, color)
.nf
int x, y      center coordinate
int rx        x - 'radius'
int ry        y - 'radius'
int ws        begin angle 0..360
int we        end  angle 0..360
int color     line color
.fi
.LI description
This function draws any sort of ellipsis. It is often
called circle, but I think this name should better be
reserved for a function, which only draws a FULL
circle (see above). 
This function may not only draw a circle or any possible
ellipsis, it is also capable of drawing only parts of.
them. This feature is often used in pie-charts. Because
of its great flexibility, this function is much slower
than circle. If you only want a full circle (or
ellipsis) you should call circle.
.LI see~also
circle, fellipsis
.LE
.bp
.H 3 "fellipsis function"
.VL 15
.LI name
fellipsis
.LI class
medium-level
.LI synopsis
fellipsis(x, y, rx, ry, ws, we, color)
.nf
int x, y      center coordinate
int rx        x - 'radius'
int ry        y - 'radius'
int ws        begin angle 0..360
int we        end  angle 0..360
int color     line color
.fi
.LI description
This function is much like ellipsis(), but it's faster because
it uses only full angles to draw the ellipsis.
Note that this function produces good results only for small radius-values.
.LI also
ellipsis, circle
.LE
.bp
.H 3 "fillbox function"
.VL 15
.LI name
fillbox
.LI class
medium-level
.LI synopsis
fillbox(x1, y1, x2, y2, color);
.nf
int x1, y1;            upper left corner
int x2, y2;            lower right corner
int color;             fill color
.fi
.LI description
This function fills a given box with the specified color.
The box is specified through the upper left (x1, y1) and
the lower right (x2, y2) corner.
This function is the counterpart to box, which draws the
border.
.LE
.bp
.H 3 "getpixel function"
.VL 15
.LI name
getpixel
.LI class
low-level
.LI synopsis
color = getpixel(x, y);
.nf
int x, y;	coordinate of the pixel
int color;	returned color of that pixel
.fi
.LI description
This function reads ("gets") the color of a
specified pixel.
.LE
.bp
.H 3 "grperror - print last error"
.VL 15
.LI name
grperror
.LI class
medium-level
.LI synopsis
grperror(fil);
.nf
FILE *fil;      file to put error-message in (or NULL if
                to put on current display).
char *msg;      error-message header text (like perror).
.fi
.LI description
This function prints the last detected error that occurred
in the graphics library.
It has the ability to write to a user text file or to the
current graphics screen. Writing to a file may be useful
for not destroying your graphics display. You chose between
this both modes by your file pointer: if it is NULL, the
function writes to the display, otherwise it uses your file
pointer to write to that file.
No write errors are reported.
The function writes first the user-specified message, than
the internal error-message.
.LE
.bp
.H 3 "gt_backw function"
.VL 15
.LI name
gt_backw
.LI class
high-level
.LI synopsis
gt_backw(count)
.nf
int count;      count to move backward
.fi
.LI description
This function is the opposite to function gt_forwd.
It moves the turtle the specified position in backward
direction.
.LI see also
gt_forwd
.LE
.bp
.H 3 "gt_dir function"
.VL 15
.LI name
gt_dir
.LI class
high-level
.LI synopsis
gt_dir(degrees);
.nf
int degrees;      new turtle direction in degrees.
.fi
.LI description
This function sets the absolute turtle direction in
degrees for further moves.
If the value contained in
"degrees" is invalid, it will be converted into the
range 0..359. Negative degrees will be correctly converted
to their positive counterpart.
.LI see also
gt_tleft, gt_trgth
.LE
.bp
.H 3 "gt_forwd function"
.VL 15
.LI name
gt_forwd
.LI class
high-level
.LI synopsis
gt_forwd(count)
.nf
int count;      count to move forward
.fi
.LI description
This function moves the turtle forward the specified count
of 'logical' pixel (or backward, if negative).
.LI see also
gt_backw
.LE
.bp
.H 3 "gt_hide function"
.VL 15
.LI name
gt_hide
.LI class
high-level
.LI synopsis
gt_hide();
.LI description
The turtle isn't shown after drawing operations if
this function is called.
.LE
.bp
.H 3 "gt_home function"
.VL 15
.LI name
gt_home
.LI class
high-level
.LI synopsis
gt_home();
.LI description
This function sets the turtle to the home position.
After this function call, the turtle will be located
at (0,0) (middle of screen) and points in upward direction.
.LE
.bp
.H 3 "gt_init function"
.VL 15
.LI name
gt_init
.LI class
high-level
.LI synopsis
gt_init();
.LI description
This function initializes the turtle-graphics package.
It clears the graphics screen and initializes the global
to local coordinate system with the default turtle window
size. All turtle parameters are reset to their defaults,
which are as follows:
.sp 1
.DS
- turtle position                    (0,0)
- turtle direction                    0
- turtle visible                      no
- turtle delay                        none
- turtle window                       -400,200,400,-200
- turtle pen up                       no
- turtle pen color                    1
.DE
.sp 1
Note: This function MUST be called before any other turtle
graphics function. If it's not called, the other function
may crash. This function MAY be called at any time to reset
the turtle graphics system.
.LE
.bp
.H 3 "gt_pcolr function"
.VL 15
.LI name
gt_pcolr
.LI class
high-level
.LI synopsis
gt_pcolr(color);
.nf
color;        new pen color
.fi
.LI description
This function sets the turtle pen color. All further
drawing will use the specified color.
.sp 1
Note: The pen status is not modified, so if the pen is lift up,
no drawing will take place.
.LE
.bp
.H 3 "gt_pendn function"
.VL 15
.LI name
gt_pendn
.LI class
high-level
.LI synopsis
gt_pendn();
.LI description
This function lifts the turtle pen down, so that a line
will be drawn by further movements. The pen may be lift
up by function gt_penup.
.LI note
It is not reported as error, if the function is called
while the turtle pen is already lift down.
.LI see also
gt_penup
.LE
.bp
.H 3 "gt_penup function"
.VL 15
.LI name
gt_penup
.LI class
high-level
.LI synopsis
gt_penup();
.LI description
This function lifts the turtle pen up, so that no line
will be drawn by further movements. The pen may be lift
down by function gt_pendn.
.LI note
It is not reported as error, if the function is called
while the turtle pen is already lift up.
.LI see also
gt_pendn
.LE
.bp
.H 3 "gt_setco function"
.VL 15
.LI name
gt_setco
.LI class
high-level
.LI synopsis
gt_setco(x, y);
.nf
int x,y;     new turtle coordinates.
.fi
.LI description
This function sets the turtle to the given position. The
position is a turtle graphics coordinate. No line is draw
while repositioning the turtle.
.LE
.bp
.H 3 "gt_show function"
.VL 15
.LI name
gt_show
.LI class
high-level
.LI synopsis
gt_show();
.LI description
The turtle is shown after each drawing operation if
this function is called.
.LE
.bp
.H 3 "gt_tleft function"
.VL 15
.LI name
gt_tleft
.LI class
high-level
.LI synopsis
gt_tleft(degrees);
.nf
int degrees;      turtle rotate count in degrees
.fi
.LI description
This function rotates the turtle in left direction, using
the rotate count specified in degrees. Negative counts mean
rotate to right position.
.LI see also
gt_trght, gt_dir
.LE
.bp
.H 3 "gt_trght function"
.VL 15
.LI name
gt_trght
.LI class
high-level
.LI synopsis
gt_tleft(degrees);
.nf
int degrees;      turtle rotate count in degrees
.fi
.LI description
This function rotates the turtle in right direction, using
the rotate count specified in degrees. Negative counts mean
rotate to left position.
.LI see also
gt_dir, gt_tleft
.LE
.bp
.H 3 "gt_usrmv function"
.VL 15
.LI name
gt_usrmv
.LI class
high-level
.LI synopsis
gt_usrmv(speedtab);
.nf
int *speedtab;      speed table for moves.
.fi
.LI description
This function allows it the program to give the user control over turtle
movement.
The function accepts single keystroke commands.
The commands are not echoed.
The caller may supply a table of speed values for each
supported speed (0 .. 9). This table consists of 10
int-type entries, which holds the movement entity in the
selected speed. If the user wishes to use the default
speed table, he must supply a NULL pointer.
Commands available:
.DS
0 .. 9 : select turtle speed.
n      : turtle direction north
e      : turtle direction east
w      : turtle direction west
s      : turtle direction south
h      : home turtle
b      : move turtle backward
f      : move turtle forward
l      : turn turtle left 1 degree
L      : turn turtle left 5 degrees
r      : turn turtle right 1 degree
R      : turn turtle right 5 degrees
g      : go, change no direction
q      : quit this function
.DE
.sp 1
Note: The turtle system must be initialized before calling this
function. The current state of the turtle system is not
altered before execution, so all parameters are as set by
you.
.LE
.bp
.H 3 "gtgetdir function"
.VL 15
.LI name
gtgetdir
.LI class
high-level
.LI synopsis
degrees = gtgetdir();
.nf
int degrees;      current turtle direction in degrees.
.fi
.LI description
This function returns the current turtle position in degrees.
The returned value will be in range 0 .. 359.
.LE
.bp
.H 3 "Line function"
.VL 15
.LI name
line
.LI class
medium-level
.LI synopsis
line(x1, y1, x2, y2, color);
.nf
int x1, y1;            starting coordinate
int x2, y2;            ending coordinate
int color;             line color
.fi
.LI description
This function draws a line of "color" between the
starting (x1, y1) and ending (x2, y2) coordinate.
.LE
.bp
.H 3 "paint function"
.VL 15
.LI name
paint
.LI class
medium-level
.LI synopsis
paint(x, y, paintclr, border)
.nf
int x, y;       coordinate of a point within the area
int paintclr;   the color used to paint
int border;     is the color of the border defining the
                area
.fi
.LI description
This function paints an area. The area is defined by a
border of a specified color ("border") and the coordinate
of one pixel within the area (x, y). The color used to
paint is given in "paintclr".
This function uses several subroutines and a recursive
algorithm! It's only the "initializer", the main work is
performed by the other Routines.
.LE
.bp
.H 3 "polyline function - draw complex lines"
This function may be used to draw charts or complex geometric figures.
.VL 15
.LI name
polyline
.LI class
medium-level
.LI synopsis
ret = polyline(color, coords, ...)
.nf
int ret;     0 on success, -1 otherwise.
int color    color of the line
int coords   coordinate list (x, y), ends with -1, -1
.fi
.LI description
This function draws a polyline. That means a line, which
is made of multiple coordinates. You supply as many
coordinates as you need to describe the line in an integer
array. They must be in (x,y) order. The end of the coordinate
list is indicated by the coordinate pair (-1,-1).
The function starts now at the first coordinate and draws a
line from this point to the second coordinate. From there it
draws a line to the third coordinate and so on, until the end
of coordinate list is reached. If the coordinate list is in-
valid, the function returns a true (-1) result, otherwise
it returns false (0). In case of an error return the graphics
error variable may be examined.
The line is drawn in color "color".
.LE
.bp
.H 3 "print screen function - hardcopy"
This function allows a program-controlled hardcopy of the current graphics
page. 
.sp 1
Warning: currently only available for the hercules graphics card!
.VL 15
.LI name
prtgraf
.LI class
medium-level
.LI synopsis
ret = prtgraf();
.nf
int ret;      0 if successful, -1 otherwise.
.fi
.LI description
This function prints the entire graphics-screen on a
dot-matrix printer. The function was developed using a
NEC P6 printer, but should run with little alteration on
every EPSON compatible printer.
The function return 0 if successful, -1 if not. In case
of an error return the error system holds an extended
or code.
.LE
.bp
.H 3 "putmsg - complex text output function"
.VL 15
.LI name
putmsg
.LI class
medium-level
.LI synopsis
putmsg(x, y, foreg, backg, string, font, mode);
.nf
int x, y;            starting coordinate (upper left corner)
int foreg;           foreground color
int backg;           background color
char *string;        string to draw
int font;            font-number to use
int mode;            output mode (font modifier)
.fi
.LI description
This function draws the string pointed to by "string" on the
graphics screen. The string must be \0-terminated. Drawing
begins at x,y, which is the upper left corner of the first
character in the string. The characters are painted in color
foreg, the character-background is painted in color backg.
The font to use must be specified in font. Currently only
STDFONT is supported. Field mode contains the string output
mode, which is a font modifier. This field may be used to
select italics or other output modes. Currently only
OM_NORM, OM_ITAL and OM_RITAL are supported.
.LE
.bp
.H 3 "setgloco function"
.VL 15
.LI name
setgloco
.LI class
medium-level
.LI synopsis
setgloco(x_beg, y_beg, x_end, y_end)
.nf
float x_beg, y_beg    minimum coordinates values
float x_end, y_end    maximum coordinates values
.fi
.LI description
This function initializes the global/local coordinate
system. This system allows you to address your pixels
based on a global coordinate system. This system is
independent from the hardware coordinate system. Global
coordinates may be converted to local coordinates,
which are to be used to address the hardware.
So your application hasn't to look at the present video
hardware but use always a hardware independent own
coordinate system.
In addition you may use floats, not only integers as
coordinates. This is a great advantage in numerical
applications.
.LI warning
This function initializes the system, so any call to the
convert routines will return garbage, until this function
is called!
.LE
.bp
.H 3 "setpixel function"
.VL 15
.LI name
setpixel
.LI class
low-level, old call, compatibility class
.LI synopsis
setpixel(x, y, color);
.nf
int x, y;	coordinate of the pixel
int color;	pixel-color
.fi
.LI description
This function sets a pixel of color "color" at
the specified coordinate.
.LI warning
This function is superseded by function wrtpixel.
It's included mainly for compatibility reasons.
It does not \fInot\fP support the graphics write mode.
Do not use this function call, if you do not absolutely need backward
compatibility to version 1.1 of the library!
.LE
.bp
.H 3 "setwm function"
.VL 15
.LI name
setwm
.LI class
medium-level
.LI synopsis
setwm(mode);
.nf
WRITEMOD mode;			new write mode
.fi
.LI description
This function sets the library write mode. All write
operations are performed in respect to the current
write mode.
For a list of currently supported write modes see the
definition of WRITEMOD in graphics.h!
.LE
.bp
.H 3 "wrtpixel function"
.VL 15
.LI name
wrtpixel
.LI class
medium-level
.LI synopsis
wrtpixel(x, y, color);
.nf
int x, y;              pixel coordinate
int color;             pixel color
.fi
.LI description
This function sets a pixel in the given color at the
specified coordinate. The pixel is set in respect to
the current write mode.
.LE
.H 2 "Linking using object files or librarys"
There are two ways to link the graphics routines into your program.
The first were described above: You may use the librarys build during the
installation process.
Then you have only to alter your link-batchfiles, the rest is automatic.
This is normally the best method.
But this method has the disadvantage of including the librarys in every
of your link-sessions. 
This needs time (especially on floppy-based systems).
So, if you only need the libraries in a few programs, you may think that
this way isn't the right.
.sp 1
Then you may use the second method: You may not only specify the libraries,
you also may specify the routines as stand-alone objects.
So link only the object files in your program.
You have to name the files each link-session (or construct a short batch-file).
Please remember, that there are low, medium and high level routines.
You have to include all of them in the link-process.
.br
Here is a sample link-call using the std. Lattice link-files and the hercules
low-level driver:
.br
LINKMS your-program +graf+herc+herca
.br
Note that you do not always have to include three object files. 
The standard BIOS driver, for example, has only one low-level object file,
because the complete low-level handling can be done in C.
However, I think there will be mostly two low-level drivers.
.br
You may specify the object-file names at any position, the above given is only
one possible specification.
You must only link your main program first, because this is the name Lattice
uses for the .EXE file.
.sp 1
Caution: The general medium-level driver must be compiled separately for each
low-level device (as stated above).
So you have to use different medium-level object files. 
Using wrong medium-level drivers may cause link errors and/or program crashes!
.H 1 "Customization of the library"
This section contains information about how to customize the library to
your specific needs.
.sp 1
In general the library is unmodified usable, though the this version may
not the best for your needs.
In order to make the library more useful, there is a facility to change
the behaviour of the library with only a few configuration parameters.
This parameters are contained in file "graphics.h".
By setting this parameters to new values, you can create a version that's
more special to your needs, without completely modifying the library.
.sp 1
This facility should allow you to configure many library features to your
needs, although it is not as powerful as really modifying library source
code.
But I think it will be powerful enough for most purposes.
.sp 1
The configuration parameters are explained in the top part of "graphics.h".
For a detailed description of configuration changes see this file.
.sp 1
Note: after changing configuration parameters,
\fBthe whole library must be recompiled\fP!
.H 1 "Modifying the library"
If the above described way of customizing the library is not powerful
enough for your needs, or if you plan to port the library to a new
environment, the information needed therefore is contained in this chapter.
.sp 1
But be aware of the fact, that modifying the library requires working
knowledge of the C programming language and porting the library requires
good knowledge of the target machine.
If you do not fulfill the above requirements, ask someone else for help or
you can become frustrated!
.sp 1
The following sections contain information about the internal implementation
and general philosophy of this library.
If you modify the library,
please don't violate the basic concepts,
.FS
like the high-, medium and low-level driver concept
.FE
because such violations will cause problems with newer releases.
It also would be fine, if you not only extend or port the library, but also
update this manual.
If you use new or significant faster algorithms, please notify the author,
so he can make these changes common to all users and include them in further
developments and enhancements.
In either case, please inform the author of bugs.
The will be fixed as fast as possible (if you developed a fix, please send it
too!).
.br
For a general overview see sections "Library design".
You should read this section before continuing here.
.H 2 "Basic rules"
This section points out the basic rules for modifying the library.
NEVER violate these rules, otherwise you can not be sure to move to new levels
of the library without problems.
Coding, which violates these rules, isn't accepted nor enhanced by the author.
.br
I think these rules are so basic software-engeneering rules that I can point
out them for short.
.AL
.LI
Never use compiler-specific features. This is the dead for portability!
.LI
Never use undocumented routines.
They may be removed or modifyed without notice.
.LI
Use only the documented interfaces.
Static variables only contain useful information, if such is guaranteed!
.LI
Never change the documented interface of a function.
.br
You would destroy portability.
If you add new functionality to a procedure, which requires a new parameter,
use a different function call name.
You may internally call the old function to avoid duplicate coding.
.FS
or use sub-subroutines
.FE
.LI
Never introduce a routine which is hardware-specific.
Use at least a pseudo-hardware-independent function, as stated above.
But do use such functions only if absolutely necessary and it is supported
by most video-devices (I think at least 75%).
Remember, that
one goal of the library was the portability on a wide range of machines.
So there have to be as less as possible hardware-dependent functions.
.br
You may use hardware-dependent functions internal, but they should not 
be documented as legal functions.
.LI
You should try to implement new features in the hardware-independent,
high- or
medium-level part of the library.
So porting this function to a new environment isn't painful.
.LI
Never implement hardware-dependent functions in the high- or medium-level part.
.br
If you introduce a hardware-dependent function, which is common to many video
devices (and may be emulated by the others), put this in the low-level parts.
This makes implementation of the library more visible.
In addition, ones who want to port the library to new devices can be sure,
that they must not modify the high- and medium-level part.
.LI
You may modify parts of the medium-level routines to call some hardware-specific
routines
.FS
which are then implemented in the low-level part
.FE
if, and only if, there is also a implementation of the same thing using the
standard read/write pixel interface.
You have to use conditional compilation to activate your coding for a special
video-device.
.br
Such an implementation makes sense, if you can reduce the runtime
significantly.
An example for this is the EGA device.
.LI
Always document a new function or function enhancement.
Use the standard function header
.FS
See the library sources for an example.
.FE
in your source to document a new function.
If you port the library to a new operating system, compiler or video device,
please complete the information in the header page of this document.
.H 2 "Library Modes"
The graphics header-files are included in both user and
library programs.
However, these files have to do different jobs in each case.
In order to simplify maintenance, the header files contain information to
do all jobs correctly.
The current job is selected via the library mode preprocessor variable
LIB_MODE.
This variable has to be defined before #including the graphics header
files.
.sp 1
This variable may hold different values, thus selecting different coding in
the header-files.
Currently modes 0, 1 and 2 are defined, but new can be added on demand.
.VL 8
.LI mode~0
is the user mode.
It is also set if LIB_MODE isn't defined.
In mode 0 all library internal features are disabled.
.LI mode~1
is the primary library mode.
All internal features are enabled, but library variables are declared to
be external.
.LI mode~2
is much like mode 1, except that the library variables will be allocated.
So this mode can only be set in one module.
This module is called the master module, because it allocates the whole
library external data space.
.LE
.H 2 "Function sets"
The medium-level library part contains two function sets.
The function set are almost identical in the calling sequence.
This two sets are introduced by the write mode system.
Generally drawing is done in normal write mode, what means directly with
the user-specified color.
But if it's done in one of the other modes, the color must first selected
according to the user-specified color and the previous contents of the
display buffer.
Thus the second approach is much more time consuming, even because some
optimizations can't be applied to write modes other than normal.
.sp 1
So most functions have two forms: one very fast, strictly optimized normal
write mode version, and one more general but also slower all write modes
function.
This design is completely invisible to the user program, because the user
program always calls the fast alternative and this function switches to its
slower pendant if needed.
.sp 1
Note that this approach is unique to the medium-level library part.
If you plan to extend this part you have to know, that there is a function
which supports write-mode applications.
It's function sel_color(), which automatically selects the drawing color
for the pixel it is called for.
Note that this function will be included in \fIall\fP further releases and
so it's save to use it.
It's not documented because it's invisible to user programs.
For the calling sequence look at the source code function header in file
graf.c.
.H 2 "Modifying High-Level Parts"
The high-level library part has to be completely hardware independent.
So no calls to hardware specific routines or other hardware specific
features can be included.
Do only use the documented interfaces of the medium- and low-level library
parts.
Always remember, that the lower level parts will be dynamically loadable in
the future.
.sp 1
I think that the high-level library part will be the least part modified.
I also think such modifications make only sense in case of bug fixes and
functionality enhancements.
.H 2 "Modifying medium-level parts"
The medium-level library parts should be hardware-INdependent.
Though, you may include some special hardware-calls if you use conditional
compilation.
.FS
#if(?) ... #else ... #endif
.FE
This way may be chosen, if the performance may grew rapidly.
However, if you use this way, implement the hardware-dependent parts as calls
to low-level routines performing the job.
Also there must be always a routine implemented, which only uses the basic
low-level routines for reading and writing a pixel.
This guarantees portability.
.br
All #define's, which are common to almost every video-device should be done
in the medium-level header file (same as the high-level header file).
There should also all documented functions be prototyped.
Please use conditional compilation, if you use newer language features like
void or enum, so these constructs can be easily commented out.
Use different names for each new feature, because some compiler implement
part of this new features.
They should be used if possible.
Note that the word USE_VOID e. g. is currently in use by the library to
implement the void keyword.
.br
Whatever you do, keep in mind that the medium-level part should be as portable
and hardware-independent as possible.
But you should also remember, that the medium-level part must be recompiled 
for each low-level driver...
.H 2 "Modifying low-level parts"
You may modify everything except the few documented and guaranteed
function-interfaces.
But you should try to implement as much coding as possible in C, because
you may use parts of existing low-level drivers or others may use your
drivers to implement another video-device.
Also a port of existing drivers to another operating system may be simpler,
if the driver is written nearly complete in C.
.br
You should use low-level routines only if it's impossible to do the same with
medium-level routines (or to get a much better performance).
Please let your low level-routine communicate with the medium-level routine only
by using strictly defined interfaces.
.br
Do not introduce a low-level routine for common usage, if it can't be
implemented on almost every video-device!
.H 2 "Modifying header files"
Header files play a very important part in the library design - so do not
forget them.
Every new high- or medium-level routine or definition should be added to the
ONE AND ONLY medium-level header file "graphics.h".
Do not add a second or third high/medium-level Header file!
Put a device-id in every low-level header file (HERCGRAF is an example).
So the medium-level routine knows which is the current low-level driver and may
activate different coding.
It may also be of interest for the user-program to know which video-device is
active.
.H 1 "Development environment"
It may be interested for you to here something about my development
environment.
The library were implemented on 2 IBM-AT's.
One is equipped with a hercules-compatible monochrome graphics card, one with
the Quad-EGA and a monochrome display.
So there could be no development done for color-graphics, but I think the
EGA-routines will run using color-graphics, too.
One AT is equipped with 2.5Mb, one with 512Kb memory, both without 80287.
Both are running under PC_DOS 3.1 (German version), one under XENIX,
and one under Mikroport's Unix System V.
There were some resident programs loaded.
.br
The software environment consist of the sh and related utilities by Allen
Holub,
.FS
, which are really great!!!!
.FE
the Lattice, Datalight, Datalight optimizing and Borland's Turbo C compiler
and Microsoft MASM 4.0 and related products.
The files were edited using the Turbo Pascal editor.
This document was edited under UNIX, using vi, nroff and the mm macro
package.
My (new) own AT is the one equipped with 2.5Mb, no 80287 and the Quad-EGA.
I currently own the sh and utilities, the Datalight, Datalight optimizing
and Borland's Turbo~C compiler, the
Blaise runoff formatter and the standard IBM PC linker.
I also own the PC/vi editor, which is really great, and Mr. Holub's nr text
formatter, which is much better than the Runoff formatter (and much less
expensive!).
The system is also equipped with Hersey Mikro Consultings Fansi-Console,
which greatly speeds up consol writing, but unfortunately needs a lot of
memory (50 Kb from small 640 Kb DOS memory).
And I'm running Microport's UNIX System V-AT since one month.
.br
Now think about new features, which can be developed with this environment...
.H 1 "Future enhancements"
This chapter list the current and planed developments, which will be made
available in the future.
In addition to these points I want to get some feedback from you, resulting in
a better and better library.
This feedback may be in form of new ideas, algorithms and, of course, critic.
I really love to hear your comments, because I myself cover only a little span
of potential applications, so my library will always be restricted, until you
send my new ideas.
.SP 1
But enough of the feedback, here are the facts about future enhancements:
.DL
.LI
First of all, the high-level library part will be greatly enhanced.
I hope I can implement many useful high-level functions in the near future.
This will include a metafile system and related plotter drivers (have you
seen the new plotter entry in the heading page?).
.LI
text output
.br
will be advanced.
.br
The current text-output function only supports a limited functionality.
This functionality will be greatly enhanced. More style-modifiers and 
loadable character sets will be implemented.
Also a font-editor may come with the next release.
.LI
virtual graphics pages will be implemented
.br
These implementation of virtual pages will base on a hardware-independent
algorithm, so that multiple display pages will also be available on devices
which not directly support them.
In addition, because graphics need a lot of memory, which is not available
at some systems, the library will implement an algorithm, which is able to
use mass-storage for inactive pages (virtual memory management).
.LI
I try to port the library to the UNIX operating system
.br
I'm not sure, that these port will be successful in the end.
I'm not very similar with the UNIX operating system, nor with graphics
programming using UNIX and, of course, graphics terminals.
My first attempt to implement the library was not successful, but I learned
that an operating system like UNIX is very, very secure (so secure that
my I/O-instructions don't work).
.br
Don't misunderstand my: I really love the way UNIX goes, but I don't know
currently how to implement the library.
.br
But I also think that the port will be possible now, where I can use my own
Unix.
.sp 1
.in +10
.ll -10
.ft B
Help!
.br
If someone has some information about this job, please write me.
I would happily receive each piece of information.
.ft R
.in -10
.ll +10
.LE
.H 1 "Final observations"
At this point, thanks to all "heros", which read this whole document.
I know, my English is very bad, so I hope you excuse all the little 
(or should I better say big!?) errors in spelling and grammar.
.br
I hope my English will grew better in the future, so the documents will grew
better, too.
.sp 1
In addition, I want to thank all people which will contact my (I hope there
will be some). I also want to thank the
.ft B
C User's Group
.ft R
for distributing the library and this document.
.TC
.\ " VI : set wm=5 :
