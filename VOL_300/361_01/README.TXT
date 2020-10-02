
   In the process of tidy-up as	this three year	project	ends (itself a 
spin-off of a decade long effort), I have gathered together a number of	
routines I have	found useful & set up a	SMALL and LARGE	model Turbo-C (v2.0) 
library	for local distribution.

   Since there is room left on a 360K Floppy, I	also tossed in a few Tools, 
among them Valve, Chopper and three cypher programs.

   I include a TIS (Text Indexed Sequential) keyed "Manual" for	the library 
and Vu to access and search it.	 While C-coding, I freqwently refer to this 
file using HKM,	a TSR built using Al Steven's "popup.c"	and "resident.c" TSR 
routines from "Turbo-C Memory-Resident Utilities, Screen I/O and Programming 
Techniques" (MIS Press,	1987) and modules lifted from Vu.

   I also include a Quick & Dirty solution to the puny DOS pipe	problem:

=============================[ PX Pipes	]====================================

   Pipe: A One-Way Link	connceting the Output of One Program to	the Input of 
	 Another.  Symbol: "|".

 Filter: A program intended to be Piped	on Both	Sides.	Example: SORT
 
   Tool: A Program (usually small) which does One Thing	Well.

-----------------------------------------------------------------------------

   Currently, there are	(at least) three competing philosphies about PC	Use: 

   1. Launch an	Application and	Do All Work there.  Example: dBASE.
   2. Overload a Graphics User Interface (GUI) and operate in a	Mouse/Icon 
      Environment.  Example: MS-Windows.
   3. Perform Processes	on Text	Files from the Command Line Interface (CLI).
      Example: Unix.
   
   This	is an exploration of the last (Unix-style) philosophy as implemented 
on the PC.  Although hobbled by	lack of	Virtual	Memory,	a Tiny (by Unix	
standars) RAM and inadequate disk space, DOS can do a surprisingly good	job 
when fitted w/ the right Tools,	some "Magic" and a knowledgeable User.

   That's right.  Tools	demand intellegent Users, which	is probably why	Unix 
has been so slow to go "Mainstream".  The PC is	a difficult machine in many 
aspects, BATch Files being one of them and, as you might expect, BATch Files 
are essentual to effective Tool	use.

   Text	Files are the Raw materials and	End product of Tool Use.  Many tasks 
which naturally	occur in the course of our daily work involve the 
manipulation, transformation or	analysis of Text Files.	 Just are as a Wood 
Wright cuts, shapes and	polishes Wood, DOS can cut, shape and polish Text.

   The Unix Philosophy is to create and	use simple Tools which,	when linked 
together, can do complex tasks quickly.	 The "Link" is the Command Line	Pipe, 
"|".  As in Early Unix,	DOS Pipes are actual Files which are created, used 
and erased as the Command Line carries out your	wishes.	 (A "Crash" might
leave them in the Root Directory, names	consisting of strictly numbers and 
often completely empty).

   For many situations,	you need only begin the	Command	Line by	TYPEing	out 
the Source Text	and "Squirting"	the finished product into a File like so:

C:>TYPE	source.txt | FIND "something" |	SORT > target.txt
   
   That	works well enough as long as your processing is	simple and your	Tool 
names (and Arguements and Option Switches) are compact enough to fit on	DOS's
127 character Command Line.  When they do not, you can use "Temporary" Files 
to pass	Text from Line to Line.

   From	that simple process (passing Text from Line to Line) came a Family of 
"Pipe Extenders" that resolve many of the problems Tool	Users encounter	on 
DOS.  The first	(most basic) pipe extender is "PX".  It	allows the above 
example	to be split like so:

C:>TYPE	source.txt | FIND "something" |	PX

C:>PX |	SORT > target.txt

   PX creates a	File in	Root, accepts Text from	an Inflow Pipe and later 
