!  9"N1N:m 2�22�� "}��a� � 2:��a/	� :�
�O>1�:�
�0�C	� >2�!\ ~��r� <2\ �@2�!] ~� 6?#>?2h :\ =_� �2\ :���� ##~2�#~2�##^#V�"��#^#V�"���* .;^ �#~2�#~2�#^ �"��"�#)P"�"�* {�z�ҫ\ � <�S:�����NO FILE$File not found.$Directory for user $:
$\ � <ʿ=������!� ��	o:�S�p~��G}�
o:��G#�*�w#w#"�*#"! �* {�z��G��Out of memory.
$*}��
�*��!P s#r#�	��+}�����*"+}��Z*�|g}o��Z}�o"�#"��*�{�oz�g"��*�"�>ͨ�E*��*�͓*��*�{�oz�g�E��E�*�*�#{�z��*���!P"!  "�"�*+}�ʲ�* .ͱ�ʖ�(�����(���*>�|�²##"*+"�k�*^#V#"��>.��^ #~�))))�!���o& :��/�o�*�"�*�#"���>k�*+}��e"����3�k�2��;���;���;>0��0��G��=�x�0�_:�>0�:���2��2�3�	� *���	� *���	� � �*�#  �ڦW+}�ʷz¡�Ýi`:����)=����	� �� : Total of $k in $ files with $k space remaining.$�>:�> ���_� ����~�#��* .	ͱ��:�<��M]	� �(����2�>�>
��
(Strike any key to continue)$�^#V#N#F�_
�#��N)	�)	N�q#N�q�N)	�)	�N#F�^#V�_
�#�º��	� �3�	� �2Ð� �� *N��                            ��                                                                                                                      

With this version of Scribble: 
@message(What version of Scribble is this <C-Z>)@include(CON:)

And with this version of 
;		    SD.ASM ver 2.2
;		   (revised 6/5/81)
;
;		SUPER DIRECTORY PROGRAM
;		  by Bruce R. Ratoff
;
;Displays the directory of a CP/M disk, sorted alphabetically,
;with the file size in K, rounded to the nearest CP/M block size.
;
;This latest variation on a common theme will automatically adjust
;itself for any block size and directory length under CP/M 1.4 or 2.x
;or MP/M (any version).  If the screen fills, program will pause until
;a key is struck (see NPL and LPS equates below).  Total space used
;and number of files are printed at end.
;
;Command: SD FILENAME.FILETYPE or just SD
;
;Allows '*' or '?' type specifications.  Drive name may also be
;specified.  Ignores "SYS" files unless SOPT is TRUE and 'S' option
;is given (i.e., SD *.* S will print all files).
;
;==============================================================
;
;Fixes/updates (in reverse order to minimize reading time):
;
;06/05/81 Added PGPAWZ (page pause) conditional for remote
;	  CP/M systems where pausing may not be wanted.
;	  Setting PGPAWZ and REPSIZ to FALSE will result in
;	  a display like DIR, but sorted and with the stat
;	  of space remaining.  Rearranged equates to allow
;	  15 lines per page when narrow display is chosen.
;	  (KBP)
;
;06/01/81 Added version number, restored CTL-C break, added
;	  CTL-C test to allow break at page pause, added
;	  routine to gobble up any waiting console character
;	  at EXIT, added conditional assembly to allow no
;	  report of file sizes, added conditional assembly
;	  for direct console I/O for remote CP/M systems
;	  where phone line noise would garbage display.  (KBP)
;
;05/06/81 Corrected double printing of drive name in CALLB.
;	  Error only occurred with narrow display when file
;	  wasn't found. (Tim Nicholas)
;
;02/06/81 Changed sort to have odd gap (K+P say its faster)
;
;01/06/80 Changed sort from bubble sort to shell sort
;	  for faster speed.
;
;12/24/80 Changed BIOS conout to BDOS conout to allow
;	  printing of directory with CTL-P.  Also added
;	  print of remaining space even if file not
;	  found. (Steve Nossen)
;
;12/15/80 Added space suppression when printing file
;	  totals.  (KBP)
;
;12/14/80 Added logic to print space remaining on disk.
;	  Changed ^C test so that interrupting character is
;	  not echoed (makes remote use cleaner).  (BRR)
;
;12/02/80 Fixed bug in print routine which compared last file
;	  against garbage before printing. (BRR)
;
;11/29/80 Changed to allow printing 4 file names. (Ben Bronson
;	  and Keith Petersen)
;
;11/22/80 Fixed bug in handling >256 files.  Changed abort test
;	  in print routine to only abort on control-c.  (BRR)
;
;Based on 'DIRS' by Keith Petersen, W8SDZ
;
;NOTE: If you add improvements or otherwise update
;this program, please modem a copy of the new file
;to "TECHNICAL CBBS" in Dearborn, Michigan - phone
;313-846-6127 (110, 300, 450 or 600 baud).  Use the
;filename SD-NEW.NEW. (KBP)
;
;Set 'RMAC' TRUE to assemble with relocating assembler
;(requires link with PAGE 0 equates in separate file).
;
;==============================================================
;
FALSE	EQU	0	  ;DEFINE LOGICAL FALSE
TRUE	EQU	NOT FALSE ;DEFINE LOGICAL TRUE
;
ALTCPM	EQU	FALSE	;PUT TRUE HERE FOR H8 OR TRS-80
RMAC	EQU	FALSE	;PUT TRUE HERE FOR ASSEMBLY BY RMAC
SOPT	EQU	TRUE 	;PUT TRUE TO ALLOW 'SD *.* S' FORM
WIDE	EQU	TRUE 	;PUT TRUE TO ALLOW 4 NAMES ACROSS
REPSIZ	EQU	TRUE 	;PUT TRUE TO REPORT FILE SIZES
PGPAWZ	EQU	TRUE 	;PUT TRUE FOR PAUSE AFTER EACH PAGE
DIRCON	EQU	FALSE	;PUT TRUE FOR DIRECT CONSOLE OUTPUT
;
DELIM	EQU	':'	;FENCE (DELIMITER) CHARACTER
;
	IF	WIDE
NPL	EQU	4	;NUMBER OF NAMES PER LINE
LPS	EQU	23	;NUMBER OF LINES PER SCREEN
	ENDIF
;
	IF	NOT WIDE
NPL	EQU	3	;NUMBER OF NAMES PER LINE
LPS	EQU	15	;NUMBER OF LINES PER SCREEN
	ENDIF
;
	IF	ALTCPM
BASE	EQU	4200H
TPA	EQU	4300H
	ENDIF
;
	IF	RMAC
	EXTRN	BASE,FCB,BDOS	;MAKE BA