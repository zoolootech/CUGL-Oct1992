
FTGRAPH.EXE was created to go with my article in _Intelligent Instruments and
Computers_. ("Understanding and Using Fast Fourier Transforms", Thomas Clune,
_II&C_ v.7 n.3, May/June, 1989, p.103 ff). That article explains the concepts
that are behind the FTGRAPH.EXE package, and should be considered part of
the documentation for FTGRAPH's use. This file is intended as an aid to
installation and configuration of FTGRAPH, not as a "user manual" for people
unfamiliar with the FT and its uses.

FTGRAPH.EXE is a set of utilities for performing Fourier transforms and
inverse Fourier transforms.  The minimum requirements for program operation
are: an IBM PC, XT, AT, or clone, 256 K or more RAM, MS-DOS or PC-DOS, v.2.0
or later.  The program will use a numerical coprocessor (8087, 80287, or
80387) if it is present, but does not require it.  The program supports any of
the standard graphics screen adaptors (Hercules, CGA, EGA, and VGA graphics are
supported). Note Well: if you are using a Hercules Graphics adaptor, you must 
install MSHERC.COM for graphics support.  MSHERC is a Microsoft program, but 
is distributed here with Microsoft's permission.  Or, you can output HPGL files 
(HPGL, Hewlett-Packard Graphics Language, is the language used by HP-compatible
plotters.)  Various devices support HPGL.  For example, at the Institute we 
often send HPGL files to a Hewlett-Packard LaserJet, using a utility marketed 
by HP (LaserPlotter, from Insight Development Corp.).  This is an easy way to 
get high-quality graphics out of different devices.  NOTE THAT FTGRAPH DOES NOT
OUTPUT HPGL DATA TO A PLOTTER.  Rather, it creates a file of HPGL data that
can then be copied to the graphics output device using either a utility like
LaserPlotter or, if you are outputting to an HP-compatible plotter, by
using the DOS COPY command.  For example, if the HP plotter is attached
to COM1: and you have set the MODE command to be compatible with the plotter
settings (see DOS manual), just COPY yourfile.plt COM1 at the DOS prompt.

In order for FTGRAPH to work properly, you must be operating under ANSI.SYS.
This is a terminal control program that comes with DOS.  To install ANSI.SYS,
you must have a line DEVICE=ANSI.SYS in your CONFIG.SYS file, and the ANSI.SYS
program must be in the root directory of your boot disk.  See the DOS manual
for details.

The file FTGRAPH.CNF is a configuration file that must be located in the same
directory as FTGRAPH.EXE.  There are six arguments in the file.  They are:
1. The first number is a floating-point value that specifies the smallest
amplitude that the program will consider as significant when you use polar
coordinates (amplitude/phase) for your transform.  This value
is used to decide whether the phase argument is significant, or just noise.
An FFT will calculate phase even if there is no signal at the given frequency,
so this value is used to decide whether phase should be set to 0 because there
really isn't any signal there.  It does not apply to rectangular format data
transforms (real/imaginary).
2. The second number is an integer used as a flag to tell the program whether
you want to use the entire transform (both positive and negative frequencies)
or just the positive frequencies.  Often, if you are using real data, you
will limit your frequency display to positive values.  However, there are
times that positive/negative are desirable.  For example, if you are going to 
process data in the frequency domain and re-transform it back to the time 
domain, you must use both positive and negative frequencies or you will lose
half the data points during transformation. NOTE WELL: to lessen this problem, 
I transform data files for filtering as positive/negative, even if you select 
positive-only display. Thus, the re-transformed time domain data will contain 
the full number of values. 
3. The third value is an integer flag that specifies whether you want to use
rectangular coordinates (real/imaginary) or polar (amplitude/phase) coordinates
for your frequency domain data.
4. The fourth value is the units for filter rolloff.  2.0= dB/octave, 10.0=
dB/decade.
5. The fifth argument is the number of decimals (INTEGER ARGUMENT) that you
want to use in your floating-point printer dumps of data files (either [seconds,
magnitude] or [frequency, magnitude] files).  This number is for printer
display ONLY, and does not affect the precision of calculation in the program.
6. The last value is an integer flag that specifies whether you want to be
able to select from the menu using only keyboard inputs or both mouse
(Microsoft compatible only) and keyboard.  NOTE WELL: if you use the mouse,
the RIGHT-HAND button is used to enter your selection.  Moving the mouse
will move the highlight to the choice you want to make.  WARNING: If you set
this variable to 1 (enable mouse operation) and a valid mouse is not installed
on your computer, the program will hang, and may display a FLOATING POINT 
COPROCESSOR UNDERFLOW error message.  Always disable the mouse option in the
configuration file if you do not have a Microsoft(-like) mouse on your system.
Remember that you must have a line like DEVICE=MOUSE.SYS in your CONFIG.SYS
file in order to install the mouse device driver.