TYPEs it into an Outflow Pipe.	Being only two letters,	PX takes up very 
little of that precious	127 character Command Line, a Plus.

   The above works on the Command Line (PX can tell where the Pipes are).  
Inside BATch Files, the	"Sniffer" doesn't work,	so you have to use Flow	
Control	Flags, like this:

TYPE source.txt	| FIND "something" | PX	/W
PX /R |	SORT > target.txt

---------------------------------[ Named Pipes ]----------------------------

   PX uses an "Illegal"	file name containing a Space.  Command.Com and 
everything in DOS avoids embedded Spaces, but CP/M allowed them, so the	DOS 
File System (to	maintain backwards compatibility) allows them, too.  PX	uses 
such names to avoid any	possible conflict with DOS or existing application 
programs.

   When	it was decided to allow	the User to specify file names for PX to use, 
the real names could not be passed thru	Command.Com.  It was decided to	let 
the User specify the last 4 characters of the name (providing several million 
choices), but signify that this	is a Pipe Name by using	"$" in place of	the 
"Illegal" part of the name.

   By this convention, PX defaults to a	Pipe Named "$OUT":

C:>TYPE	source.txt | FIND "something" |	PX $OUT

C:>PX $OUT | SORT > target.txt

   Tools which allow $Pipes will say so	in their "Barefoot" use	message.

-----------------------------[ Piping BATch Files ]--------------------------

   By now you may have guessed that PX can "Jump the Fence" into and out of 
.BATch Files.  Flow into a PX Pipe outside the BATch File, read	it inside, 
refill it with the results and exit.  Later on,	read the PX Pipe.  Works 
every time.

   Unfortunately, you can't Pipefit a BATch File into a	Pipeline using the 
above method.  For one thing, to call a	BATch File within a Pipeline, you 
have to	spawn a	Secondary Shell	using "Command /C ":

 .. | PX /W | Command.com /C mybat | PX	/R | ...

   Which is equivalent to:
   
 .. | PX myfile	| PX | ...   (PX spawns	the Secondary Shell)
 
   All the restrictions	of the "/C" option apply except	one: PX	will convert 
single quotes to double	quotes as it passes "myfile" and its arguements	to 
the Secondary Shell.  (Just enclose "myfile 'arg1' 'arg2' ..." in double 
quotes).  Notice also that PX didn't need the "/W" & "/R" flags	(even in a 
BATch File).

   Sometimes you want a	BATch File to print out	to the CRT when	used from the
Command	Line, but use a	$Pipe when called in a Pipeline	by PX:

DIR | FIND "-" | FIND /V "<" | CUT 12 |	TRIM | TR /S " " | TR "	" "." |	PXO

   The above Pipeline was taken	from Ls.BAT, a "DOS Script" (PX	can Call it 
inside other BATch Files). 

-----------------------------[ Pipefitting ]---------------------------------
   
   Once	you have the ability to	build Scripts, many of the techniques used by 
Unix Shell Programmers can be done in DOS.  One	serious	problem	remains: How 
do you pass the	contents of a Pipe to a	Command?

   For example,	Ls gives you a list of File Names, how can you present that 
to the User for	him to select some (several or one) to copy to a Floppy?

   PX Ls | PX |	MSO /R | RSX "DO COPY $1 A:"

   The first part uses "Ls" as "myfile"	as discussed above.  The Piped 
outflow	of that	(a list	of file	names) is presented to the User	who can	
select which ones he wants copied.  (See BatMan	MSO)

   "RSX" means "Read-Single-Execute", a	variation of PX	that repeats the 
"command" over and over	until the Pipe runs dry.  Each line is "Parsed"	into 
"Words"	ranging	from $1	thru $8	($9 is the remainder, $0 is the	whole line).  
Before execution, $1 (in this case) is replaced	by the first word on the line 
read in.

   Notice that "command" is quoted (there is an	embedded space).  Since	we 
