#  HEADER:        ;
#  TITLE:         Makefile dependency generator;
#  DATE:          09/28/1989;
#  VERSION:       1.2;
#  DESCRIPTION:   "An AWK program which finds included files in a PL/M
#                 source file and builds a dependency list.  This
#                 dependency list is a component of a make-file.  The list 
#                 consists of the source file name followed by a colon and
#                 then all of the files which it includes (and also
#                 those which are included by the included files, &c.)
#                 The last item is a compile-command.";
#
#  KEYWORDS:      Makefile, make, include, dependency generator;
#  SYSTEM:        MS-DOS, UNIX;
#  WARNINGS:      "Doesn't handle directory names embedded within
#                 $include(here).  Doesn't handle preprocessor $if.. blocks
#                 or $include statements which are "commented out."
#                 PolyAwk will generate false matches to lines
#                 beginning with non-ASCII (i.e. 128..256) chars."
#                 Filenames are case sensitive.";
#  FILENAME:      PINCDEP.AWK;
#  SEE-ALSO:      TLR2MAK.AWK, CF2MAK.AWK, CINCDEP.AWK, LBI2MAK.AWK;
#  AUTHORS:       James Yehle;
#  COMPILERS:     PolyAWK, Mortice Kern AWK, Rob Duff's PC-AWK 2.0,
#                 In any case, must be 1985 awk (not 1977);
# ============================================================================
#
# pincdep.awk         Last modified  Sep 28, 1989  22:30
#
#   Scans a PL/M source file for all "$include" dependencies
#   This scanning process involves all nested include files, too.
#
#   Usage is:
#     awk -f [path]pincdep.awk [path]src_file.c objname=[path]filename
#        cc=compile_comd ccf=[p][n][e] [>[path]outfile]
#     Pathnames must contain trailing separator, e.g. "c:\inc\"
#     Order of command-line parameters (save for "-f ..." 
#      and srcfile.c) is irrelevant, but identifiers must be in lower case.
#     "src_file.c" may be replaced by special "__NOFILE__"
#     cc is the compile command
#     ccf controls whether directory path (p) precedes filename in
#         the compile command line, and whether extension is added (e)
#
#   Output is:
#     [path]src_file.obj: [path]src_file.c [path]included_file1 linecont
#                         [path]included_file2 [path]included_file3 ...
#     <\t>comd [path]src_file.c
#     <\n>
#   For __NOFILE__, output is:
#     [path]src_file.obj:
#     <\n>
#     <\n>
#
#  Jim Yehle
#
# . . . Revision history . . . . . . . . . . . . . . . . . . . . . . . . . . .
#
#  1.2  Sep 10 89  JRY  Added legal-filename-char regular expression
#  1.1  Aug 17 89  JRY  Added add_fnexist option (as internal directive)
#                       If an included file can't be found, pincdep now
#                        beeps and prints an error to CO, then continues
#                        without scanning the file for nested inclusions.
#                        (It adds it to the dependency list.)
#                        Also added an optimization:  if addfile() sees
#                        that a file is already in the list, then it has
#                        already been scanned, so it isn't done again.
#                       Added "verbose" CO output as debug level 1.
#  1.0  Jul 23 89  JRY  Made pincdep for PL/M files; adapted from
#                        Intel asm286 assembly dependency scanner 
#                        aincdep.awk, which was in turn adapted from
#                        the original C version cincdep.awk.
# . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
#
# Notes about this program:
#  It assumes Intel PLM286 compiler syntax
#  It doesn't make any attempt to switch out $If...$Endif blocks.
#  It is powerless to cover files $Included in the compiler's command-line
#  It has no facilities for an include-file directory to be specified
#   (since iRMX's PL/M compiler has none)
#
# Here are the syntax rules for $Include() (or $IC()):
#  Control lines begin with $ in column 1 (or in LEFTCOL)
#   and end with or end-of-line or a semicolon before end-of-line
#  Multiple controls (immediately) follow the $, separated by space/tabs
#  Only one "include" control is allowed per line
#
# What I can't deal with:
#  "include(...)" as a parameter within some other control
#   e.g. "title 'include(blah)' "
#  LEFTCOL settings other than 1
# ============================================================================
#

