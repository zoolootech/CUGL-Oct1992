#  HEADER:        ;
#  TITLE:         Intel Librarian input-file object file extractor
#  DATE:          09/28/1989;
#  VERSION:       1.1;
#  DESCRIPTION:   "An AWK program which scans a librarian input-file
#                 and builds a batch file which will in turn
#                 produce a make-file.  Requires the services of a
#                 source-file dependency generator (such as cincdep)."
#  KEYWORDS:      Makefile, make, include, dependency generator;
#  SYSTEM:        MS-DOS;
#  WARNINGS:      "PolyAwk will generate false matches to lines
#                 beginning with non-ASCII (i.e. 128..256) chars."
#  FILENAME:      LBI2MAK.AWK;
#  SEE-ALSO:      CINCDEP.AWK, TLR2MAK.AWK, AINCDEP.AWK, PINCDEP.AWK;
#  AUTHORS:       James Yehle;
#  COMPILERS:     PolyAWK, Mortice Kern AWK, Rob Duff's PC-AWK 2.0;
#-----------------------------------------------------------------------------
#
#  Lbi2mak.awk              Last modified  Sep 28, 1989  22:22
#
#     Scans an Intel library make (from scratch) input file to produce
#     to produce a batch file for use with dependency generator(s)
#     to build a set of make-file dependency lists (and eventually a make-file)
#
#  Jim Yehle
#
# . . . Revision history . . . . . . . . . . . . . . . . . . . . . . . . . . .
#
#  1.1  Sep 10 89  JRY  Added legal-filename-char regular expression
#  1.0  Sep 10 89  JRY  Initial creation--from cf2mak.awk v 1.6
# . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
#
#  Usage is:
#    awk -f lbi2mak.awk filename.lbi outfile=[path]filename
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
#  Takes an Intel librarian input file as input.
#  Generates an executable-dependency list at the start of outfile,
#  followed by the link command and the primary (executable) file name.
#  Extracts all object files which are to be added to the library.
#  Generates a batch file line, for each object file found, of the form:
#   comdpre srcfile "incpath="incpath "objname="file.obj
#     "cc="cc "ccf="ccf comdmid outfile comdpost
#  unless the source file can't be found, in which case it makes this batch file line:
#   comdpre "__NOFILE__ objname="file.obj "cc= ccf= " comdmid outfile comdpost
#  The search for source files covers C, assembly, and PL/M path and extensions.
#
#  Assumes object file names contain only these chars: [^\\]*.[Oo][Bb][Jj]
#  Assumes FILENAME ends with .LBI or .lbi and that primary target (library)
#   file name is made by replacing .LBI with exe_ext.
#
#-----------------------------------------------------------------------------
#
BEGIN {
   # Set various strings used in building batch-file command lines
   # (these should probably get passed in as lbi2mak command-line arguments)

   #  Lbi2mak supports several languages.  Each language must have
   #   1) a source file name extension (to replace ".obj" in searches)
   #   2) a source directory pathname (stuck on front of file name during search)
   #   3) an include-file directory whose name to pass to a dependency scanner
   #   4) a compiler command to pass to dependency scanner (no spaces allowed)
   #   5) controls for filename pathname/extension inclusion in compile command line
   #   6) a dependency generator awk program

# ------------- User-customized section ------------------------------
   # Language 1 is C
   ext[1] = ".c"
   path[1]    = "s:\\c\\"
   ipath[1]   = "s:\\c\\inc\\"
   cc[1] = "ccd"
   ccf[1] = "n"
   depgen[1] = "s:\\awk\\cincdep.awk"
   # Language 2 is PL/M
   ext[2] = ".plm"
   path[2]  = "s:\\plm\\"
   ipath[2] = "s:\\plm\\inc\\"
   cc[2] = "plmld"
   ccf[2] = "n"
   depgen[2] = "s:\\awk\\pincdep.awk"
   # Language 3 is assembly
   ext[3] = ".a28"
   path[3]  = "s:\\asm\\"
   ipath[3] = "s:\\asm\\inc\\"
   cc[3] = "asm"
   ccf[3] = "n"
   depgen[3] = "s:\\awk\\aincdep.awk"

   #    allpre      Leading line(s) of entire batch file
   #    allpost     Trailing line(s) of entire batch file
   #    comment     Comment-line lead-in: DOS "REM" or "ECHO", iRMX ";"
   #    comdpre     Command line prefix
   #    comdmid     Command line midle: between source & output file names
   #    comdpost    Command line suffix
   #    exe_ext     Executable file extension (e.g. ".exe" or ".com" for DOS)
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
   exe_ext = ".LIB" 
   linecont = " \\"
   fname_chars = "[^\\\\ :]" # Unix "[^/ ]"; used to use "[A-Za-z0-9_]"
# ------------ End of user-customized section ------------------------
#
   co = "CON"   # Console-out: MS-DOS "CON", iRMX ":co:", unix "/dev/tty"
   debug = 0    # 0=off, 1=verbose, 2=main-level debug, 3= adds functions

   print "lbi2mak.awk  Library input-file object-file extractor  v 1.1" > co

   outfile = get_cl_param( "outfile=", 0)
   lc = get_cl_param( "lc=", 0)
   lcf = get_cl_param( "lcf=", 0)

   print allpre
   if (debug) print allpre >co

}
#
$0 ~ /^[ \t]*[Aa][Dd][Dd][ \t]+/  ||  $0 ~ /^[ \t]*[Aa][ \t]+/ {
   # Object file name has been extracted (into $2)
   fullobjname = $2
   for (flix=1; flix in flist; flix++)
      ;
   flist[flix] = fullobjname # Add to array
   if (debug>1) print "NR " NR ", fullobjname = " fullobjname >co

   # Split object file name into Path, Name and Extension components
   split_filename( fullobjname, objnameparts, fname_chars)
   if (debug>1) print "objnameparts[\"name\"] = " objnameparts["name"] >co

   # Generate one batch file line
   look_for_source_file( objnameparts["name"], fullobjname)
}
#
END {
   print allpost
   if (debug) print allpost >co

   # Generate executable-file dependency list (into outfile)
   # Split primary link-configuration file name into Path, Name, and Extension 
   split_filename( FILENAME, exefile, fname_chars)
   exefile["ext"] = exe_ext  # Change it from ".lbi"
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
                               lang     # Local var: loop index
)
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
         if (debug) printf( "%s %s %s ", comdpre, depgen[lang], srcname) >co
         printf( "incpath=%s objname=%s ", ipath[lang], objname)
         if (debug) printf( "incpath=%s objname=%s ", ipath[lang], objname) >co
         printf( "cc=%s ccf=%s ", cc[lang], ccf[lang] )
         if (debug) printf( "cc=%s ccf=%s ", cc[lang], ccf[lang] ) >co
         printf( "%s %s %s\n", comdmid, outfile, comdpost)
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
   print comment "LBI2MAK.AWK Warning: \"" name "\" source file not found."
   print "LBI2MAK.AWK Warning: \"" name "\" source file not found." > co
   print comdpre " " depgen[1] " __NOFILE__ objname=" objname " cc= ccf= " comdmid outfile comdpost
   if (debug) print comdpre " " depgen[1] " __NOFILE__ objname=" objname " cc= ccf= " comdmid outfile comdpost >co
   close( srcname) # just in case it was opened
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

   printf( "lbi2mak.awk usage error:  Command-line parameter '%s' missing\n",
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


