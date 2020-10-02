.nr N 4
\" use "tbl -D z80adoc.mm |troff -Tps -mm |eps | lp -dpsript" 
\" on altos86 to print
.PF ""
.PH ""
.rs
.sp 5
.ce
\s24 Z80 Cross Assembler\s10
.sp 2
.ce
Portable Version
.sp 20
.AS
This program lets you use your computer to assemble
code for the Intel Z80 family of microprocessors.
The program is written in portable C.
All assembler features are supported except
relocation, linkage, and macros.
.AE
.SK
.nr P 1
.PH ""
.PF "''Page \\\\nP''"
.ce
Z80 Cross-Assembler (Portable)
.ce
Version 0.0
.ce
Copyright (c) 1988 Michael G. Panas
.SA 1
\" Use 3 for troff and 1 for nroff
.ds HF 1 1 1
\".ds HF 3 3 3
.H 1 "Introduction"
.P
This assembler was developed to cross-assemble the Z80 assembly language
mnemonics and pseudo opcodes defined by an assembler already
running on a mini-computer.
As such the mnemonics may look very strange, since they are not
standard Zilog Z80 mnemonics but, extensions to the 8080 language.
Many enhancements to the original mini-computer version have been made.
.P
This package assembles the dialect of Z80 source code
defined in Appendix I into Z80 object code.
It will run under DOS 2.x and up and also several 
UNIX machines (see appendix II - portability).
.H 1 "How to Use the Cross-Assembler"
.P
This program requires one input file (your Z80 source code) and 
zero to two output files (the listing and the object).  The input 
file MUST be specified, or the assembler will bomb on a fatal 
error.  The listing and object files are optional.  If no listing 
file is specified, no listing is generated, and if no object file 
is specified, no object is generated.  If the object file is 
specified, the object is written to this file in "Intel 
hexadecimal" format.

.ti
The command line for the cross-assembler looks like this:

.ti
z80a source_file { -l list_file } { -o object_file }

.ti
where the { } indicates that the specified item is optional.
.DS I
Some examples are in order:

z80a testz80.asm		 source:	testz80.asm
				listing:	none
				 object:	none

z80a testz80.asm -l testz80.prn	 source:	testz80.asm
				listing:	testz80.prn
				 object:	none

z80a testz80.asm -o testz80.hex	 source:	testz80.asm
				 listing:	none
				 object:	testz80.hex

z80a testz80.asm -l testz80.prn -o testz80.hex
				 source:	testz80.asm
				listing:	testz80.prn
				 object:	testz80.hex