All these arguments are explained in the FTGRAPH.CNF file itself.  You may
edit the file with a common word processor to customize the package to your
taste.  WARNING: Do not capriciously move from positive to positive/negative,
or polar to rectangular coords.  The program does not keep a record of which
format was used with which data file.  The current settings ARE ASSUMED TO
BE THE CORRECT ONES for the data file under analysis.  If a file was collected
using positive only settings, and transformed under the positive/negative
format, for example, it will assume that the file contains both positive and
negative data.  Similarly, there is nothing in the file that identifies the
data as time-domain or frequency domain, etc.  Establish a format that
is right for your applications, and stick with it.  Since the right format
for display is often different from the right format for data processing, as
explained above, FTGRAPH allows you to change the default settings within
the program by selecting menu item B.  NOTE WELL: resetting defaults using
menu item B applies only as long as the program is running.  When you load
FTGRAPH, it reads its initial conditions from FTGRAPH.CNF, so the only way
to make changes in configuration permanent is by editing that file.

The data files that are read by this program must have the following format:
The files are plain ASCII text files,and the first line of the file contains
three numbers.  First is an integer specifying how many data points are in the
file.  The program expects a power of 2 size data set ONLY.  If the number of
points in the data set is not a power of 2, the program will automatically
zero-fill the working set (not the disk data) to the next highest power of two.
The second and third number are floating point numbers used to scale graphs if
they will be something other than full-scale. The second number is the minimum
y value of the graph scale, and the third number is the maximum y value for
the graph. Note that you can set multiple graphs to the same scale by making
these numbers the same in the various data sets. This lets you compare 
magnitudes across graphs. Also, if you wish to expand the scale of a graph,
this feature will let you clip the graph at min and max values of your choice.
Alternatively, you may set both the min and max values to the same value. This
serves as a flag to the program to autoscale the graph to fill the screen. As
a convention, I suggest using 0 for the min and max if you want to autoscale.
By the way, when FTGRAPH creates a data file the values that it uses for the
minimum and maximum are the actual minimum and maximum of the data set.
All numbers in the data file are separated by white space only (space, tab, or
carriage return).  That is, there are no commas or semicolons allowed.  After
the first three numbers comes the data set: floating point numbers separated
by white space.  If you are unclear on the format, see one of the sample
data sets included here (any .DAT file).  All .DAT files included here are
time-domain files.

The sample data sets are: A 16-cycles square wave data set
in SQUARE.DAT, a gaussian waveform (GAUSS.DAT), a sine and a cosine wave
data file TWOWAVES.DAT, a noisy Gaussian curve in NOISY_GS.DAT, included
for demonstrating correlation, and HANNING.DAT, for use in demonstrating
windowing with the multiplication option.  Notice that the multiplication
feature of the program lets you window data sets if you define your window as
a data file and multiply the window with the data set.  I remind you that
multiplying in one domain is the same as convolving in the other.  Thus, the
combination of being able to transform, inverse transform, and to multiply
allows you to perform convolutions. This is the basis for the filtering option.
The time-domain data is transformed to the frequency domain, the filter is
defined in the frequency domain and then multiplied by the transformed data,
then the filtered data is transformed back to the time domain.  If you like,
you can save the filter that you define for later use.  But remember that the
data file needs the same number of data points as the filter file, and the
sampling frequency in the data file must be the same as that used in defining
the filter. Also, the choice of positive/negative or just positive frequencies
will affect the filtering operation.  These bookkeeping points are managed for
you when you select the FILTER option.

The program internally does forward and inverse transforms in rectangular
format, performing polar-to-rectangular conversions as necessary before or
after an FFT.  The exception to this is that power spectra are left in power
spectrum format during conversion to correlation spectra. You should always use
the CORRELATION menu item for doing correlations instead of selecting INVERSE
TRANSFORM on a power spectrum, therefore.