want to	see the	Command	as it is being executed, "DO" is used (See MAN DO).   

   For those situations	where a	Select List isn't appropriate, you can Pop an 
"Ask" Box onto the screen using	APO:

   APO /I *.*  "Enter Files" | rsx dir $0
   
   Try it.  Notice that	if you just hit	<Enter>	that only one DIR cycle	
occurs.

-------------------------[ Final Thoughts ]---------------------------------

   If you can reduce your User Interactions to a Single	Keystroke, Keys.h (or 
BATchman) can handle you needs via ERRORLEVELs.	 If you	can't, use APO or MSO 
to feed	text down a Pipeline.

=============================================================================
  
>APO.  A "Boxed" Ask w/	Prompting & Text Capture.  Squirts to Stdout.

Usage:
	APO [/option] text [title]

Options:
       /I ---> Ignore Case.  (Everything is UPPERS).
       /N ---> Numbers Only.

   APO may be Piped w/o	Disrupting User	Input.

  Try It!  APO must be Piped for OutFlow (or It	isn't very Useful).

   APO is a Public Domain Program Written in C.

==========================================================================

>CAT.  conCATinate Files/Pipes ala Unix	cat w/ Line Number & Queue Options.

Usage:
       CAT [/Option or file list] ---> Stream Text to Outflow Pipe.

/Options:
    /Q ---> Stack Stdin	Lines into 75 CHR$ max lines.
    /N ---> Add	Line Numbers to	Each Stdin Line.
    /T ---> Add	TEKTEST	Line Numbers to	Each Stdin Line.
    /U ---> Remove Line	Numbers	from Each Stdin	Line.

File List:
    File Names (Wild Cards OK),	$pipe or "-" for Stdin.

----

   CAT will "glue together" a list of files/pipes into one continuous Text
Stream.	 Any EOF markers (^Z) are filtered out.

   CAT supports	the "PX" Named Pipe conventions.  Use $IN to read the
default	PX pipe.

----

   CAT does not	Rub-Out	Named Pipes after use.	You may	re-read	them by
calling	CAT again.

   You can Rub-Out a Named Pipe	named "JEEP" like this:

   PX $JEEP | SORT > NUL

   PX must have	an Outflow Pipe	or flow	into a File.  NUL isn't	enough.

----

   CAT operating under the "/Options" is strictly a Filter.  No	Files,
please.

-----

   Note	to Unix	retreads: Use VIS for "cat -v" applications.

   CAT is a Public Domain program written in C.

==========================================================================

>CHOPPER.  Hack	up a Text Stream into Printable	Lines.

Usage:
       CHOPPER [[drive:][path]filename]--> Hack	Up Text	into Lines.

   $Pipes are Legal.

   If the Stream is Page-sized,	CHOPPER	is transparent.	 Otherwise a "Wrap"
action occurs w/ "" markers at	the inserted line break.

   "Choppered" Text can	be "DeWrapped" by piping thru: | SAR "$021?$021" |

   CHOPPER is a	Public Domain program written in C.
 
==========================================================================

>CYPHERS.  A Toolbox of	Useful Encryption Techniques:

>ENIGMA.  Louis	XIV's "Great Cypher".
>GARBLE.  A Text Stream	Scrambler used by Roman	Centurians.
>FOG.     An 80% "Snow Factor" of Random "Noise".

   Cyphers have	been used by Governments and Armed Forces as far back as
Roman times.  Today's secure Cyphers such as DES are algoritmic	based and
(unless	you are	a real bit-head) about as interesting as oatmeal.
Yesterday's Cyphers, the machines that (mechanically) implemented them and
the techniques to analyze them spun off	the field of Information Theory.
Because	they are well understood and documented, yesterday's Cyphers strongly
influence today's hardware/software/system designs.

   Three Cypher	Tools have been	created	implementing three different schemes
for "Covering" information.  All are written in	ANSI C and will	port directly
to DOS and Posix platforms.  All generate seven	bit ASCII code suitable	for
telecommunications via Sytek or	modem.

   These Tools ARE NOT suitable	for classified information (use	DES or other
certified method), but will resist attacks (even by experts) long enough to
discourage casual Snoops.  In effect, these are	"Honest	Man Locks",
sufficient to remove the temptation to browse or tamper.

   The first is	a file scrambler called	"GARBLE".  It is based on the "Ribbon
Trick" used by Roman Centurians	to encode/decode field orders.	Each Field
Officer	was issued a Staff (appropriately adorned) as a	Symbol of Rank.
Actually, it was a Cypher tool.	 A length of ribbon was	wrapped	around the
Staff.	The message was	written	along the Staff's axis and the ribbon
unwound, rolled	up and "Mailed".  Without a cylinder of	correct	size, the
ribbon's jamble	of letters was unreadable.  GABLE text resembles a binary
file printout, which means a casual snoop would	skip it.

   The second Tool is a	Substitution Cypher based on Bazeries Cylinders, the
defacto	standard cryptographic method through WW II.  Invented by Antoine
Rossignol, court cryptanalyst to Louis XIV, the	secret to the "Great Cypher"
died with him in 1715.	After a	century	of attack, Cmdr	Bazeries (of the
French Army) cracked the code and invented a Cypher machine based on it.
The German Enigma machine was an electro-mechanical version of that machine.

   Project Ultra and a special purpose electronic computer (Colossis) so
thoroughly cracked Enigma that,	by D-Day, Allied Commanders often had German
field orders before the	addressee.  So important was Project Ultra that
all trace of it	remained classified until 1974.

   The classical Diplomatic Cylinder Cypher used only letters printed on the 
periphery of 20	disks, each in a different scrambled order.  The "Key" 
specified what order these disks were to be stacked.  ENIGMA faithfully	
reproduces the action of these classic machines, except	that more disks	are 
used (40) and more characters are used (79) so that the	entire 255 character
Ascii/IBM code set can be handled.

   Using the /N	(neat) option, ENIGMA produces 5 letter	groups of
printable text,	twelve groups/line.  This allows visual	inspection during
telecommunications without exposing the	covered	text.  ENIGMA is well suited 
as a "Mailer", an envelope in which a file can be placed during	"Mailing",
especially if it must be "Parked" on a public system for later pickup.

   The third Cypher Tool, FOG, is based	on a trick used	by the KGB in the US
during the early 1960's.  Embarrassingly simple	(the original decoder was 6 
lines of BASIC code), the CIA never did	crack it, learning its secret only 
after the KGB's	US Station Chief defected.

   FOG obscures	information by inserting an 80%	dose of	"Noise".  At the cost
