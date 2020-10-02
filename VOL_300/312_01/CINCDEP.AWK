#  HEADER:        ;
#  TITLE:         Makefile dependency generator;
#  DATE:          09/28/1989;
#  VERSION:       2.2;
#  DESCRIPTION:   "An AWK program which finds included files and
#                 builds a dependency list.  This dependency list
#                 is a component of a make-file.  The list consists
#                 of the source file name followed by a colon and
#                 then all of the files which it includes (and also
#                 those which are included by the included files, &c.)
#                 The last item is a compile-command.";
#
#  KEYWORDS:      Makefile, make, include, dependency generator;
#  SYSTEM:        MS-DOS, UNIX;
#  WARNINGS:      "Doesn't handle directory names embedded within
#                 #include <here>.  Doesn't handle preprocessor #if.. blocks
#                 or #include statements which are "commented out."
#                 Only handles a single include-path.  
#                 PolyAwk will generate false matches to lines
#                 beginning with non-ASCII (i.e. 128..256) chars."
#                 Filenames are case sensitive.";
#  FILENAME:      CINCDEP.AWK;
#  SEE-ALSO:      TLR2MAK.AWK, CF2MAK.AWK, AINCDEP.AWK, PINCDEP.AWK;
#  AUTHORS:       James Yehle;
#  COMPILERS:     PolyAWK, Mortice Kern AWK, Rob Duff's PC-AWK 2.0,
#                 In any case, must be 1985 awk (not 1977);
#
# ============================================================================
#
# cincdep.awk         Last modified  Sep 28, 1989  22:23
#
#   Scans a C file for all "#include" dependencies
#   This scanning process involves all nested include files, too.
#
#  Jim Yehle  753 Left Fork Rd. #B/Sugarloaf Star Route/Boulder, CO 80302 9252
# . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
#
#   Usage is:
#     awk -f [path]cincdep.awk [path]src_file.c objname=[path]filename
#        cc=compile_comd ccf=[p][n][e] [incpath=[pathname]] [>[path]outfile]
#     Pathnames must contain trailing separator, e.g. "c:\inc\"
#     Order of command-line parameters (save for "-f ..." and srcfile.c)
#      is irrelevant, but identifiers must be in lower case.
#     "src_file.c" may be replaced by special "__NOFILE__"
#     cc is the compile command (no spaces allowed)
#     ccf controls which components of the source file's name get
#         included in the compile command line: p = directory path,
#         n = name, e = extension.
#
#   Output is:
#     [path]src_file.obj: [path]src_file.c [path]included_file1 linecont
#                         [path]included_file2 [path]included_file3 ...
#     <\t>comd [path]src_file[extenstion]
#     <\n>
#   For __NOFILE__, output is:
#     [path]src_file.obj:
#     <\n>
#     <\n>
# . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
#
# Known limitations and anomalies:
# - Assumes that no directory paths are within the "file" or <file> strings
# - Doesn't make any attempt to switch out #if-#endif blocks.
#    I don't see this as a serious problem.  (The solution certainly isn't
#    trivial.)  It will mean only that your make-file may turn out to be a
#    bit more "paranoid," including more dependencies than really exist
#    if an #include directive is #iffed out.  It could be a problem if 
#    nonexistent include-files appear in a make-file's dependency, and your
#    make utility barfs when it doesn't find a file.  
# - The same problem can be created by "commenting out" a line containing
#    an include directive, such as:
#       /*
#       #include <stdio.h>
#       */
# - File names are case- and directory-sensitive.  Huh?  If a file includes
#    both blah.h and Blah.h, they'll both appear in the dependency list.
#    Furthermore, if it includes "splunge.h" and <splunge.h>, and an instance
#    appears in both the default directory and the include-file directory,
#    you'll get two occurrences in the dependency list, one with the 
#    directory pathname tacked on the front and the other without.
# - So far it has been tested with Polytron and MKS awk interpreters, and
#    Turbo C's make utility, all running under the MS-DOS operating system.
# - Your ability to specify complex compilation commands is really limited.
#    There's also no way to set source-file-specific command-line options.
# - I don't know what will happen if your source file doesn't end with .c
#    and your object files don't end with .obj.  Try it!
# - Only one include-path is supported.  For the form #include <file>, cincdep
#    searches the include-path first, then the current directory for the
#    given file.  If your file says #include "file", the order is reversed.
#
# . . . Revision history . . . . . . . . . . . . . . . . . . . . . . . . . . .
#
#        Sep 28  89            -- Released for C User's Group distribution --
#  2.2   Sep 10  89       JRY  Added legal-filename-char regular expression
#  2.1   Aug 17  89       JRY  Added add_fnexist option (as internal directive)
#  2.0   Aug 10  89       JRY  If an included file can't be found, cincdep now
#                               beeps and prints an error to CO, then continues
#                               without scanning the file for nested inclusions.
#                               (It adds it to the dependency list, without a
#                               directory path on the front of the filename.)
#                               Also added an optimization:  if addfile() sees
#                               that a file is already in the list, then it has
#                               already been scanned, so it isn't done again.
#                               Added "verbose" CO output as debug level 1.
#  1.7   Jun 20  89       JRY  Added "ccf=" command-line parameter
#  1.6   Jun 7   1989     JRY  Added include-file pathnames to output,
#                               added objname= & cc= command-line parameters
#  1.5   May 31  1989     JRY  Added primary source file at start of
#                               dependency list
#  1.4  Cinco di Mayo 89  JRY  Cleaned output-line-too-long test
#  1.3   Apr 21  1989     JRY  Added line-too-long continuation on next line,
#                               translation of primary file from .c to .obj.
#  1.2   Feb 22  1989     JRY  Added incpath command-line option
#  1.1   Feb 21  1989     JRY  Re-wrote to handle recursion internally.
#  1.0   Feb 15  1989     JRY  Initial creation & debugging.. algorithm used
#                               recursive invocations of awk via system(awk)
#                               function, but ran out of memory (640k DOS)
#                               after three levels.
# ============================================================================
#