BEGIN {
   co = "CON"       # Console-out: MS-DOS "CON", iRMX ":co:", unix "/dev/tty"
   linecont = " \\" # Line-continuation EOL char: snake " /", unix make "\\"
   debug = 0        # 0=off, 1=verbose, 2=main-level debug, 3= adds functions
   use_fnexist = 1  # Add nonexistent header files to dependency list?
   fname_chars = "[^\\\\ :]" # Any single legal char in a file name.
   #  (Must exclude directory path separator character! (DOS \, UNIX /) )

   printf( "pincdep.awk  PL/M $include dependency scanner  v 1.2") > co
   objfile = get_cl_param( "objname=", 0)
   if (debug>1) printf( "objfile = '%s'\n", objfile) > co
   cc = get_cl_param( "cc=", 0)
   if (debug>1) printf( "cc = '%s'\n", cc) > co
   ccf = get_cl_param( "ccf=", 0)
   if (debug>1) printf( "ccf = '%s'\n", ccf) > co
   if (ARGV[1] == "__NOFILE__") {
      if (debug)                    
         printf( "\n__NOFILE__: Empty dependency list generated.\n") > co
      else
         printf( "  (of __NOFILE__)\n") > co
      exit # Hop to END; srcfile is still an empty string & flist is empty
   }
   # Split primary source file name into Path, Name, and Extension components
   # 1st in list of ARGV[]'s is source file (FILENAME not set in BEGIN section)
   fullsrcfile = ARGV[1]
   split_filename( fullsrcfile, srcfile, fname_chars)
   if (debug)
      printf( "\n Scanning primary source file '%s%s%s'\n",
              srcfile["path"], srcfile["name"], srcfile["ext"]) > co
   else
      printf( "  (of %s%s%s)\n",
              srcfile["path"], srcfile["name"], srcfile["ext"]) > co
   if (debug>1) printf( "srcfile[\"path\"] = \"%s\"\n", srcfile["path"]) > co
   if (debug>1) printf( "srcfile[\"name\"] = \"%s\"\n", srcfile["name"]) > co
   if (debug>1) printf( "srcfile[\"ext\"] = \"%s\"\n", srcfile["ext"]) > co
}
#
/^\$[^;]*[Ii][Nn][Cc][Ll][Uu][Dd][Ee][ \t]*\(.*\)/ {
   if (debug>1) printf( "$include control line: $0='%s'\n", $0) > co
   extract_include_filename()  # into $1
   if (debug>1) printf( " extracted filename = '%s'\n", $1) > co
   # Check for nested inclusions by scanning included file for $Include's
   scanfile( $1)
}
/^\$[^;]*[Ii][Cc][ \t]*\(.*\)/ {
   if (debug>1) printf( "$include control line: $0='%s'\n", $0) > co
   extract_ic_filename()  # into $1
   if (debug>1) printf( " extracted filename = '%s'\n", $1) > co
   # Check for nested inclusions by scanning included file for $Include's
   scanfile( $1)
}