Whenever one prepares a general-purpose package, there are various compromises
that must be made.  These include limiting the end-user's access to various
parameters so that the package can be used without the user having to know
as much about the program as the programmer does, but providing the user with
access to the parameters that (s)he will likely want to change, etc.  Some
of the most difficult choices that I have made have to do with balancing the
usual desires for how data will be displayed with the usual desires of how
the data will be processed.  In particular, positive/negative frequencies
have been a sticking point for me.  If you want to display only positive
frequencies, that does not mean that you want to process data as positive-
only, for example. I have made the filter function perform the FT of data
to be filtered as pos/neg, even if you have selected positive-only for your
ft displays.  Thus, when the filtered data is transformed back to the time
domain, it contains the same number of data points as the unfiltered data.
However, if you save a filter for viewing or inverse transforming to see
the ripple, etc, the filter is saved as either pos/neg or pos only, depending
on how you have selected your options.  My expectation is that these choices
will make the program behave according to naive expectations in the majority
of cases, but such choices are fraught with peril.  Let the user beware.
Similarly, when you choose to use both positive and negative frequencies in
your display, I assume that you want the transform ordered temporally.  That
is, I unscramble the transform to present a continuous x-axis format to the
data.  Thus, when I inverse transform data where the positive/negative
frequencies have been selected, I must reorder the data under the assumption
that I had previously unscrambled the data.  These kinds of assumptions and
decisions have a way of proving inappropriate for a surprisingly large
percentage of users.  I have therefore included all the source code that I
have used to construct the routines.  If you read over the source code, you
will soon see that there are many functions included in the library that
are not used in FTGRAPH.  These functions are designed to provide full
support for a text-based menuing system, Microsoft mouse control, full
HPGL plotter support, etc.  My hope is that, if the options that I have made 
accessable in FTGRAPH are not suited to you, the library of functions will be.

The programs used to create the FTGRAPH package are included in source as
.C files or the supporting .H files.  The program and the library were
compiled using Microsoft C v. 5.1, large memory model.  Microsoft-specific
features were used, so the programs should not be considered portable.
I compiled these modules with optimization disabled (/Od flag), and recommend
that you do the same at least with my code. The only other flag used was '/AL'. 
Large memory model should always be used.  You may modify the source as
you see fit, but I will not provide support for anything but the distributed
form of the program.  To link, use the following:
LINK/ST:6000 FTGRAPH,,,FTPLOT MOUSELIB MOUSE GRAPHICS

The package requires the Microsoft mouse driver library, MOUSE.LIB, in order
to compile (my version is dated 1-23-89  Make sure that your mouse.lib is recent
enough to contain cmousel()).  In case you do not have mouse.lib, you can still
compile and link the program using NONMOUSE.LIB instead of MOUSELIB.LIB.  The
link command would then be:
LINK/ST:6000 FTGRAPH,,,FTPLOT NONMOUSE GRAPHICS

All functions that call MOUSE.LIB are in the file MOUSELIB.C, and the source 
file replacing it on non-mouse systems is NONMOUSE.C.  All that NONMOUSE.C
does is stub the mouse calls to avoid unresolved externals at link time.
If you try to call a mouse function after linking NONMOUSE.LIB, the program
will display an error message.

Alternatively, if you have a mouse and MOUSE.SYS, you can use my MOUS_SYS.LIB 
instead of MOUSE.LIB.  The way to compile and link with this library is:
LINK/ST:6000 FTGRAPH,,,FTPLOT MOUSELIB MOUS_SYS GRAPHICS

This program finds the .CNF file it needs by reading the filespec in ARGV[0].
This argument normally contains the full filespec for the program, e.g.,
C:\FT_DIR\SUB_DIR\FTGRAPH.EXE.  The program then replaces the "EXE" with
"CNF."  The reason to do this is that the DOS PATH argument will find
executable files (.BAT, .COM, or .EXE) ONLY.  Thus, this program expects
the .CNF file to be in the same directory as the .EXE file, and by using
the ARGV[0] mechanism, it can operate under any configuration that the
program can operate under.  At least that is the theory.  Unfortunately,
versions of DOS before 3.0 and some unusual DOS clones do not support
passing the filespec of the program in ARGV[0].  Thus, I have provided a
second way of finding the .CNF file.  If the ARGV[0] approach fails, I will
check the default directory for the .CNF file.  If you get an "Unable to
find configuration file" error message, even though the .CNF file is in
the same directory as the .EXE file, you must copy the .CNF file to the
default directory you will be using when you run the program.  If you are
going to compile your own version of these routines, make sure that you read
the comments in FTGRAPH.C and FTGRAPH.H about how to handle this situation.

I  hope you enjoy the package.  I remind you that the program is copyright
(Copyright (c) 1989, Eye Research Institute, 20 Staniford St., Boston, MA
02114.  All rights reserved.)  You are free to use the program for non-
commercial purposes.  However, if you are trying to make a profit on it, we
at E.R.I. want to talk to you about getting our share.

If you need support getting the program set up, feel free to contact me at:
Tom Clune, (617) 723-6078, x545, or write me c/o E.R.I. at the address above.
	--tom clune