of a 5:1 bulk expansion, FOG allows secure transmission	of small items without
need of	Keys.  Passwords are one of the	items FOG was intended to protect.
Like ENIGMA, FOG can generate printable	text in	5 letter groups.

==========================================================================

>HKM.  Hot-Key Manual.	A TIS Compliant	Utility	w/ TSR Power.

Usage:
       HKM [options] path/manual.TXT  --> VU a Text File or TIS	Manual.

Options:
       /C0xhhhh	-> Specify Colors.
       /I -------> Install as a	TSR.  <Hot Key is ALT-Comma>.
       /K -------> Jump	to Item	Select List First.
       /R -------> Remove. (Try	to "UnInstall").
       /S -------> CGA Snow Fence.
     none -------> Pop Immediately & Run as a Normal Program.

   Item	Select Listings	are Created/Updated by TIS.BAT

-------

   The F1 Help Key produces this Display:

      <ESC> ---------> Exit This Program: Hot-Key Manuals.
    
      <Enter> -------> Pop Item	Select List.  <If Any>
       F2 -----------> Find a Text Phrase.
       F3 -----------> Find a Text Phrase.  <Ignore Case>
       F4 -----------> Repeat Search.
    
      <Home> / <Page Up>   / <Up Arrow>	----> Roll Upwards.
      <End>  / <Page Down> / <Down Arrow> --> Roll Downward.
    
      <Left Arrow> --> Shift Text Left	10 Spaces.
      <Right Arrow> -> Shift Text Right	10 Spaces.
    
      F5/F6:F7/F8 ---> Alter Colors for	This Session.
       F9 -----------> Switch to a Different Text File.
       F10 ----------> Report Current Color Settings.
    
      Any Other	Key -> Roll Down 11 Lines (Half	a Screen).