.DE
.P
The order in which the source, listing, and object files are 
specified does not matter.  Note that no default file name extensions
are supplied by the assembler as this gives rise to portability problems.
.H 1 "Format of Cross-Assembler Source Lines"
.P
The source file that the cross-assembler processes into a 
listing and an object is an ASCII text file that you can prepare 
with whatever editor you have at hand.  The most-significant 
(parity) bit of each character is cleared as the character is 
read from disk by the cross-assembler, so editors that set this 
bit (such as WordStar's document mode) should not bother this 
program.  All printing characters, the ASCII TAB character (09H), 
and newline character(s) are processed by the assembler.  All 
other characters are passed through to the listing file, but are 
otherwise ignored.
.P
The source file is divided into lines by newline character(s).
The internal buffers of the cross-assembler will 
accommodate lines of up to 255 characters which should be more 
than ample for almost any job.  If you must use longer lines, 
change the constant MAXLINE in file z80a.h and recompile the 
cross-assembler.  Otherwise, you will overflow the buffers, and 
the program will mysteriously crash.
.P
Each source line is made up of three fields:  the label 
field, the opcode field, and the argument field.
The label field 
is optional, but if it is present, it must begin in column 1.  
The opcode field is optional, but if it is present, it must not 
begin in column 1.
If both a label and an opcode are present, 
one or more spaces and/or TAB characters must separate the two.  
If the opcode requires arguments, they are placed in the argument 
field which is separated from the opcode field by one or more 
spaces and/or TAB characters.
Multiple arguments are separated by semicolons.
Finally, an optional comment can 
be added to the end of the line.  This comment can begin with an
optional
asterisk which signals the assembler to pass the rest of the 
line to the listing and otherwise ignore it.
The assembler ignores any text after the argument field, for this
reason arguments must not contain any spaces.
Thus, the source line looks like this:
.DS I
{label}{ opcode{ arguments}}{{*}commentary}
.DE
where the { } indicates that the specified item is optional.
.DS I
Some examples are in order:
.DE
.DF I
column 1
|
v
GRONK		JP	NC; LOOP	This line has everything.
		INC	C		This line has no label.
		BEEP			This line has no opcode.
*					This line has no label and no opcode.

* The previous line has nothing at all.
		END			This line has no argument.
.DE
.H 2 Labels
.P
A label is any sequence of alphabetic or numeric characters 
starting with an alphabetic.  The legal alphabetics are:
.DS I
! $ % & , . : ? [ \\\ ] ^ _  ` { | }  ~  A-Z  a-z
.DE
.P
The numeric characters are the digits 0-9.
Note that "A" is not the same as "a" in a label.
This is an enhancement from the mini-computer version which
accepted only upper case labels.
This can explain mysterious U (undefined label) errors occurring
when a label appears to be defined.
.P
A label is permitted on any line except a line where the 
opcode is LIST, ULIST, NCODE, CODE, TTL, PAG, PAGT, SKP, TYP,
IF, ELS, or ENF.
The label is assigned the value of 
the assembly program counter before any of the rest of the line 
is processed except when the opcode is EQU, ORG, or SET.
.P
Labels can have the same name as opcodes, but they cannot
have the same name as operators or registers.  The reserved 
(operator and register) names are:
.DS 
	*	A	AF	AND	B	BC	
	C	D	DE	EQ	GE	GT
	H	HIGH	HL	IX	IY	L
	LE	LT	LOW	M	MOD	NC	
	NE	NOT	NZ	OR	P	PE
	PO	SHL	SHR	SP	XOR	Z
.DE
.P
Labels may be pin-addressed by the use of an "@" as a prefix to the label.
Pin-address labels are referenced in the code by the opposite side of the
bytes than they normally would. For example this DC strings' label
points to the right hand address of the string[B:
.DS I
LABEL	DC	"this is a normal string"
.DE
.P
If you then generate a pin-addressed version of the
same string the label refers to the left hand address of the string:
.DS I
@LABEL	DC	"this is a normal string"
.DE
.P
It is neccessary to include pin-addressing in this assembler
to remain compatable with existing source files written with
the mini-computer version.
The sense of byte addresses when referenced without pin-addressing
is reversed from what one would expect in many pseudo ops such
as DC and DAC.
.P
If a label is used in an expression before it is assigned a 
value, the label is said to be "forward-referenced."  For 
example:
.DS
	L1	EQU	L2+1	L2 is forward-referenced here.
	L2
	L3	EQU	L2+1	L2 is not forward-referenced here.
.DE
.P
Note no space between operands or operators is allowed.
.H 2 "Numeric Constants"
.P
Numeric constants can be formed in two ways; the mini-computer version,
or by the Intel convention.
The Intel convention was not part of the original mini-computer language,
but has been added as an enhancement.
.P
The original standard used only decimal or hexadecimal numbers in the
style of Motorola.
Decimal numbers are formed by using the digits 0-9. 
Hexadecimal numbers are formed by using a dollar sign ($) followed
by one or more hexadecimal digits (0-9, A-F).
The hex digits a-f are converted to upper case by the assembler.
Also added as an enhancement was the (%) Motorola style prefix
for binary numbers. In this case only 0 and 1 are allowed
as digits.
.P
A numeric constant formed according to the Intel convention
starts with a numeric character 
(0-9), continues with zero or more digits (0-9, A-F), and ends 
with an optional base designator.  The base designators are H for 
hexadecimal, none or D for decimal, O or Q for octal, and B for 
binary.  The hex digits a-f are converted to upper case by the 
assembler.  Note that an Intel style numeric constant cannot begin
with A-F as it would be indistinguishable from a label.
.P
Thus, all of the following evaluate to 255 (decimal):
.DS I
$ff  %11111111   0ffH   255   255D   377O   377Q   11111111B
.DE
.H 2 "String Constants"
.P
A string constant is zero or more characters enclosed in 
either single quotes (' ') or double quotes (" ").  Single quotes 
only match single quotes, and double quotes only match double 
quotes, so if you want to put a single quote in a string, you can 
do it like this:  "'".  In all contexts except the DB statement, 
the first character or two of the string constant are all that 
are used.  The rest is ignored.  Noting that the ASCII codes for 
"A" and "B" are 041H and 042H, respectively, will explain the 
following examples:
.DS I
"" and ''		evaluate to 0000H
"A" and 'A'	evaluate to 0041H
"AB"		evaluates to 4142H
.DE
Note that the null string "" is legal and evaluates to 0000H.
.P
Several escape sequences are also valid in strings:
.DS I
Character	HEX Gen	Function
&N		00	Null
&B		08	Backspace
&T		09	Tab
&V		0B	Vertical Tab
&I		0D	Carridge Return
&&		26	A single &
&F		FF	   -
&$XX		XX	Embedded Hex data
.DE
.H 2 "Expressions"
.P
An expression is made up of labels, numeric constants, and 
string constants glued together with arithmetic operators, 
logical operators, and parentheses in the usual way that 
algebraic expressions are made.  Operators have the following 
fairly natural order of precedence:
.DS I
Highest		anything in parentheses
unary +,  unary -
*,  /,  MOD,  SHL,  SHR
binary +,  binary -
LT,  LE,  EQ,  GE,  GT,  NE
NOT
AND
OR,  XOR
Lowest		HIGH,  LOW
.DE
.ti
A few notes about the various operators are in order:
.VL 8
.LI 1)
The remainder operator MOD yields the remainder from 
dividing its left operand by its right operand.
.LI 2)
The shifting operators SHL and SHR shift their left 
operand to the left or right the number of bits 
specified by their right operand.
Since the exponentiation operator (**) is not supported
the SHL operator can be used in most cases to replace it.
.LI 3)
The relational operators LT, LE, EQ, GE, GT, and NE can 
also be written as <, <= or =<, =, >= or =>, and <> or 
><, respectively.  They evaluate to 0FFFFH if the 
statement is true, 0 otherwise.
.LI 4)
The logical opeators NOT, AND, OR, and XOR do bitwise 
operations on their operand(s).
.LI 5)
HIGH and LOW extract the high or low byte, of an expression.
.LI 6)
The special symbol * can be used in place of a label or 
constant to represent the value of the program counter 
before any of the current line has been processed.
.LE
.DS I
Here are some examples of legal expressions:
.DE
.DF I
2+3*4					evaluates to 14
(2+3)*4				evaluates to 20
NOT 11110000B XOR 00001010B	evaluates to 00000101B
HIGH 1234H SHL 1			evaluates to 0024H
001Q EQ 0				evaluates to 0
001Q = 2 SHR 1				evaluates to 0FFFFH
.DE
.P
All arithmetic is unsigned with overflow from the 16-bit word ignored.
Thus:
.DS I
32768 * 2			evaluates to 0
.DE
.H 1 "Machine Opcodes"
.P
The opcodes of the Z80 processor that are supported by
this assembler are listed with thier syntax and Zilog
equivalents in Appendix I.
.H 1 "Pseudo Opcodes"
.P
Unlike Z80 opcodes, pseudo opcodes (pseudo-ops) do not 
represent machine instructions.  They are, rather, directives to 
the assembler.  These directives require various numbers and 
types of arguments.  They will be listed individually below.
.H 2 "Pseudo-ops -- DC and DATA"
.P
The DC and DATA pseudo-ops allow arbitrary bytes to be spliced into 
the object code.  Its argument is a chain of zero or more 
expressions that evaluate to -128 through 255 separated by 
semicolons.
If a semicolon occurs with no preceding expression, a 00H 
byte is spliced into the object code.  The sequence of bytes 
0FEH, 0FFH, 00H, 01H, 02H could be spliced into the code with the 
following statement:
.DS
	DC	-2;-1;;1;2
.DE
A special case exists here.  If a string constant is entered with 
no arithmetic done on it, then the entire string is spliced into 
the code stream.  Thus, the sequence of bytes 002H, 043H, 041H, 
054H, 044H could be spliced into the code with the following 
statement:
.DS
	DC	1+1;"CAT";"C"+1
.DE
.P
The label associated with a DC statement will point to the right-most
byte of the entire definition.
This can be changed to the left most byte by pin-addressing the label.
Thus the label in the next statement
points to the fifth byte (the null):
.DS
MESS	DC	"Mess";$00
.DE
.H 2 "Pseudo-ops -- DA"
.P
The DA pseudo-op is used to reserve a block of storage for 
program variables, or whatever.  This storage is not initialized 
in any way, so its value at run time will usually be random.  The 
argument expression (which may contain no forward references) is 
added to the assembly program counter.  The following statement 
would reserve 10 bytes of storage called "STORAGE":
.DS
	STORAGE	DS	10
.DE
.P
The label points to the right-most byte and may be changed
to left by pin-addressing the label:
.DS
	@STORAGE	DS	10
.DE
.H 2 "Pseudo-ops -- DAC"
.P
The DAC pseudo-op allows 16-bit words to be spliced into the 
object code.  Its argument is a chain of zero or more expressions 
separated by semicolons.  If a semicolon occurs with no preceding 
expression, a word of 0000H is spliced into the code.  The word 
is placed into memory low byte in low address, high byte in high 
address as per standard Intel order.  The sequence of bytes 
0FEH, 0FFH, 00H, 00H, 01H, 02H could be spliced into the code 
with the following statement:
.DS
	DW	0FFFEH;;0201H
.DE
.P
The label if present points to the right-most byte and may be changed
to left by pin-addressing the label.
.H 2 "Pseudo-ops -- CSECT"
.P
Used in the mini-computer version to enter a control section into
the object code. In this assembler it is a nop provided for
compatability only.
.H 2 "Pseudo-ops -- NCODE"
.P
Turns off object code generation in the output file.
NCODE allows RAM areas to be defined without the generation of any code.
The listing is still enabled unless turned off by a SKP.
.H 2 "Pseudo-ops -- CODE"
.P
Enables object code generation if previously disabled by NCODE.
.H 2 "Pseudo-ops -- END"
.P
The END pseudo-op tells the assembler that the source 
program is over.  Any further lines of the source file are 
ignored and not passed on to the listing.  If an argument is 
added to the END statement, the value of the argument will be 
placed in the execution address slot in the Intel hex object 
file.  The execution address defaults to the program counter 
value at the point where the END was encountered.  Thus, to 
specify that the program starts at label START, the END statement 
would be:
.DS
	END	START
.DE
.P
If end-of-file is encountered on the source file before an 
END statement is reached, the assembler will add an END statement 
to the listing and flag it with a * (missing statement) error.
.H 2 "Pseudo-ops -- EQU"
.P
The EQU pseudo-op is used to assign a specific value to a 
label, thus the label on this line is REQUIRED.  Once the value 
is assigned, it cannot be reassigned by writing the label in 
column 1, by another EQU statement by a REG statement, or by a 
SET statement.  Thus, for example, the following statement 
assigns the value 2 to the label TWO:
.DS
	TWO	EQU	1 + 1
.DE
.P
The expression in the argument field must contain no forward 
references.
.H 2 "Pseudo-ops -- IF, ELS, ENF"
.P
These three pseudo-ops allow the assembler to choose whether 
or not to assemble certain blocks of code based on the result of 
an expression.  Code that is not assembled is passed through to 
the listing but otherwise ignored by the assembler.  The IF 
pseudo-op signals the beginning of a conditionally assembled 
block.  It requires one argument that may contain no forward 
references.  If the value of the argument is non-zero, the block 
is assembled.  Otherwise, the block is ignored.  The ENF 
pseudo-op signals the end of the conditionally assembled block.  
For example:
.DS
	IF	EXPRESSION	;This whole thing generates
	DB	01H, 02H, 03H	;  no code whatsoever if
	ENF			;  EXPRESSION is zero.
.DE
The ELS pseudo-op allows the assembly of either one of two 
blocks, but not both.  The following two sequences are 
equivalent:
.DS I
		IF	EXPRESSION
		... some stuff ...
		ELS
		... some more stuff ...
		ENF

TEMP_LAB	SET	EXPRESSION
		IF	TEMP_LAB NE 0
		... some stuff ...
		ENF
		IF	TEMP_LAB EQ 0
		... some more stuff ...
		ENF
.DE
.P
The pseudo-ops in this group do NOT permit labels to exist 
on the same line as the status of the label (ignored or not) 
would be ambiguous.
.P
All IF statements (even those in ignored conditionally 
assembled blocks) must have corresponding ENF statements and 
all ELS and ENF statements must have a corresponding IF 
statement.
.P
IF blocks can be nested up to 16 levels deep before the 
assembler dies of a fatal error.  This should be adequate for any 
conceivable job, but if you need more, change the constant 
IFDEPTH in file z80a.H and recompile the assembler.
.H 2 "Pseudo-ops -- USE"
.P
The USE pseudo-op is used to splice the contents of another 
file into the current file at assembly time.  The name of the 
file to be USEd is specified as a normal string constant, so 
the following line would splice the contents of file "const.def" 
into the source code stream:
.DS
	USE	"const.def"
.DE
.P
USE files may, in turn, USE other files until four 
files are open simultaneously.  This limit should be enough for 
any conceivable job, but if you need more, change the constant 
FILES in file z80a.H and recompile the assembler.
.H 2 "Pseudo-ops -- ORG and RLC"
.P
The ORG pseudo-op is used to set the assembly program 
counter to a particular value.  The expression that defines this 
value may contain no forward references.  The default initial 
value of the assembly program counter is 0000H.  The following 
statement would change the assembly program counter to 0F000H:
.DS
	ORG	0F000H
.DE
.P
If a label is present on the same line as an ORG statement, 
it is assigned the new value of the assembly program counter.
.P
The RLC pseudo-op was used to relocate a block of code to run
in a different address than it was loaded.
It is provided here as an ORG statement since relocation is not
supported.
.H 2 "Pseudo-ops -- URLC"
.P
Nop provided for compatability.
.H 2 "Pseudo-ops -- PAG"
.P
The PAG pseudo-op always causes an immediate page ejection 
in the listing by inserting a form feed ('\\f') character before 
the next line.  If an argument is specified, the argument 
expression specifies the number of lines per page in the listing.  
Legal values for the expression are any number except 1 and 2.  A 
value of 0 turns the listing pagination off.  Thus, the following 
statement cause a page ejection and would divide the listing into 
60-line pages:
.DS
	PAGE	60
.DE
.P
Note arguments to PAG are an enhancement from the mini-computer version.
.H 2 "Pseudo-ops -- SKP"
.P
Causes subsequent lines to not be printed on the listing
until a TYP statment is encountered.
SKP/TYP blocks may be nested.
.H 2 "Pseudo-ops -- TYP"
.P
List output is resumed.
.H 2 "Pseudo-ops -- LIST and ULIST"
.P
Nops provided for compatability. Originally altered assembly
listing format.
.H 2 "Pseudo-ops -- SET"
.P
The SET pseudo-op functions like the EQU pseudo-op except 
that the SET statement can reassign the value of a label that has 
already been assigned by another SET statement.  Like the EQU 
statement, the argument expression may contain no forward 
references.  A label defined by a SET statement cannot be 
redefined by writing it in column 1 or with an EQU statement.  
The following series of statements would set the value of label 
"COUNT" to 1, 2, then 3:
.DS
	COUNT		SET	1
	COUNT		SET	2
	COUNT		SET	3
.DE
.H 2 "Pseudo-ops -- TTL"
.P
The TTL pseudo-op sets the running title for the listing.  
The argument field is required and must be a string constant, 
though the null string ("") is legal.  This title is printed 
after every page ejection in the listing, therefore, if page 
ejections have not been forced by the PAG pseudo-op, the title 
will never be printed.  The following statement would print the 
title "Random Bug Generator -- Ver 3.14159" at the top of every 
page of the listing:
.DS
	TTL	"Random Bug Generator -- Ver 3.14159"
.DE
.P
Subsequent title lines are used as subtitles.
.H 2 "Pseudo-ops -- PAGT"
.P
Set the subtitle line to "string".
This line is printed on the line following the TTL line on the
listing after every page ejection. May be changed many times.
.DS I
	PAGT	"string"
.DE
.H 1 "Assembly Errors"
.P
When a source line contains an illegal construct, the line 
is flagged in the listing with a single-letter code describing 
the error.  The meaning of each code is listed below.  In 
addition, a count of the number of lines with errors is kept and 
printed on the C "stderr" device (by default, the console) after 
the END statement is processed.  If more than one error occurs in 
a given line, only the first is reported.  For example, the 
illegal label "=$#*'(" would generate the following listing line:
.DS
	L   0000	FF 00 00	=$#*'(	LDI16	DE,1000H
.DE
.H 2 "Error * -- Illegal or Missing Statement"
.P
This error occurs when either:
.VL 8
.LI 1)
the assembler reaches the end of the source file 
without seeing an END statement, or
.LI 2)
an END statement is encountered in a USE file.
.LE
.P
If you are "sure" that the END statement is present when the 
assembler thinks that it is missing, it probably is in the 
ignored section of an IF block.  If the END statement is missing, 
supply it.
If the END statement is in a USE file, delete 
it.
.H 2 "Error ( -- Parenthesis Imbalance"
.P
For every left parenthesis, there must be a right parenthesis.  Count them.
.H 2 "Error `` -- Missing Quotation Mark"
.P
Strings have to begin and end with either " or '.  Remember 
that " only matches " while ' only matches '.
.H 2 "Error D -- Illegal Digit"
.P
This error occurs if a digit greater than or equal to the 
base of a numeric constant is found.  For example, a 2 in a 
binary number would cause a D error.  Especially, watch for 8 or 
9 in an octal number.
.H 2 "Error E -- Illegal Expression"
.P
This error occurs because of:
.VL 8
.LI 1)
a missing expression where one is required
.LI 2)
a unary operator used as a binary operator or vice-versa
.LI 3)
a missing binary operator
.LI 4)
a SHL or SHR count that is not 0 thru 15
.LE
.H 2 "Error I -- IF-ENF Imbalance"
.P
For every IF there must be a corresponding ENF.  If this 
error occurs on an ELS or ENF statement, the corresponding IF 
is missing.  If this error occurs on an END statement, one or 
more ENF statements are missing.
.H 2 "Error L -- Illegal Label"
.P
This error occurs because of:
.VL 8
.LI 1)
a non-alphabetic in column 1
.LI 2)
a reserved word used as a label
.LI 3)
a missing label on an EQU, or SET statement
.LI 4)
a label on an IF, ELS, or ENF statement
.LE
.H 2 "Error M -- Multiply Defined Label"
.P
This error occurs because of:
.VL 8
.LI 1)
a label defined in column 1 or with the EQU statement 
being redefined
.LI 2)
a label defined by a SET statement being redefined 
either in column 1 or with the EQU statement
.LI 3)
the value of the label changing between assembly passes
.LE
.H 2 "Error O -- Illegal Opcode"
.P
The opcode field of a source line may contain only a valid 
machine opcode, a valid pseudo-op, or nothing at all.  Anything 
else causes this error.
.H 2 "Error P -- Phasing Error"
.P
     This error occurs because of:
.VL 8
.LI 1)
a forward reference in a EQU, ORG, or SET 
statement
.LI 2)
a label disappearing between assembly passes
.LE
.H 2 "Error S -- Illegal Syntax"
.P
This error means that an argument field is scrambled.  Sort 
the mess out and reassemble.  In particular, look for use of 
registers that don't apply to a particular opcode, missing 
commas, and the like.
.H 2 "Error T -- Too Many Arguments"
.P
This error occurs if there are more items (expressions, 
register designators, etc.) in the argument field than the opcode 
or pseudo-op requires.  The assembler ignores the extra items but 
issues this error in case something is really mangled.
.H 2 "Error U -- Undefined Label"
.P
This error occurs if a label is referenced in an expression 
but not defined anywhere in the source program.  If you are 
"sure" you have defined the label, note that upper and lower case 
letters in labels are different.  Defining "LABEL" does not 
define "Label."
.H 2 "Error V -- Illegal Value"
.P
This error occurs because:
.VL 8
.LI 1)
an 8 bit immediate value is not -128 thru 255, or
.LI 2)
a DC argument is not -128 thru 255, or
.LI 3)
a USE argument refers to a file that does not exist.
.LE
.H 1 "Warning Messages"
.P
Some errors that occur during the parsing of the cross-
assembler command line are non-fatal.  The cross-assembler flags 
these with a message on the C "stdout" device (by default, the 
console) beginning with the word "Warning."  The messages are 
listed below:
.H 2 "Warning -- Illegal Option Ignored"
.P
The only options that the cross-assembler knows are -l and  
-o.  Any other command line argument beginning with - will draw 
this error.
.H 2 "Warning -- -l Option Ignored -- No File Name"
.H 2 "Warning -- -o Option Ignored -- No File Name"
.P
The -l and -o options require a file name to tell the 
assembler where to put the listing file or object file.  If this 
file name is missing, the option is ignored.
.H 2 "Warning -- Extra Source File Ignored"
.P
The cross-assembler will only assemble one file at a time, 
so source file names after the first are ignored.  To assemble a 
second file, invoke the assembler again.  Note that under CP/M-
80, the old trick of reexecuting a core image will NOT work as 
the initialized data areas are not reinitialized prior to the 
second run.
.H 2 "Warning -- Extra Listing File Ignored"
.H 2 "Warning -- Extra Object File Ignored"
.P
The cross-assembler will only generate one listing and 
object file per assembly run, so -l and -o options after the 
first are ignored.
.H 1 "Fatal Error Messages"
.P
Several errors that occur during the parsing of the cross-
assembler command line or during the assembly run are fatal.  The 
cross-assembler flags these with a message on the C "stdout" 
device (by default, the console) beginning with the words "Fatal 
Error."  The messages are explained below:
.H 2 "Fatal Error -- No Source File Specified"
.P
This one is self-explanatory.  The assembler does not know 
what to assemble.
.H 2 "Fatal Error -- Source File Did Not Open"
.P
The assembler could not open the source file.  The most 
likely cause is that the source file as specified on the command 
line does not exist.  On larger systems, there could also be 
priviledge violations.  Rarely, a read error in the disk 
directory could cause this error.
.H 2 "Fatal Error -- Listing File Did Not Open"
.H 2 "Fatal Error -- Object File Did Not Open"
.P
This error indicates either a defective listing or object 
file name or a full disk directory.  Correct the file name or 
make more room on the disk.
.H 2 "Fatal Error -- Error Reading Source File"
.P
This error generally indicates a read error in the disk data 
space.  Use your backup copy of the source file (You do have one, 
don't you?) to recreate the mangled file and reassemble.
.H 2 "Fatal Error -- Disk or Directory Full"
.P
This one is self-explanatory.  Some more space must be found 
either by deleting files or by using a disk with more room on it.
.H 2 "Fatal Error -- File Stack Overflow"
.P
This error occurs if you exceed the INCLude file limit of 
four files open simultaneously.  This limit can be increased by 
increasing the constant FILES in file z80a.H and recompiling the 
cross-assembler.
.H 2 "Fatal Error -- If Stack Overflow"
.P
This error occurs if you exceed the nesting limit of 16 IF 
blocks.  This limit can be increased by increasing the constant 
IFDEPTH in file z80a.H and recompiling the cross-assembler.
.H 2 "Fatal Error -- Too Many Symbols"
.P
Congratulations!  You have run out of memory.  The space for 
the cross-assembler's symbol table is allocated at run-time using 
the C library function malloc(), so the cross-assembler will use 
all available memory.  The only solutions to this problem are to 
lessen the number of labels in the source program or to add more 
memory.
.SK
.PF ""
.PH ""
\" define appendix heading
.nr Hu 1
.nr a 0
.af a I
.nr Hc 1 \" centered headings
.nr N 0

.de aH
.nr a +1
.nr P 1
.PF "'''Appendix \\na-\\\\\\\\nP'"
.SK
.HU "Appendix \\na - \\$1"
..
.aH "Z80 Opcode Table"
\" Opcode table for the Z80 microprocessor
.TS H

llll.
FORMAT	ENCODING	ZILOG FORM	COMMENT
.sp
.TH
ADC r		ADC A,r
ADCI n		ADC A,n
ADCHL d		ADC HL,d	T{
d may be BC, DE, HL, SP
T}
ADD r		ADD A,r
ADDI n		ADD A,n
ADDHL d		ADD HL,d	T{
d may be BC, DE, HL, SP
T}
ADDIX d		ADD IX,d	T{
d may be BC, DE, IX, SP
T}
ADDIY d		ADD IY,d	T{
d may be BC, DE, IY, SP
T}
AND r		AND A,r
ANDI n		AND A,n
BIT b;r		BIT b,r
CALL nn		CALL nn
CALLC c;nn		CALL c,nn
CCF		CCF
CMP r		CP r
CP r		CP r	T{
alternate form of CMP
T}
CMPI n		CP n
CPDEC		CPD
CPDR		CPDR
CPINC		CPI
CPIR		CPIR
CPL		CPL
DAA		DAA
DEC r		DEC r
DEC16 d		DEC d	T{
d may be BC, DE, HL, IX, IY, SP
T}
DI		DI
DJNZ nn		DJNZ nn	T{
The assembler fills in the displacement.
nn is the actual address to jump to.
T}
EI		EI
EXSPHL		EX (SP),HL
EXSPIX		EX (SP),IX
EXSPIY		EX (SP),IY
EXAFAF		EX AF,AF'
EXDEHL		EX DE,HL
EXX		EXX
HALT		HALT
IM0		IM 0
IM1		IM 1
IM2		IM 2
IN n		IN A,(n)
INPUT r		IN r,(C)	T{
r may be A, B, C, D, E, H, L
T}
INC r		INC r
INC16 d		INC d
INDEC		IND
INDR		INDR
ININC		INI
INIR		INIR
JUMP nn		JP nn
JP c;nn		JP c,nn
JUMPI d		JP (d)	T{
d may be HL, IX, IY
T}
JREL nn		JR nn	T{
nn is the location to jump to
T}
JR c;nn		JR c,nn
LD r1;r2		LD r1,r2
LDI r;n		LD r,n
LDABC		LD A,(BC)
LDADE		LD A,(DE)
LDBCA		LD (BC),A
LDDEA		LD (DE),A
LDA nn		LD A,(nn)
STA nn		LD (nn),A
LDI16 d;nn		LD d,nn	T{
d may be BC, DE, HL, IX, IY, SP
T}
LD16 d;nn		LD d,(nn)
ST16 nn;d		LD (nn),d
LDAI		LD A,I
LDAR		LD A,R
LDIA		LD I,A
LDRA		LD R,A
LDSP d		LD SP,d	T{
d may be HL, IX, IY
T}
LDDEC		LDD
LDDR		LDDR
LDINC		LDI
LDIR		LDIR
NEG		NEG
NOP		NOP
OR r		OR A,r
ORI n		OR A,n
OUTDR		OTDR
OUTINC		OUTI
OUTIR		OTIR
OUT n		OUT (n),A
OUTPUT r		OUT (C),r
POP d		POP d	T{
d may be AF, BC, DE, HL, IX, IY
T}
PUSH d		PUSH d	T{
d may be AF, BC, DE, HL, IX, IY
T}
RES b;r		RES b,r
RETURN		RET
RET c		RET c
RETI		RETI
RETN		RETN
RL r		RL r
RLA		RLA	T{
(short form RL A)
T}
ROLC r		RLC r
RLCA		RLCA	T{
(short form of RLC A)
T}
RLD		RLD
RR r		RR r
RRA		RRA	T{
(short form of RR A)
T}
RRC r		RRC r
RRCA		RRCA	T{
(short form of RRC A)
T}
RRD		RRD
RST x		RST x	T{
x may be 0, 8, 16, 24,...,56
T}
SBC r		SBC A,r
SBCI n		SBC A,n
SBCHL d		SBC HL,d	T{
d may be BC, DE, HL, SP
T}
SCF		SCF
SETB b;r		SET b,r
SLA r		SLA r
SRA r		SRA r
SRL r		SRL r
SUB r		SUB A,r
SUBI n		SUN A,n
XOR r		XOR A,r
XORI n		XOR A,n
.TE
.SK
NOTES:
.DS I
8-bit registers (r):
A
B
C
D
E
H
L
M	This is (HL) in Zilog terms
IX,0	This is (IX+0)
IY,0	This is (IY+0)

16-bit registers (d):
BC	B & C pair
DE	D & E pair
HL	H & L pair
IX	Index X
IY	Index Y
SP	Stack Pointer
AF	Accumulator & Flags

Condition flags (c):
Z or EQ	Zero/Equal
NZ or NE	Nonzero/Notequal
C
NC
P
M
PE
PO

n and nn are immediate expressions. n means an 8-bit value.
nn means a 16-bit address or value.

b refers to a bit position between 0 and 7. It may be
an expression evaluating to a number between 0 and 7.

Relative jumps allow only Z (EQ), NZ (NE),
C and NC for the condition (c).
.DE
.SK
.aH "Portability"
.P
This cross-assembler package has been compilied and run on the following
machines:
.VL 8
.LI 1.
MS DOS 2.x 3.x - Using Microsoft 'C' Version 3.0 and 5.1
.LI 2.
Altos System V UNIX - using makefile
.LI 3.
Xenix 3.0 - Using makefile
.LE



.VL 12
.                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                               umentation.
.LE
.nr Cl 3
.TC
