

Program CUGINS;

{
DESCRIPTION:

This program will replace the specified header entries in all files
specified in an input file (up to 200 file names allowed).

This program is used to edit a whole series of source files for
one program. It was used for initially for YACC with has almost
60 "C" Source  and "H" Header files.

AUTHOR: C.E. Thornton [ Box 55085 Houston, TX 77255: (713) 467-1651) ]

APPLICATION: Setting up CUG Disks for their library

DATE:	Sept 26, 1985

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
  inbuf:	  line;
  key1: 	  line;
  key2: 	  line;

  edtext:	  iltext;
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

  ki1:		  Integer;
  ki2:		  Integer;
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
  If (PARAMCOUNT <> 2) THEN
  BEGIN
    Writeln('Usage:   CUGEDT Infile Editfile');
    Writeln;
    Writeln('  Where: "Infile" is the name of a file containing a list');
    Writeln('         of files, one file specification per line, which are');
    Writeln('         to be prefixed with the specified edtext information');
    Writeln('  Where: "Editfile" is the name of a file containing the edit');
    Writeln('         information. The first line is the starting key and');
    Writeln('         second line is termination key. The program deletes');
    Writeln('         all text between the two keys and inserts all lines');
    Writeln('         following the two keys into the file in place of the');
    Writeln('         deleted Text. NOTE: The line containing the start key');
    Writeln('         is deleted and the termination line is left intact!');
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
      LOAD EDIT FILE
  ===========================
}
  iflist := PARAMSTR(2);
  While NOT Exist(iflist) Do
  BEGIN
    Write('Enter Correct Edit File Name: ');
    Readln(iflist);
  END;

  assign(cug,iflist);
  reset(cug);

  ii := 1;
  WHILE ii <= 100 DO
  If NOT EOF(cug) THEN
  BEGIN
    Readln(cug,edtext[ii]);
    ii := ii + 1;
  END
  ELSE
  BEGIN
    ihend := ii - 1;
    ii := 999;	   {force terminate the loop}
  END;

 Close(cug);


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

      caseupper(edtext[1],key1);
      ki1 := POS(':',key1);
      if ki1 <> 0 THEN ki1 := ki1 + 1;
      caseupper(edtext[2],key2);
      ki2 := POS(':',key2);
      if ki2 <> 0 THEN ki2 := ki2 + 1;


      WHILE ( (NOT EOF(fin)) AND (ki1 > 0) AND (ki2 > 0) ) DO
      BEGIN
	IF NOT EOF(fin) THEN READLN(fin,inbuf);
	caseupper(inbuf,lineuc);

	WHILE ( (NOT EOF(fin)) AND (POS(Copy(key1,1,ki1),lineuc) = 0) ) DO
	BEGIN
	  WRITELN(fout,inbuf);
	  READLN(fin,inbuf);
	  caseupper(inbuf,lineuc);
	END;

	WHILE ( (NOT EOF(fin)) AND (POS(Copy(key2,1,ki2),lineuc) = 0) ) DO
	BEGIN
	  READLN(fin,inbuf);
	  caseupper(inbuf,lineuc);
	END;

	IF NOT EOF(fin) THEN
	  FOR jj := 3 to ihend DO WRITELN(fout,edtext[jj]);

	WRITELN(fout,inbuf);

      END;

      Close(fin);
      Close(fout);

      Erase(fin);
      Rename(fout,ilist[ij]);

    END;
  END;
  END.