-------

   HKM retains the 4-Way scroll	and most other features	of VU, substituting 
a Select List capability for VU's WildCards and	Pipes.

   HKM can operate as a	StandAlone Manual Browser or Terminate and Stay	
Resident (TSR) w/ ALT-COMMA as its "Hot	Key".  When Resident, HKM will 
remember your place & Jump back	there on Wake Up.

   HKM is a Public Domain Program Written in C.

==========================================================================

>KEYS.  Report Keyboard	Hits w/	StdEK.h, Keys.h	& ScanCode.h Lookup.

Usage: Keys

Key Stroke Test.  Hit [Esc] to Exit.  Hit Any Key.

	Ascii	ScanCode	KeyMask		Keys.h		Symbol
	32	57		160		32		" "
	97	30		160		97		"a"
	65	30		162		65		"A"
	1	30		164		1		SOH
	0	30		168		158		ALT_A
	0	126		168		254		ALT_7
	0	127		168		255		ALT_8
	0	128		168		128		ALT_9
	0	129		168		129		ALT_0
	0	14		168		142	NewKey	ALT_BS 
	0	28		168		156	NewKey	ALT_RTN 
	0	133		160		133	NewKey	F11 
	0	78		168		206	NewKey	ALT_PLUS	
	0	166		168		166	NewKey	ALT_ENTER 
	0	1		168		129	NewKey	ALT_ESC
	27	1		160		27		ESC

   Above is a sample of	what Keys.com produces.	 Notice	that Keys.h, 
which uses the convention of ORing non-Ascii ScanCodes w/ 128 "wraps" at
ALT_8, going from 255 to 128 for ALT_9.

   Usually this	fold-back causes no problem.  However, 101-Key Keyboards, an
option on GSA Z-248s (which must use NewKeys.Com) and all GSA UNISYS 386s
can generate the items marked "NewKey",	some of	which will collide w/ other 
Keys.h style ScanCodes,	e.g., ALT_9 and	ALT_ESC.

   ScanCode.h is a straight ScanCode listing using the same symbol names as 
Keys.h.	 All symbols there are unique unsigned bytes.

   For Zenith Users, NewKeys.Com provides access to the	F11/F12	keys and to 
some variations	on other keys, such as ALT_BS and ALT_RTN, that	IBM left out 
of the original	PC's BIOS.  It also provides access to the Keypad "Grey" Keys 
such as	ALT_PLUS and ALT_ENTER

   Keys	is a Public Domain program written in C.

==========================================================================

>MSO.  Display a (Multi-Column)	List & Output User Selected Item(s).

Usage:
       MSO [options] [file] ---> Present a List	& Output Selection(s).

  Note:	$Pipes are Legal in "file" Name.

Options:
	 /Cn ---> Specify # of Columns Displayed [n = 1-4;D:4]
	 /N ----> Prefix Selections w/ Item Line Numbers.
	 /R ----> Multiple Selections Permitted.

   MSO may be Piped In/Out w/o Disrupting List Display/Selection.

   MSO is a Public Domain Program written in C.

==========================================================================

>PX.  A	Pipe Extensions	& Named	Pipes for DOS.

Usage:
       PX [options] [Command [Arguements] or $Pipe]  --> Extend	DOS Piping.

