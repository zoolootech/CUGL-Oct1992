                    YAM (Yet Another Modem program)

                           by Chuck Forsberg

                         Manual Revised 8-29-81

 YAM  performs  a  number  of  "Super-Terminal"  functions.  It may be
 regarded  as  a  (not  proper)  superset  of  MODEM7.   Commands  and
 subcommands are listed below.

 XYAM is a version of yam which corresponds to XMODEM.



 a:               Change to a: disk (or b, etc.)

 a1:              Change to a: disk user 1

 bm               Set baudrate to m.  Example: "b19200"

 bye              Drop any call in progress and prepare to make/answer
                  another.

 call name        Enable Data  Terminal  Ready (DTR), and set baudrate
                  to the value (if present) corresponding to name.  If
                  autodial is supported,  dial  the telephone number.
                  If autodial is not  supported,  the telephone number
                  and  preferred baudrate are displayed.   It  is  not
                  necessary to type the entire  name  as it appears in
                  the file.  Example: "call tcbbs" or "call tc"

 close            Dump cpature buffer if a recrive file is open, close
                  files.

 dir [pattern ...] Display  pathnames  matching  pattern  alphabetized
                  across the page.  The usual  ambiguous filenames are
                  allowed under CP/M  provided  a  correct  version of
                  setfcb() is used.   Since  dir or any other commands
                  which  accept  a pattern use the circular buffer for
                  filename  expansion,  be  sure  to  write  out   any
                  captured data first!  Example: "dir" "dir  *.c" "dir
                  *.c *.h"

 dirr [pattern ...]
                  Displays the directory with the number of sectors in
                  each file.  The  number  of files matched, number of
                  blocks,  number  of kb allocated to those files, and
                  estimated  transmission time at the current baudrate
                  for  all  files  listed are displayed.  Transmission
                  time estimate is  based on batch transmission from a
                  Z89 with 700kb Tandon drives to a Cromenco 4mHz 4fdc
                  system  with  Persci  277 drive.  Time includes file
                  i/o but not error correction.

 d{mode}           Disable  mode,  where  mode is 1  or  more  of  the
                  following.        (Modes       affecting        file
                  transmission/reception affect term mode; the "s" and
                  "r" commands always send data transparently.

                  e              Exit  from  term  mode  when  EOF  is
                               encountered on transmitted file.

                  f            Full duplex.

                  h            Half Duplex.

                  g            Resumes (GO) sending  file once in term
                               mode, equivalent to XON.

                  i            Image transparent  data  capture, all 8
                               bits   of  all   characters   received,
                               including NULLS. This  overrides  the t
                               and/or z modes.

                  l             List  unit  (Printer)  on.   Since the
                               printer   is  separately  buffered,  it
                               needn't be as fast as the incoming data
                               as   long  as  the  difference  doesn't
                               exceed  the  buffer  size.   The rewind
                               command may be used to get extra copies
                               of the received data.

                  n               Send   NEWLINE   (lf)   only    when
                               transmitting file (no CR).

  