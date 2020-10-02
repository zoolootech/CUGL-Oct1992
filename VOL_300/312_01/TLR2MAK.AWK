#  HEADER:        ;
#  TITLE:         Turbo Link Response file object file extractor
#  DATE:          09/20/1989;
#  VERSION:       1.2;
#  DESCRIPTION:   "An AWK program which scans a link-configuration
#                 file and builds a batch file which will in turn
#                 produce a make-file.  Requires the services of a
#                 source-file dependency generator (such as cincdep)."
#  KEYWORDS:      Makefile, make, include, dependency generator;
#  SYSTEM:        MS-DOS;
#  WARNINGS:      "Filenames are assumed to contain only characters from
#                 the limited set [A-Za-z_0-9.].  Your TLR file must
#                 contain all of the object files which you want scanned
#                 for dependencies, as well as the executable file name."
#                 PolyAwk will generate false matches to lines
#                 beginning with non-ASCII (i.e. 128..256) chars."
#  FILENAME:      TLR2MAK.AWK;
#  SEE-ALSO:      CINCDEP.AWK, CF2MAK.AWK, AINCDEP.AWK, PINCDEP.AWK;
#  AUTHORS:       James Yehle;
#  COMPILERS:     PolyAWK, Mortice Kern AWK, Rob Duff's PC-AWK 2.0;
#
#-----------------------------------------------------------------------------
#
#  TLR2mak.awk              Last modified  Sep 20, 1989  12:54
#
#     Scans an Turbo Link response file to produce a batch file for
#     use with cincdep.awk to build a set of make-file dependency lists
#
#  Jim Yehle  753 Left Fork Rd. #B/Sugarloaf Star Route/Boulder, CO 80302 9252
#
# . . . Revision history . . . . . . . . . . . . . . . . . . . . . . . . . . .
#
#       Sep 10 89  JRY  -- Released for C User's Group distribution --
#  1.2  Sep 10 89  JRY  Added legal-filename-char regular expression
#  1.1  Sep 8  89  JRY  Added "verbose" CO output as debug level 1.
#  1.0  Aug 31 89  JRY  Initial creation.. adapted from Intel bind-
#                        configuration file extractor CF2MAK.AWK (v 1.5).
# . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
#
#  Usage is:
#    awk -f tlr2mak.awk filename.tlr outfile=[path]filename
#        lc=command lcf=[p][n][e] [>batchfile]
#     where outfile gets the conglomerated dependency generator output
#     Order of command-line parameters (save for "-f ..." 
#      and ">batchfile") is irrelevant, but identifiers must be in lower case.
#     lc is the compile command (no spaces allowed)
#     lcf controls which components of the source file's name get
#         included in the link command line: p = directory path,
#         n = name, e = extension.
# . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
#
#  Takes a Turbo Link response file as input.  Assumes that the portion of the
#   TLINK command line containing the object file list and the executable file 
#   is put into a "Turbo Link Response" file.  The linker must be invoked via a
#   form such as "TLINK /c c0s @bawk.tlr,bawk.map,emu maths cs", where BAWK.TLR
#   might contain "o:\bawk o:\bawkdo o:\bawkact o:\bawkpat o:\bawksym, o:\bawk".
#   Alternatively, you could just put the whole business into BAWK.TLR and
#   tell the linker "TLINK @bawk.tlr".
#  Generates an executable-dependency list at the start of outfile,
#   followed by the link command and the primary (executable) file name.
#  Extracts all object file names.
#  Generates a batch file line, for each object file found, of the form:
#   comdpre srcfile "incpath="incpath "objname="file.obj
#     "cc="cc "ccf="ccf comdmid outfile comdpost
#  unless the source file can't be found, in which case it makes this line:
#   comdpre "__NOFILE__ objname="file.obj "cc= ccf= " comdmid outfile comdpost
#  The search for the source file covers C files only, but can be easily
#   extended to support other languages. (See example in CF2MAK.AWK.)
#
#  Assumes object file names contain only these chars: [A-Za-z0-9_]*.obj
# . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
#
# Turbo Link (response file) rules:
#
#  Lines can be continued by putting a '+' at the end of line
#  Unterminated line is taken as a comma 
#
#  Turbo Link's command-line invocation form is:
#   TLINK /options objfile objfile ..., exefile, mapfile, libfile libfile ... 
#  Everything past TLINK can come from a response file, as in TLINK @exe.tlr
#
#-----------------------------------------------------------------------------
#
BEGIN {
   #  Cf2mak supports several languages.  Each language must have
   #   1) a source file name extension to replace object file extension (.obj)
   #   2) a source directory pathname (stuck on front of file name during search)
   #   3) an include-file directory whose name to pass to a dependency scanner
   #   4) a compiler command to pass to dependency scanner (no spaces allowed)
   #   5) controls for filename pathname/extension inclusion in compile command line
   #   6) a dependency generator awk program

   # Language 1 is C
   ext[1] = ".c"
   path[1]    = "s:\\c\\"
   ipath[1]   = "s:\\c\\inc\\"
   cc[1] = "ccc"
   ccf[1] = "n"
   depgen[1] = "s:\\awk\\cincdep.awk"

   # Set various strings used in building batch-file command lines

   #    allpre      Leading line(s) of entire batch file
   #    allpost     Trailing line(s) of entire batch file
   #    comment     Comment-line lead-in: DOS "REM" or "ECHO", iRMX ";"
   #    comdpre     Command line prefix
   #    comdmid     Command line midle: between source & output file names
   #    comdpost    Command line suffix
   #    linecont    Line-continuation EOL char: snake " /", unix make "\"
   #    fname_chars Regular expression specifying any single legal char
   #                which can appear in a file name.  Must exclude 
   #                directory path separator character! (DOS \, UNIX /)
   allpre = "echo off"
   allpost = "echo on"
   comment = "echo "
   comdpre = "awk -f "
   comdmid = " >>"
   comdpost = ""
   linecont = " \\"
   fname_chars = "[^\\\\ :]" # Unix "[^/ ]"; used to use "[A-Za-z0-9_]"
#
   co = "CON"   # Console-out: MS-DOS "CON", iRMX ":co:", unix "/dev/tty"
   debug = 0    # 0=off, 1=verbose, 2=main-level debug, 3= adds functions

   print "tlr2mak.awk  Bind configuration object-file extractor  v 1.2" > co

   outfile = get_cl_param( "outfile=", 0)
   lc = get_cl_param( "lc=", 0)
   lcf = get_cl_param( "lcf=", 0)

   print allpre

   field = "objs"
   field_end = 0
}
#
{
   gsub( /[\t]/, " ", $0) # Tabs -> spaces; re-parse $0

   # If line-continuer is glued to a fname, push it into next fname
   sub( /\+$/, " +", $0) # (forces re-parsing of $0 into $1..$NF+1)
   if (debug>1)
      print "Line #" NR " has " NF " fnames:" >co
   # Preserve NF, $1..$NF; since sub-functions' getline() will change them
   nfnames = NF
   for (f = 1; f <= NF; ++f) {
      fname[f] = $f
      if (debug>1)
         print " -> fname[" f "] = '" $f "'." >co
   }

   # Process each file name (i.e. word, or $1..$NF) in the line
   for (f = 1; f <= nfnames; ++f) {
      if (debug>1)
         printf( "Processing fname[%d]: '%s'\n", f, fname[f]) >co
      if (f==nfnames && fname[f]=="+") {
         if (debug>1)
            printf("Last fname[%d] == \"+\" (\"%s\")\n", f, fname[f]) >co
         break
      }  
      if ( fname[f] ~ /^\// ) { # 1st char is '/':  its an /option
         field_end = 0
         if (debug>1)
            print " ..it's an option." >co
         continue
      }
      # If fname ends with a comma, remove it; stop after this fname
      field_end = sub( /,$/, "", fname[f] )
      if (field=="exe") { # Get executable-file name
         # Split executable file name into Path, Name, and Extension components
         split_filename( fname[f], exefile, fname_chars)
         if (exefile["ext"]=="") { /* Turbo Link assumes ".EXE" extension
            if (debug>1)
               printf( " \".EXE\" added to executable-name \"%s\"\n",
                       exefile["path"] exefile["name"]              ) >co
            exefile["ext"]==".EXE")
         }
         if (debug>1) 
            printf( "Executable file name components: '%s' '%s' '%s'\n",
                     exefile["path"], exefile["name"], exefile["ext"]  ) >co
         exit
      }
      # Object file name has been extracted (into fname[f])
      if (debug>1)  print " ..it's an object file." >co
      fullobjname = fname[f]

      split_filename( fullobjname, objnameparts, fname_chars)

      # If extension missing, add ".OBJ" (This is Turbo Link's rule.)
      if (objnameparts["ext"]=="") {
         if (debug>1)
            printf(" \".OBJ\" added to object file name \"%s\"\n",
                   objnameparts["path"], objnameparts["name"]    ) >co
         objnameparts["ext"] = ".OBJ"
         fullobjname = fullobjname ".OBJ"
      }
      for (flix=1; flix in flist; flix++)
         ;
      flist[flix] = fullobjname # Add to array

      # Generate one batch file line
      look_for_source_file( objnameparts["name"], fullobjname)

      if (field_end) {
         field = "exe"
         if (debug>1) print "End of object-files section."
      }
   }

   if (fname[nfnames] != "+") {
      field = "exe"
      if (debug>1) 
         printf( "fname[%d] != \"+\" (\"%s\"): end of object-files section.\n",
                 nfnames, fname[nfnames]                                   ) >co 
   }

}
#
END {
   print allpost

   # Generate executable-file dependency list (into outfile)
   # Print primary (executable) file name before list (left of colon)
   primarydependent = sprintf( "%s%s%s", exefile["path"],
                               exefile["name"], exefile["ext"])
   printf( "%s:", primarydependent) >outfile
   if (debug) printf( "%s:", primarydependent) >co
   linelen = length(primarydependent) + 1
   for (i=1; i in flist; i++) {
      # If line will be too long, put out a line-continuation char and newline
      if (linelen + 1 + length(flist[i]) + length(linecont)  > 79) {
         printf( "%s\n", linecont) >outfile
         if (debug) printf( "%s\n", linecont) >co
         # On the new line, tab out to underneath the first included file name
         for (linelen=0; linelen<length(primarydependent)+1; ++linelen) {
            printf(" ") >outfile
            if (debug) printf(" ") >co
         }
      }
      printf( " %s",  flist[i] ) >outfile
      if (debug) printf( " %s",  flist[i] ) >co
      linelen += length(flist[i]) + 1
   }
   # Build the link command line's only parameter: the filename
   #  depending on options specified in lcf command-line param
   link_comd_filename = sprintf( "%s%s%s",
                                 lcf ~ /[Pp]/? exefile["path"] : "",
                                 lcf ~ /[Nn]/? exefile["name"] : "",
                                 lcf ~ /[Ee]/? exefile["ext"]  : "" );
   printf( "\n\t%s %s\n\n", lc, link_comd_filename) >outfile
   if (debug) printf( "\n\t%s %s\n\n", lc, link_comd_filename) >co

}
#
function look_for_source_file( name,    # File name: without path nor extension
                               objname, # Object file's full name (for batch line)
                               srcname, # Local var: complete source file name
                               lang)    # Local var: loop index
#  Looks for source file name, using dir path/extension for each language
#  Generates a batch file dependency generator line for proper language
#  if file name found; otherwise makes a __NOFILE__ line for language #1
{
   if (debug>2) 
      print "look_for_source_file(name=" name ", objname=" objname ")" >co
   for (lang in ext) { # For each language supported
      # Build source file name
      #  using this language's directory pathname and extension
      srcname = path[lang] name ext[lang]
      if (debug>2) 
         print "look_for_source_file(), lang #" lang ": srcname = " srcname >co
      # See if the file exists, by reading 1st line of it
      # (getline returns -1 on error.. assume that means file not found)
      if (getline <srcname != -1) { # caution.. getline fills $0, $1 ...
         # File found.. Build the normal batch file line
         printf( "%s %s %s ", comdpre, depgen[lang], srcname)
         printf( "incpath=%s objname=%s ", ipath[lang], objname)
         printf( "cc=%s ccf=%s ", cc[lang], ccf[lang] )
         printf( "%s %s %s\n", comdmid, outfile, comdpost)
         if (debug) printf( "%s %s %s ", comdpre, depgen[lang], srcname) >co
         if (debug) printf( "incpath=%s objname=%s ", ipath[lang], objname) >co
         if (debug) printf( "cc=%s ccf=%s ", cc[lang], ccf[lang] ) >co
         if (debug) printf( "%s %s %s\n", comdmid, outfile, comdpost) >co
         close( srcname)
         return
      }
      else {
         if (debug>2)
            print "look_for_source_file(): " srcname " not found." >co
         continue;
      }
   }
   # File not found--all language extensions/paths tried
   print comment "TLR2MAK.AWK Warning: \"" name "\" source file not found."
   print "TLR2MAK.AWK Warning: \"" name "\" source file not found." > co
   print comdpre " " depgen[1] " __NOFILE__ objname=" objname " cc= ccf= " comdmid outfile comdpost
}
#
function get_cl_param( parname,  # Which parameter ya lookin' fer? (e.g. "blah=")
                       optional) # Is it an optional parameter?
# Search the ARGV[] array for a command-line parameter beginning w/ parname
# If optional is 0, aborts program if not found
# Returns the string to the right of the '='
{
   for (i in ARGV)
      if (match(ARGV[i],parname))
         return substr(ARGV[i],length(parname)+1)

   if (optional)
      return ""

   printf( "tlr2mak.awk usage error:  Command-line parameter '%s' missing\n",
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

