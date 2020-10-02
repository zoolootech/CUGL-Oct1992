
Program CUGINS;

{
DESCRIPTION:

This program will concantenate each file specified in an input file
(up to 200 file names allowed) with a strandard CUG Header at front
of file. The header is inserted as is from a specification file, with
the exception that the name of the file is inserted in the filename
spot.

This program is used to setup a whole series of source files for
one program. It was used for initially for YACC with has almost
60 "C" Source  and "H" Header files. In the case of a program that
is made up many little modules, the only thing different about each
header is the Filename.

AUTHOR: C.E. Thornton [ Box 55085 Houston, TX 77255: (713) 467-1651) ]

APPLICATION: Setting up CUG Disks for their library

DATE:	JAN 28, 1985

NOTE: This software may be freely distributed and used by non-commerical
      users. Users Groups and Clubs may charge reasonable (less than $15)
      distribution fees. Contributions are always gratefully recieved.

>>>> Copyright Sept 1985 by C.E. Thornton

}
TYPE

  infile	= STRING[20];
  inlist	= ARRAY[1..200] OF infile;
  line		= STRING[255];
  iltext	= ARRAY[1..100] OF line;

VAR

  iflist:	  infile;
  ilfile:	  infile;

  lineuc:	  line;

  header:	  iltext;
  ilist:	  inlist;

  il:		  TEXT;
  cug:		  TEXT;
  fin:		  TEXT;
  fout: 	  TEXT;

  ilend:	  Integer;
  ihend:	  Integer;

  ii:		  Integer;
  ij:		  Integer;
  jj:		  Integer;
  fnlen:	  Integer;

  OS:		  Integer;
  OSE:		  Integer;
  ch:		  Char;
{
==============================================================================
	    C O N V E R T   S T R I N G   T O	U P P E R C A S E
==============================================================================
}
PROCEDURE caseupper (VAR instr,outstr: line);

VAR
  ichar:	  char;
  ii:		  integer;
{
  =============================
   CONVERT STRING TO UPPERCASE
  =============================
}
BEGIN;
  FOR ii := 1 to length(instr) DO
  BEGIN
    ichar := instr[ii];
    outstr[ii] := Upcase(ichar);
  END;
  outstr[0] := char(length(instr));
END;


{
==============================================================================
	    F I L E   E X I S T   T E S T   A N D   R E P O R T
==============================================================================
}
FUNCTION Exist (VAR filename: infile): Boolean;

VAR
  Tstfil:	  FILE;

{
  =============================
       DOES FILE EXIST?
  =============================
}
BEGIN;
  Assign(Tstfil, Filename);
  {$I-}
  Reset(Tstfil);
  {$I+}
  Exist := (IOresult = 0);
  close(Tstfil);

  If IOresult <> 0 THEN
  BEGIN
    Writeln('');
    Writeln(' *** ERROR - File "'+filename+'" does not exist!');
    Writeln(' ');
  END;
END;

{
==============================================================================
			 M A I N   P R O G R A M
==============================================================================
}

BEGIN;
{
  ===========================
     Check for Usage
  ===========================
}
  If (PARAMCOUNT < 2) OR (PARAMCOUNT > 3) THEN
  BEGIN
    Writeln('Usage:   CUGINS Infile Header [Replacement Offset]');
    Writeln;
    Writeln('  Where: "Infile" is the name of a file containing a list');
    Writeln('         of files, one file specification per line, which are');
    Writeln('         to be prefixed with the specified header information');
    Writeln('  Where: "Header" is the name of a file containing the C Users');
    Writeln('         Group (C.U.G.) Header. This information will be copied');
    Writeln('         intact, with the exception of inserting the current');
    Writeln('         file name into the header at the appropriate location');
    Writeln('  Where: "Replacement Offset" is optional and specifies the');
    Writeln('         offset between keyword (ie FILENAME: and Replacement');
    Writeln('         Text.');
    Writeln;
    Write('AUTHOR: C.E. Thornton [ Box 55085 Houston, TX ');
    Writeln('77255: (713) 467-1651) ]');
    Writeln('APPLICATION: Setting up CUG Library Disks');
    Writeln('DATE:   JAN 28, 1985     (Version 1.1)');
    Writeln;
    Write('NOTE: This software may be freely distributed ');
    Writeln('and used by non-commerical');
    Write('      users. Users Groups and Clubs may charge');
    Writeln(' reasonable (less than $15)');
    Write('      distribution fees. Contributions are always ');
    Writeln('gratefully recieved.');
    Writeln;
    Writeln('>>>> Copyright Sept 1985 by C.E. Thornton');
  END
  ELSE
  BEGIN
{
  ===========================
     LOAD INPUT FILE LIST
  ===========================
}
  iflist := PARAMSTR(1);
  While NOT Exist(iflist) Do
  BEGIN
    Write('Enter Correct input file list Name: ');
    Readln(iflist);
  END;

  assign(il,iflist);
  reset(il);

  ii := 1;
  WHILE ii <= 200 DO
  If NOT EOF(il) THEN
  BEGIN
    Readln(il,ilist[ii]);
    if ilist[ii] <> ' ' THEN
      ii := ii + 1;
  END
  ELSE
  BEGIN
    ilend := ii - 1;
    ii := 999;	   {force terminate the loop}
  END;

 Close(il);
{
  ===========================
      LOAD "CUG" HEADER
  ===========================
}
  iflist := PARAMSTR(2);
  While NOT Exist(iflist) Do
  BEGIN
    Write('Enter Correct Header Template File Name: ');
    Readln(iflist);
  END;

  assign(cug,iflist);
  reset(cug);

  ii := 1;
  WHILE ii <= 100 DO
  If NOT EOF(cug) THEN
  BEGIN
    Readln(cug,header[ii]);
    ii := ii + 1;
  END
  ELSE
  BEGIN
    ihend := ii - 1;
    ii := 999;	   {force terminate the loop}
  END;

 Close(cug);

 ch := ' ';

 If PARAMCOUNT = 3 Then
   BEGIN
     VAL (paramstr(3),os,ose);
     If ose <> 0 Then os := 1;
     if os < 1 Then os := 1;
   END
 Else
   os := 1;

{
  ============================================================================
		   M A I N   P R O C E S S I N G   L O O P
  ============================================================================
}


  FOR ij := 1 to ilend DO
    If NOT Exist(ilist[ij]) THEN
      Writeln
      ('File "',ilist[ij],'" Does not exist!')
    ELSE
    BEGIN
      assign(fout,'cugins.tmp');
      rewrite(fout);
      writeln(ilist[ij]);
      assign(fin,ilist[ij]);
      reset(fin);

      FOR jj := 1 to ihend DO
      BEGIN
	caseupper(header[jj],lineuc);
	fnlen := POS('FILENAME:',lineuc);
	IF fnlen > 0 THEN
	BEGIN
	  Writeln(fout,copy(header[jj],1,fnlen+9),ch:OS,ilist[ij]);
	END
	ELSE
	 Writeln(fout,header[jj]);
      END;

      WHILE NOT EOF(fin) DO
      BEGIN
	READLN(fin,lineuc);
	WRITELN(fout,lineuc);
      END;

      Close(fin);
      Close(fout);

      Erase(fin);
      Rename(fout,ilist[ij]);

    END;
  END;
  END.
