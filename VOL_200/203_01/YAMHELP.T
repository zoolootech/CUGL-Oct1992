Yamhelp.t	       Help File for YAM	       Rev 26 AUG 85

*****************************************************************
*
*	       	        Commands
*
*****************************************************************
n:		= Change to disk n.
b [m]		= Set baud rate to m.  Eg. b 9600.
break		= Send a break.  May abbreviate br.
bye		= hang up phone, return to dos.
call name	= dial call using yamphone.t.  May abbreviate name.
cd [ufn]	= change directory to ufn.
chat		= Chat kbd to kbd.
crc afn		= CRC calucation of files.
close		= Close files.  Dump capture buffer if receive file open.
crck afn	= Crck calcuation for files.
del afn		= delete files.
dir		= Directory listing.
dump		= hex-ascii dump of file.
e{mode[s]}	= Enable modes in term function.  (- = e)
d{mode[s]}	= Disable modes in term function.
f[modes] ufn	= Send file while in term function.
h		= Help.  Display this file yamhelp.t.
init		= Init YAM to default values.
kill		= Kill capture buffer data.
type afn	= Type file(s) with file length and xmsn time.
mn		= Set modem data port to n in decimal.
o{ff}		= Hang up modem, return to command mode.
pxm		= Set parameter to value m.
pwd		= print current working directory.
qu		= return to dos, do not hang up.
reset		= Dump capture buff, close file if open, re-init modem.
replot m	= Redisplay last m modem lines.
rewind		= Reset buffers.
r[opt] [file]	= Receive files.  Details below.
s[opt] [file]	= Send files.     Details below.
s		= Status display.
sum aft		= unix sum of afn.
t[c][mode] [file]	= Term function.  c=close files. ^e=exit.  See below.
tab [n]		= set tab expansion to n.
w		= Write capture buff to file if open.
wc afn		= word count of afn.
x		= same as bye.
;		= Delimiter for multiple cmmds on line.
!		= create a shell and execute dos command

*****************************************************************
*
*	       	        pxm Command Parameters
*
*****************************************************************
pxm	= Set parameter to value m.
gc	= Gochar set to c.  Default lf.
wn	= Waitnum set to n.  Default 1.
tm	= Throttle set to m.  Default approx 80 wpm.
pm	= Pause set to m.  Timeout after XOFF, p and w modes.


*****************************************************************
*
*	       	        Term Command
*
*****************************************************************
t[c][modes] [ufn] = Term cmmd with optional modes and file capture.

Note:	Dialog may be saved on disk AFTER term conversation
	by command sequence:  t ufn ; ^e ; close

Modes:	 Term modes are set by e(nable),d(isable) or f(ile sender) command.

c	= Close capture buffer immediately to already existing filename.
!	= Negate sense of following mode.
2	= Enable Compuserve A protocol.
a	= Keyboard cr sent as crlf.
b	= Binary (8 bit) transmission.
d	= Enb/dis dumping of captured text if squelch mode, else enb dumping.
e	= Echo characters.
f	= Full duplex.
g	= Go send file in term function.  Equiv to XON.
h	= Half duplex.
i	= Ignore, or strip off received data defined in RXNONO.
l	= List device on.  Printer is separately buffered.
n	= Newline (lf) sent only when xmit.  Store rcvd lf as crlf on disk.
p	= Pause after cr sent until remote echo rcvd.  Timeout if none.
r	= Return (cr) only sent at end of each line.
s	= Squelch dumping of captured data with ^t, unsquelch with ^r.
t	= Throttle to 50 wpm.
v	= View control chars as ^char and parity set chars as ~char.
w	= Wait for Gochar (default = lf) after sending cr.  Timeout if none.
x	= Exit from term when EOF encountered on transmitted file.
z	= Close file upon ^z while dumping in capture mode.  Else ignore ^z.


*****************************************************************
*
*	       	        Keyboarded characters in Term
*
*****************************************************************
^b		= Break char sent.
^e		= Exit to command mode.  Quick ^e^e sends one ^e.
^s		= Stop file xmsn.
^q		= Resume file xmsn stopped by ^s.
^v		= Replot last 24 lines.  Quick ^v^v sends one ^v.


*****************************************************************
*
*	       	        Send or Receive File Command
*
*****************************************************************
r | s[options] [file] = Receive or Send files.  Options below.

Notes:	Options MUST be reset each time s | r command used.
	Protocol compatable with Christenson (Modem7, etc)
	in single file, single sector transfers, cksum or crc.
	Do NOT use batch or 1K block modes with Modem7.
	Usage examples: rcvyt fn.ft, st fn.ft

b	= Batch. Not compatable with modem7.
	  Do not use filename (sender supplies) in r cmmd.
c	= Crc xfr request to sender.
k	= Use 1K blocks.
q	= Quiet mode.
t	= Term mode after xfr.
v	= View data.
y	= Yes, ok to cream old file.


*****************************************************************
*
*	       	        Replot Commands
*
*****************************************************************
b		= Beginning of buff.
k		= Kill buffer contents.
v | bs 		= View previous page.  (bs = backspace).
space		= Next page
/search-string<cr> = search buffer from current loc for string
p		= Backup 1 line.
n		= search for next occurrence of search string
lf		= Advance 1 page.
x		= Display rest of buff, exit.
z		= Kill rest of buff.
