rem MIRACL - IBM PC/MS-DOS Version 2.0
rem This batch files creates miracl.lib from its component parts
rem using the Aztec v3.4 (or greater) compiler, the Aztec
rem as assembler and lb librarian utility
rem Also included are the commands to create all the example programs
rem using the Aztec ln utility
rem
rem Read your compiler documentation for further information
rem 
rem Invoke as "azdoit". It is assumed that paths have been correctly set up to
rem the compiler, assembler, librarian and linker.
rem
rem Note - use with care. There is insufficient space for all of the files
rem created by this batch file on the distribution diskette alone.
rem
rem Provided mainly as a guide for creating a batch file tailored
rem specifically to your own configuration.
rem
rem On a diskette based system these commands should be executed manually,
rem changing diskettes as necessary
rem
rem Compile MIRACL modules
cc bncore.c 
cc bnarth0.c
cc bnarth1.c
cc bnarth2.c
cc bnsmall.c
cc bnround.c
cc bnio1.c 
cc bnio2.c 
cc bngcd.c 
cc bnxgcd.c
cc bnarth3.c
cc bnrand.c 
cc bnprime.c
cc bnflash.c 
cc bndouble.c
cc bnbuild.c
cc bnflsh1.c
cc bnpi.c
cc bnflsh2.c
cc bnflsh3.c
cc bnflsh4.c
rem
rem Assemble bnmuldv.c
as bnmuldv.c
rem Create library 'miracl.lib'
del miracl.lib
lb miracl.lib     bnflsh4.o bnflsh3.o bnflsh2.o bnflsh1.o bnpi.o
lb miracl.lib -a+ bnio2.o bnio1.o bngcd.o bnxgcd.o bnflash.o 
lb miracl.lib -a+ bndouble.o bnprime.o bnrand.o bnround.o bnbuild.o bnsmall.o
lb miracl.lib -a+ bnarth3.o bnarth2.o bnarth1.o bnarth0.o bncore.o bnmuldv.o
del bn*.o
rem
rem Compile and link example programs
cc brute.c
ln brute.o miracl.lib c.lib
del brute.o
cc mersenne.c
ln mersenne.o miracl.lib c.lib
del mersenne.o
cc rsakey.c
ln rsakey.o miracl.lib c.lib
del rsakey.o
cc encode.c 
ln encode.o miracl.lib c.lib
del encode.o
cc decode.c 
ln decode.o miracl.lib c.lib
del decode.o
cc okakey.c 
ln okakey.o miracl.lib c.lib
del okakey.o
cc enciph.c 
ln enciph.o miracl.lib c.lib
del enciph.o
cc deciph.c 
ln deciph.o miracl.lib c.lib
del deciph.o
cc pi.c 
ln pi.o miracl.lib c.lib m.lib
del pi.o
cc sample.c
ln sample.o miracl.lib c.lib m.lib
del sample.o
cc roots.c 
ln roots.o miracl.lib c.lib m.lib
del roots.o
cc hilbert.c 
ln hilbert.o miracl.lib c.lib
del hilbert.o
cc pollard.c
ln pollard.o miracl.lib c.lib
del pollard.o
cc williams.c
ln willimas.o miracl.lib c.lib
del williams.o
cc lenstra.c
ln lenstra.o miracl.lib c.lib
del lenstra.o
cc qsieve.c
ln qsieve.o miracl.lib c.lib m.lib
del qsieve.o
cc brent.c 
ln brent.o miracl.lib c.lib
del brent.o
cc fact.c 
ln fact.o miracl.lib c.lib
del fact.o
cc palin.c 
ln palin.o miracl.lib c.lib
del palin.o
cc hail.c
ln hail.o miracl.lib c.lib
del hail.o
 