BEGIN {
   co = "CON"       # Console-out: MS-DOS "CON", iRMX ":co:", unix "/dev/tty"
   linecont = " \\" # Line-continuation EOL char: snake " /", unix make "\\"
   debug = 0        # 0=off, 1=verbose, 2=main-level debug, 3= adds functions
   use_fnexist = 1  # Add nonexistent header files to dependency list?
   fname_chars = "[^\\\\ :]" # Any single legal char in a file name.
   #  (Must exclude directory path separator character! (DOS \, UNIX /) )

   printf( "cincdep.awk  C #include dependency scanner  v 2.2") > co
   objfile = get_cl_param( "objname=", 0)
   if (debug>1) printf( "objfile = '%s'\n", objfile) > co
   cc = get_cl_param( "cc=", 0)
   if (debug>1) printf( "cc = '%s'\n", cc) > co
   ccf = get_cl_param( "ccf=", 0)
   if (debug>1) printf( "ccf = '%s'\n", ccf) > co
   # Omitting "incpath=xx" ok.. assume default directory for #include files
   incpath = get_cl_param( "incpath=", 1)
   if (debug>1) printf( "incpath = '%s'\n", incpath) > co
   if (ARGV[1] == "__NOFILE__") {
      if (debug)                    
         printf( "\n__NOFILE__: Empty dependency list generated.\n") > co
      else
         printf( "  (of __NOFILE__)\n") > co
      exit # Hop to END; srcfile is still an empty string & flist is empty
   }
   # 1st in list of ARGV[]'s is source file (FILENAME not set in BEGIN section)
   fullsrcfile = ARGV[1]
   # Split primary source file name into Path, Name, and Extension components
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
# Parser must deal with these abberations on the assumed form '#include "file".'
#  a) '# include "file"'            c) '#include <file>'
#  b) '#include"file"'              d) '#include "file"/* .. */'
#                                   e) ' #include "file"'
/^[ \t]*#[ \t]*include[ \t]*["<]/ {
   if (debug>1) printf( "include: NR=%d, $0='%s', $1='%s'\n", NR, $0, $1) >co
   incpath_first = extract_filename()  # into $1
   if (debug>1) printf( " extracted filename = '%s'\n", $1) > co
   # Check for nested inclusions by scanning included file for #include's
   scanfile( $1, incpath, incpath_first)
}