Option Flags are Required in .BAT Files:
	 /A ---> Append	to Specified Pipe.
	 /R ---> Read From Specified Pipe.  [Force OutFlow]
	 /W ---> Write to Specified Pipe.

Pipes:
    1. PX "Pipe	Extends" DOS stdin/stdout Pipes	using ordinary Files w/
       Unusual Names.  These Names can not be Generated	by DOS.
    2. A "Named" Pipe name is 1-4 Alphanumeric Characters Preceeded by '$'.

Action:
    1. If InFlow Redirection, Create the Specified $Pipe ($IN is Default).
    2. If a Command is Passed in, Shells Out & Execute it.
    3. If InFlow Redirection, PX Quits Here.
    4. If Redirected to	a File/DOS Pipe, Print Contents	of Specified $Pipe.
	 (Default is $OUT if found, otherwise $IN).
    5. PX erases $Pipe after reading.

----

   PX "Pipes" are just ordinary	files (as the original Unix Pipes were)	with
unusual	names.	Each PX	Pipe Name begins with "$" (for you) and	"\STD "	(for
DOS).

   Notice the Space.  While DOS	can handle file	names with embedded Spaces,
COMMAND.COM (the command processor "Shell") can't.  In other words, these
"Illegal" names	will never be generated	or used	by "normal" DOS.

----

   PX can be used to "Extend" DOS stdin/stdout Pipes into and out of BATch
Files.	There are a few	"Gotcha!'s" that DOS throws in:

   1. BATch Files can not load/unload TSRs except at the lowest	(Bootup)
level.	You can	not call these files with PX.

   2. The stdout Stream	is "Junked" up by COMMAND.COM's	mindless echos and
prompts.  See PXO Below	for a Solution.

   3. BATch Files will not OutFlow Pipe	unless called by PX.  DOS squirts
them directly to the CRT unless	the Shell is a "Secondary" Shell (has the
"/C" Option).

   4. Since DOS	doesn't	support	"Quoting" as Unix does,	PX converts BANG!
character into |Pipes and 'Single' Quotes into "Doubles" as it passes your
"Command Line" to the Secondary	Shell for execution.

   5. From the Command Line, PX	will figure out	what you want by how you Pipe
it.  Inside .BAT Files,	the Options Flags are Required.	 (Use /A anytime).

   6. Redirection of PX	to a "Character	Device"	like LPT: or NUL: can Fool PX