# Produce the dependency list printout
END {
   # Print primary (object) file name before list (left of colon),
   # Print primary source file name (*.c) as first item in list.
   printf( "%s: %s", objfile, fullsrcfile )
   if (debug)  printf( "%s: %s", objfile, fullsrcfile) >co
   linelen = length(objfile) + 2 + length(fullsrcfile)

   for (i=1; i in flist; i++) {
      # If line will be too long, put out a line-continuation char and newline
      if (linelen + 1 + length(flist[i]) + length(linecont)  > 79) {
         printf( "%s\n", linecont)
         if (debug)  printf( "%s\n", linecont) >co
         # On the new line, tab out to underneath the first included file name
         for (linelen=0; linelen<length(objfile)+1; ++linelen) {
            printf(" ")
            if (debug)  printf(" ") >co
         }
      }
      printf( " %s",  flist[i] )
      if (debug)  printf( " %s",  flist[i] ) >co
      linelen += length(flist[i]) + 1
      }

   # Build the compile command line's only parameter: the filename
   #  depending on options specified in ccf command-line param
   compile_comd_filename = sprintf( "%s%s%s",
                                    ccf ~ /[Pp]/? srcfile["path"] : "",
                                    ccf ~ /[Nn]/? srcfile["name"] : "",
                                    ccf ~ /[Ee]/? srcfile["ext"]  : "" );
   if (debug>1) 
      printf( "\ncompile_comd_filename = \"%s\"", compile_comd_filename) > co
   printf( "\n\t%s %s\n\n", cc, compile_comd_filename)
if (debug)    printf( "\n\t%s %s\n\n", cc, compile_comd_filename) >co
}
#
function scanfile( filename,  # <- parameters
                   glr)     { # <- local variables
   # glr is a local variable: getline return value.
   glr = getline <filename
   if (glr == -1) { # Error opening file.. assume doesn't exist
     printf( "\007PINCDEP.AWK error:  cannot open file %s\n", filename)  > co
     if (use_fnexist) # Put it in list anyway, if you say so
        addfile( filename,  flist )
     return # Can't scan this file
   }
   # File has been found (its name is in filename) and opened.
   addfile( filename, flist)
   while (glr == 1) {
      if ($0 ~ /^\$[^;]*[Ii][Nn][Cc][Ll][Uu][Dd][Ee][ \t]*\(.*\)/ ) {
         extract_include_filename()
         scanfile( $1)
      }
      if ($0 ~ /^\$[^;]*[Ii][Cc][ \t]*\(.*\)/ ) {
         extract_ic_filename()
         scanfile( $1)
      }
      glr = getline <filename
   }
   close( filename)
}
#
function extract_include_filename() {
   # Takes $0 as input; produces extracted filename in $1
   if (debug>2)
      printf( " extract_include_filename() entered: $0='%s'\n", $0) > co
   # Delete "$[^;]*include(" from $0
   # This forces re-parsing, making $1 into (unquoted) filename.
   sub( /^\$[^;]*[Ii][Nn][Cc][Ll][Uu][Dd][Ee][ \t]*\([ \t]*/, "")
   sub( /[ \t]*\).*$/, "")
   # Filename, stripped of either type of quote char, is now $1 (also $0)
}
function extract_ic_filename() {
   # Takes $0 as input; produces extracted filename in $1
   if (debug>2)
      printf( " extract_ic_filename() entered: $0='%s'\n", $0) > co
   # Delete "$[^;]*include(" (or "$..ic(") from $0
   # This forces re-parsing, making $1 into (unquoted) filename.
   sub( /^\$[^;]*[Ii][Cc][ \t]*\([ \t]*/, "")
   sub( /[ \t]*\).*$/, "")
   # Filename, stripped of either type of quote char, is now $1 (also $0)
}

function addfile( filename, flist,    flix) {
   # filename is a string to be added to flist[]
   # flist[] is an array of strings to which addfile() appends a file name
   # flix is a (local) index into flist[]
   for (flix=1; flix in flist; flix++)
      if (flist[flix] == filename) # Already in list--
         return                    #  don't add a duplicate
   flist[flix] = filename
}
#
function get_cl_param( parname,  # Which parameter ya lookin' fer? (e.g. "blah=")
                       optional) # Is it an optional parameter?
# Search the ARGV[] array for a command-line parameter beginning w/ parname
# If optional is 0, aborts program if not found
# Returns the string to the right of the '='
{
   for (i in ARGV)
      if ( match(ARGV[i],parname) )
         return substr( ARGV[i], length(parname)+1 )

   if (optional)
      return ""

   printf( "pincdep.awk usage error:  Command-line parameter '%s' missing\n",
           parname )                                                      > co
   exit 1  # Abort
}
#
function split_filename( name,        # In: The full file name
                         shards,      # Out:array which receives ["path"],
                                      #     ["name"] and ["ext"] members
                         fname_chars, # In: Regular expression which matches
                                      #     any single file-name character.
                                      #     (must exclude directory separator)
                         # Local variables:
                         pnsep,       # Path/Name separator (1st char of Name)
                         nesep)       # Name/Extension separator (1st Ext char)

# Split a file name into three components: Path, Name, and Extension 
{
   pnsep = match( name, fname_chars"*$")
   shards["path"] = substr( name, 1, pnsep-1)
   nesep = match( name, "\\."fname_chars"*$")
   if (debug>2)
      print "in \"" name "\", name starts @ " pnsep ", ext @ " nesep >co
   if (nesep==0) {
      shards["name"] = substr( name, pnsep)
      shards["ext"] = ""
   }
   else {
      shards["name"] = substr( name, pnsep, nesep-pnsep)
      shards["ext"] = substr( name, nesep)
   }
}