# Produce the dependency list printout
END {
   # Print primary (object) file name before list (left of colon),
   # Print primary source file name (*.c) as first item in list.
   printf( "%s: %s", objfile, fullsrcfile )
   if (debug) printf( "%s: %s", objfile, fullsrcfile) >co
   linelen = length(objfile) + 2 + length(fullsrcfile)

   for (i=1; i in flist; i++) {
      # If line will be too long, put out a line-continuation char and newline
      if (linelen + 1 + length(flist[i]) + length(linecont)  > 79) {
         printf( "%s\n", linecont)
         if (debug) printf( "%s\n", linecont) >co
         # On the new line, tab out to underneath the first included file name
         for (linelen=0; linelen<length(objfile)+1; ++linelen) {
            printf(" ")
            if (debug) printf(" ") >co
         }
      }
      printf( " %s",  flist[i] )
      if (debug) printf( " %s",  flist[i] ) >co
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
   if (debug) printf( "\n\t%s %s\n\n", cc, compile_comd_filename) >co
}
#
function scanfile( filename, incpath, incpath_first, # <- parameters
                   glr, fullname1, fullname2) {      # <- local variables
   # Looks for filename in directory incpath if incpath_first TRUE,
   #  otherwise looks in default (current) directory path first.
   # glr is a local variable: getline return value.
   # fullname1, 2 are a local vars: path, filename glued together.
   #  1 is first file to try, 2 contains other dir path option
   #  If fullname1 not found but fullname2 is, fullname2 copied over fullname1
   if (incpath_first) {
      fullname1 = incpath filename;
      fullname2 = filename
   }
   else {
      fullname1 = filename; 
      fullname2 = incpath filename
   }
   glr = getline <fullname1
   if (glr == -1) { # Error opening file.. assume doesn't exist
      # Try looking in the second choice of dir paths
      glr = getline <fullname2
      if (glr == -1) { # Error opening file.. assume doesn't exist
         # Can't find filename in either directory path option
         printf( "\007CINCDEP.AWK error:  cannot open file %s", fullname1) >co
         if (incpath != "") printf( " nor %s\n", fullname2) >co
         else               printf( "\n")                   >co
         if (use_fnexist) # Put it in list anyway, if you say so
            addfile( filename,  flist )
         return # Can't scan this file
      }
      fullname1 = fullname2
   }
   # File has been found (its name is in fullname1) and opened.
   # Add it to list of included files; if already there, don't scan it again.
   already = addfile( fullname1, flist)
   if (already)
      return
   while (glr == 1) {
      if ( $0 ~ /^[ \t]*#[ \t]*include[ \t]*["<]/ ) {
         incpath_first = extract_filename()
         scanfile( $1, incpath, incpath_first)
      }
      glr = getline <fullname1
   }
   close( fullname1)
}
#
function extract_filename(    retval) {
   # Takes $0 as input; produces extracted filename in $1
   # Returns TRUE if angle brackets (i.e. <file>) were used,
   # 0 if "file" form was used.
   # retval is a local variable: the return value.
   if (debug>2)
      printf( " extract_filename() entered: $0='%s', $1='%s'\n", $0, $1) > co
   # Delete "#include" from $0; forces re-parsing, making $1 into
   # (quoted) filename.  This solves a), b) and e).
   gsub( /\t/, " "); # Replace all tabs w/SPs (field separator FS is space)
   sub(/ *# *include */, "")
   retval = (index($1,"<")==1) # 1 for <file>, 0 for "file"
   if (debug>2)
      printf( "  extract_filename(): incpath_first = %s\n",
                   retval? "TRUE":"FALSE")                       > co
   # Replace quote chars w/SPs to force what's after into $2; solves case d).
   gsub( /[\"<>]/, " ")
   if (debug>2)
      printf( "  extract_filename() nuked [\"<>] -> '%s'\n", $1) > co
   # Filename, stripped of either type of quote char, is now $1
   return retval
}

function addfile( filename, flist,    flix) {
   # filename is a string to be added to flist[]
   # flist[] is an array of strings to which addfile() appends a file name
   # flix is a (local) index into flist[]
   # Returns 1 if filename is already in list, 0 if it's a new entry.
   for (flix=1; flix in flist; flix++)
      if (flist[flix] == filename) # Already in list--
         return 1                  #  don't add a duplicate
   flist[flix] = filename
   return 0
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

   printf("cincdep.awk usage error:  Command-line parameter '%s' missing\n",
          parname                                                          ) >co
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

