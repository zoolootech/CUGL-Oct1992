

@closing(5-SEP-85)


This document outlines the usage of my version of the Bose-Nelson
sorting algorithm presented in the September 1985 issue of Dr.
Dobbs.  As far as I could tell, both programs that were presented
(in good faith I presume), were not functional as written. 
Rather than try to correct the bugs, I wrote my own version of
the recursive program.

The version of "BOSE.COM" that is contained in this library will
run on Z80 CP/M only.  You will have to recompile "BOSE.C" to
suit your machine and C compiler, if this is not the case.

The "BOSE.COM" program will generate a program containing the
swap pairs for a known, fixed set of elements.  To use the
program, type "BOSE <#> <filespec.ext>", where "#" is the number
of elements to be sorted and "filespec.ext" is any legal file
name that you wish.  I use ".c" as my extension.

A few notes about the efficientcy of both the speed of
generating the swap pairs and the object code size.  The
Bose-Nelson algorithm begins to take excessive amouts of time
when the number of elements gets above 200-300.  For this many
elements, there are about 12,000 ( right! 12K) swaps.  Thats
about 20k in swap code alone for my rather inefficient (remaining
nameless) C compiler.  However, the speed is quite excellent. 
For the example ("STEST.C") sort of 50 random integers, it puts
quicksort to shame.  Try it!

Anyway, enjoy the program.  I'm sure that you will be pleased to
have a working version of the algorithm.  Several improvements
are in order, but the first that comes to mind is to reduce that
number of swaps involved for large amounts of elements.  It was
mentioned in Dr. D's that this routine did not generate optimal
sorts.  They weren't kidding.

@closing(Mark D. Lougheed
6704 Sierra Drive S.E.
Lacey Wa
98503)
legal file
name that you wish.  I use ".c" as my extension.

A few notes about the ef