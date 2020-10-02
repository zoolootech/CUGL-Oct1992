@center[@b(Using MicroSPELL 1.0)]
@center(Author:  Brian Straight)


@u<Introduction>

MicroSPELL 1.0 is a compact, efficient, fast spelling checker for use
both as a stand alone program, and with MicroEMACS v3.9 and above on
many different computers.  This document briefly describes how to use
MicroSPELL. 

@b<Command Syntax>

spell {<options>} <file> {<file>.........<file>}

The currently available options are:
@begin(verbatim)

-d		debugging mode
-e		use MicroEMACS to scan errors
-u <fname>	use a user word list 
-w		output a word list instead of a location list
@end(verbatim)

@b<Example>

	Let's assume that you want to spell check a text file called
text.txt which is on the @b{B} drive of your machine.  To invoke spell
type:

@b[spell -e b:text.txt]

	Be patient.  It will take SPELL a short while to complete its
setup procedure.  If would like to know more about what SPELL is doing
at this point, you can set the debugging switch (@b[-d]} when you first
invoke the program.  SPELL will then give you detailed information about
what it is doing.

	When SPELL has finished checking your file against its internal
dictionary, it will give you a brief report of how many suspect words
it found, and then, since we used the @b(-e) switch, it will invoke
MicroEMACS in order to scan through the suspect words.  Notice that the
screen is very like the standard MicroEMACS screen, except that the
function key window (if it is normally there) is now replaced by the
SPELL functions and status window. 

	There are a number of elements in this function window that you
need to be aware of.  On the top line, the "File" field lists which file
the scan is currently on.  This is useful when many different files are
spell checked together in one run (many file names can be put on the
command line at once).  The first number is the current file, and the
second number is the total number of files.

	The second field on the top line is the "Word" filed. This
describes the current suspect word and the total number of suspect words
in the current file.  This will give you an idea of how far throughout the
file you have progressed during the scan.

	One at a time, each suspect word in the file is presented to you
for you to decide on its disposition.  The suspect word is displayed,
and the question "Option:" appears in the Dialog Window.  The options
you have include:

@begin(verbatim)
(S)kip		Skip this word, leaving it alone, and go on to the next.

(I)gnore	Ignore this word and all other occurences of this word
		during this scan.

(C)hange	Change this word.  You will be asked to type a
		replacement word.  This word will replace the original
		and MicroEMACS will remember the replacement.

(D)efault	If you have already replaced an identical occurance
   replace	of this suspect word, MicroEMACS will present the last
		replacement as a "Default Replacement".  Picking this
		option automatically replaces the suspect word with
		this default word.

(G)lobal	When you first encounter a suspect word, if you wish
    change	it to be replaced in the entire document, use this
		option. You will be prompted for the replacement word.

(A)dd word	To a user dictionary.  This word will then be remembered
		for the remander of the current scan, and also for all
		future scans when that user dictionary is invoked.

(U)pper case	Add a word to a user dictionary in its current case.
		This is good for names, proper nouns and acronyms.

(Q)uit		Finish the current scan, saving the file with the
		already completed changes.
@end(verbatim)

@i(Note: Scanning document files will be availible on the IBM 370 series
under the CMS operating system in September 1987 using an XEDIT script). 

