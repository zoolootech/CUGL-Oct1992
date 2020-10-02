                         CHECK REGISTER PROGRAM
                          CP/M-80 Version 1.0
                             by WoolleyWare

The Check Register Program simplifies the task of managing one or more
checking accounts.  Because of its size, the program was originally
provided as two separate library files named CHX8010A.LBR and
CHX8010B.LBR.  The filenames were derived from the following:

     CHX  Acronym for Check Register Program
     80   CP/M-80 Version
     10   Version 1.0
     A    Executable and Demonstration Files
               -READ-ME.A     Similar to -READ-ME.B
               ABSTRACT.DOC   Abstract of document
               CHECKS.COM     Manage current check register
               CLEARS.COM     Mantain/summarize cleared entries
               CHECKSIN.COM   Install CHECKS.SCR
               CHECKSIN.DAT   Installation data
               CHECKS.SCR     Screen control data
               DEMO.DAT       Demonstration check register
               DEMO.INF       Demonstration information
  or B    Documentation and C Language Source Files
               -READ-ME.B     This file
               CHECKS.DOC     Complete document
               CHECKS.H       \
               CHECKS.C        |
               CHECKA.C        |
               CHECKB.C         >  Source files
               CHECKC.C        |
               CHECKD.C        |
               XIO.C          /

You may find it convenient to evaluate the A library first, to determine
whether the program will satisfy your needs.

An appropriate public domain (PD) program such as LU, NULU, or LSWEEP
may be used to extract files from each LBR distribution library.  Any
which are squeezed (have a Q as the second letter of the file type, as
in FILENAME.TQP) should be unsqueezed using a PD program like USQ or
NSWEEP.

The CHECKS.DOC file should be printed for your use.  Do not use a
formatting print routine such as WordStar's Print function.  CHECKS.DOC
has already been formatted, so that page numbers will be consistent with
the Table of Contents.  It may be conveniently printed using the CP/M
PIP utility.  Place PIP.COM and CHECKS.DOC in drive A.  At the A>
prompt, type

     PIP LST:=CHECKS.DOC

after first making sure that your printer is set for 66 lines per page
(11 inch page at 6 lines per inch) and 80 columns per line, loaded with
continuous fan-fold paper, and positioned with the print-head at top-of-
page.

Once all necessary files have been extracted and unsqueezed, store the
original LBR distribution disk in a safe place as a backup.  That disk
should be used if you wish to make a copy for a friend.

Read the document for detailed information about the Check Register
Program.