unless the Option Flags	are Used.  (PX > NUL won't work, PX /R > NUL will).

   7. PX combs NULLs out of its	byte stream.  NULLs are	<0> after VIS.

   PX is a Public Domain program written in C.

==========================================================================

>PXO.  A "Pipe Cap" w/ the Option to Redirect into PX Pipes.

Usage:
    | PXO [$pipe] ---> BATch File PX/CRT Pipe Cap.

      If the Script is Piped out of PX,	PXO Squirts Text into "\STD OUT" or
      into Specified Named Pipe.  (Name	= 1-4 Alpha-numeric Characters.)

----

   PXO is used in .BAT files to	bypass the "Chatty" garbage DOS	throws into
the Text Stream.

Examples:

   LS ---> Short List the Current DIR to stdout.
   PX LS | PX |	...  --> Dump DOS "Chatty Prompts" in the Bit Buck.

   PXO is a Public Domain program written in C.

==========================================================================

>RSX.  Read a Single Line & Execute a Command (w/ Passed Parameters).

Usage:
       RSX [/Fc] Command [Arguements] --> Do Command w/	Parameter Passing.

Options:
	 /Fc ---> Use 'c' as the Field Delimited vice Spaces.
		  The $### format for Decimal ASCII is Legal.

Action:
    1. RSX reads a Single Line of Text from Stdin.
    2. Using the Field Delimiter, Slots	$1-$9 are stuffed.  ($0	is the
       whole Line.)
    3. RSX Spawns a Sub-Shell, passing to it a "Command	Line" made up
       from Passed Parameters w/ these modifications:
	  A. Contents of Field Slots get substitued for	$Field Tags.
	  B. Bangs (!) and Dits	(') become (|) & ("), respecitively.
	  C. $$	becomes	$.
	  D. DOS WildCards are Passed Unchanged.
    4. RSX Repeats the Above for Until Stdin Runs Dry.

----

   RSX uses Stdin as its "to-Do" List, so Piping into the "Command Line"
requires some thought.

   For example,	Using PX works only for	the First Pass (PX erases $IN).

   If you Use PX into a	Named Pipe (say	$THIS) and begin the "Command Line"
w/ "CAT	$THIS !	"  (Remember ! ==> |), each Pass will have access to the
contents of that Pipe.
 

----

   The "/Fc" option allows RSX to operate on "Fields" instead of "Words".

   You can run a simple	Data Base like this:

   (one	line of	data in	file DB.DAT):

   Author: Jack	Ekwall:	Age: 45: DOB: 3	July 43: Code::6072: Phone: 1854

   TYPE	DB.DAT | RSX /F: ECHO Attn: $2 Mail Stop $8.

   Prints out:

   Attn:  Jack Ekwall Mail Stop	6072.

----

   Notice that imbedded	spaces are preserved.

   Notice that repeated	Dlimiters are ignored ("::" is read as ":").

   You are limited to 9	fields.	 ($0 is	the whole line).

----

   RSX is a Public Domain program written in C.

==========================================================================

>SAR.  Search & Replace in a Text Stream ala RAM-Page SAR.

Usage:
      SAR "This" "That" ---> Find <This> & Substitute <That>.
      SAR "This" ----------> Find <This> & Remove It.

Wild Cards:
   ? --> Place Holder in <This>.  Any Character Matches.
   ? Cannot Be First Character In <This>.
   ? --> Place Holder in <That>.  Original Character Remains Unchanged.
   $### --> An ASCII Character w/ Decimal Value <3 Digits>.
   $$ --> A Single Dollar Sign.

SAR is a Filter.

----

   SAR is a straight "This" = "That" replacement.

   "That" is allowed to contain "This" as a Sub-String.

----

   Unlike the Unix "sed" and "awk" Tools, SAR is not Line-Oriented.  In other
words, a "New Line" character is just another ASCII Byte.  SAR also allows
WildCards in the "That" String, something very useful but rarely found.

   On the down side, SAR's WildCards are kind of "Whimpy" compared to Unix
Regular Expressions.  SAR is more of a multi-character "tr" than a "sed".

   SAR is a Public Domain "C" Language.

==========================================================================

>TEE.  Squirt Stdin/out	Text into a File or $Pipe.

Usage:
       TEE file_name ---> Squirt Stdin/out Text	into a File.

Example: ... | TEE CON | ...  lets you (the CRT) Peek Inside a Pipeline.

  TEE a	Public Domain program written in C.

==========================================================================

>VALVE.  A Text	Stream Flow Control Filter.

Usage:
       Valve [options] [filename] --> Control Text Flow.

Options:
	  /An ---> Trip	on the "n"th Trigger Character.	[D:1]
	  /B ----> Block Text Until Triggered.
	  /Cc ---> Trigger on 'c' vice NL.  'c'	is any Printable.
		   or $$ or $### (3 Decimal Digits) for	any ASCII.
	  /F ----> FIFO	for Lines.  Pops Top Line Each Call.
	  /Hn ---> Pass	Head (Top) n Lines.  [D:10]
	  /I ----> Initial Character Trips.  (Ignore Everything	Else).
		   /C option Required.
	  /J ----> Jog.	 (Trip Twice & Hold Initial State).
	  /L-----> LIFO	for Lines.  Pops Last Line Each	Call.
	  /Tn ---> Pass	Tail (Bottom) n	Lines.	[D:10]
	  /V ----> Verbose.  Echo Text to CRT Until Triggered.

   Valve defaults to a Filter.	$Pipes are Legal in filename.

   After Loading, Repeated /F or /L Calls Pop Lines until Stack/Queue
is Empty.  Text	is Stored in the Named Pipe, $VALV.

----

   Used	as a Filter, Valve does	just what the name implies, passes or blocks
the Text Stream.  Default is to	pass one line of Text and then terminate.

   Using the /B	option,	Valve will block (skip)	the first line of Text and
pass everything	else.

   VALVE /H is functionally equivalent to "head" in Unix.

   VALVE /H5 is	the Same as VALVE /A5.

   VALVE /T is functionally equivalent to "tail" in Unix.

----

   The /L and /F options spill blocked text into a Buffer File instead
of the "Bit Bucket".  Calling VALVE without an Inflow Pipe Pops	a Line Out of
this holding area.  Try	This:

   DIR | VALVE /BA5 | VALVE /F > NUL strips the	DIR header and Stashes
the Listing.

   VALVE /L > NUL Pops off the last ("bytes remaining")	line into "Ye Ole Bit
Bucket".

   VALVE /F "Pops" off the DIR listing,	one line at a time until the Buffer
File is	empty (returns an EOF).

   VALVE /L /A2	pulls two lines	off the	End of the Listing.

   The Buffer File is a	Named Pipe, $VALV, which may be	"Highjacked" and used
as an ordinary Named Pipe.

----

   The /Cc option changes the "Trigger"	Character from NL to 'c', some
spcified character.  These "Gating" Flags pass thru unaltered EXCEPT for the
one causing the	"Trigger" Action.  That	one gets gobbled.

   This	differs	from NL	"Gateing Flags"	(the Default), which are passed	or
blocked	as needed to assure Text Lines stay properly delimited.

----

   The /J (Jog)	and /I (Ignore)	Options	allow Valve to Split Out Enties	from
a DataBase File	like \HELP\MANUAL.HLP (the DOS Manual).	 Try This:

   MSO /N \HELP\MANUAL.LST | RSX VALVE /BA$1 /JI /C$$62	\HELP\MANUAL.HLP

   Notice that Options can be in Any Order & can be Combined ala Unix Flags.

----

   The /V option is useful during DeBugging.

    VALVE is a Public Domain program written in	C.

==========================================================================

>VU.  A	"README" style file browser ala	Unix "more" w/ PX Pipes.

Usage:
       VU [options][drive:][path]filename [, ...] --> View Text	Files.
     | VU [options] ---> View a	Text Stream ala	MORE.

Options:
       /A -------------> Query for Initial Search.
       /C0xhhhhhh -----> Specify Colors.   [D:/C0x0F1F0E]
       /K -------------> Pop w/	Select List.
       /F \"FindMe\" ----> Jump	to Text	Match.
       /R -------------> Honor RAM-Page	Tables.
       /S -------------> CGA Snow Protection.

  Named	Pipes are Legal.  Pipes	are NOT	Errased	by Viewing.

----

   VU is "MORE"	w/ 4-Way Scrolling via Arrow Keys, Searches w/ Repeat &
Ignore-Case Options, and PopUp Help on F1.

   VU can be Fully Piped w/o Lockups, will Display a List of Files or the 
InFlow from Stdin.

   The /C option accepts a Hexadecimal Integer such as 0x1F301E, where "1F" 
sets the Text to "Bright White on Blue", "30" sets the Bars to "Black on 
Auqa" and "1E" shows Underlined	Text in	Yellow on Blue.	 
(See DOS Colors	above for details).

   The User can	alter the Colors for the current session via F10.  If a	
combination is pleasing, F10 will also report the equivalent /C	option.

   VU is the Display Engine for	TIS.BAT, which is the "Business	End" of	MAN, 
HELP and TERMS.

   VU is a Public Domain program written in C.

==========================================================================

