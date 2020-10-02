/****************************************************************************
*                                WHERE                                      *
*                                                                           *
*  WHERE is a program to locate files on a disk.  It requires DOS 2.x or    *
*  above.  It will search through all subdirectories of the disk looking    *
*  for files which match information coming from the command line.          *
*                                                                           *
*  The command line syntax is:                                              *
*                                                                           *
*     WHERE [ starting directory ] filename.ext [p] (for file prompting)    *
*                                                                           *
*     Rule: if a directory name is the last item in the path name, then     *
*           the directory name MUST be followed by a backslash or else it   *
*           will be considered a filespec.                                  *
*                                                                           *
*  This program originally came from the Oct. 85 issue of PC Tech Journal.  *
*  While it was written originally for the Mark Williams compiler, this     *
*  version is for the Microsoft C compiler, vsn 3.0 or later.               *
*                                                                           *
*  George Defenbaugh,  918-622-7926,  10034 E. 29th St.  Tulsa, OK  74129   *
****************************************************************************/

/****************************************************************************
*  C header files                                                           *
*  These identify library routines.  LINT_ARGS must be defined first.       *
****************************************************************************/

#define LINT_ARGS = 1      /* value doesn't matter, only definition does   */

#include <conio.h>         /* for console i/o                              */
#include <dos.h>           /* for DOS interrupt calls                      */
#include <stdio.h>         /* for standard i/o                             */
#include <string.h>        /* for string functions                         */

union REGS inregs,outregs; /* register structure for DOS calls             */

/****************************************************************************
*  Structure for MS-DOS date and time fields returned by DOS functions      *
*  See DOS Tech Reference for more information                              *
****************************************************************************/

struct msdos_date
{
   unsigned ms_sec      : 5; /* time in 2 sec. int (5 bits)             */
   unsigned ms_min      : 6; /* minutes (6 bits)                        */
   unsigned ms_hour     : 5; /* hours (5 bits)                          */
   unsigned ms_day      : 5; /* day of the month (5 bits)               */
   unsigned ms_month    : 4; /* month (4 bits)                          */
   unsigned ms_year     : 7; /* year since 1980 (7 bits)                */
};

/****************************************************************************
*  Define the DOS Disk Transfer Area (DTA).  The structure will be filled   *
*  in by DOS for interrupt 21H calls.  See DOS Tech Ref for more info       *
****************************************************************************/

struct DTA
{
   char     DTA_dosinfo[21];      /* reserved for DOS use               */
   char     DTA_attr;             /* file attribute byte                */
   struct   msdos_date DTA_date;  /* date structure from above          */
   long     DTA_size;             /* file size                          */
   char     DTA_filename[13];     /* file name (w/o path)               */
};

/****************************************************************************
*  Macros                                                                   *
****************************************************************************/

/* This macro returns the number of elements of its array argument         */

#define SIZE(x)     (sizeof(x)/sizeof(x[0]))

/****************************************************************************
*  Definition of constants                                                  *
****************************************************************************/

#define not !                 /* C normally uses the ! for NOT             */
#define and &                 /* C normally uses the & for AND             */
#define lower_half_reg 0x00ff /* mask for anding into lower register half  */
#define carry_set      0x0001 /* mask for flag register carry bit          */
#define no_type        0x06   /* gets system and hidden files too          */
#define directory_type 0x16   /* directory bit on, with system and hidden  */
#define directory_only 0x10   /* directory bit only is on                  */
#define no_more_files  18     /* DOS return code for no more files         */
#define full_screen    23     /* max number of lines on a screen           */
#define end_of_string  '\0'   /* C uses a binary zero for end of string    */
#define backslash      '\\'   /* The backslash character                   */
#define colon          '\:'   /* The colon character                       */
#define stop           'n'    /* Used with the continue_pgm switch         */
#define no             'n'    /* Used with the user_want_prompt switch     */
#define continue       'y'    /* Used with the continue_pgm switch         */
#define yes            'y'    /* Used with the user_want_prompt switch     */
#define a_ronly        0x01   /* Read only file                            */
#define a_hidden       0x02   /* Hidden file                               */
#define a_system       0x04   /* System file                               */
#define a_directory    0x10   /* Directory                                 */
#define a_archive      0x20   /* Archive bit                               */
#define n_directory    'd'    /* switch value if directories are prompted  */
#define n_ronly        'r'    /* Read only file                            */
#define n_write        'w'    /* Write allowed                             */
#define n_hidden       'h'    /* Hidden file                               */
#define n_visible      'v'    /* Visible file                              */
#define n_system       's'    /* System file                               */
#define n_user         'u'    /* User file                                 */
#define n_archive      'a'    /* Archive bit                               */
#define n_not_archive  'n'    /* Not archived                              */

/****************************************************************************
*  The next structure and array are for analyzing the attribute bits for    *
*     each file.  The structure consists of pairs of integers, the first    *
*     integer in each pair is a bit mask which has the bit set that         *
*     corresponds to a particular attribute for a file.  The second integer *
*     is a character that is displayed if the attribute byte has the bit    *
*     set that matches the mask.  The attribute bits are:                   *
*                                                                           *
*           a - archive bit                                                 *
*           d - directory                                                   *
*           h - hidden file                                                 *
*           r - read only file                                              *
*           s - system file                                                 *
****************************************************************************/

static  struct atr {          /* Attribute structure                       */
                    char a_mask;
                    char a_name;
                   } 
atr[] = {
         a_archive,   n_archive,
         a_directory, n_directory,
         a_system,    n_system,
         a_hidden,    n_hidden,
         a_ronly,     n_ronly
        };

char *time_of_day[2] = {"AM","PM"};

/****************************************************************************
*  Define the type "filename" to be a string of 51 characters               *
*    This type is used in other definitions                                 *
****************************************************************************/

typedef char filename[51];

/****************************************************************************
*  The following filename strings are used in the program                   *
*                                                                           *
*     check_string               filename to be searched for from the       *
*                                   command line                            *
*     directory_string           directory name to be searched              *
*     newdirectory_string        directory name to be searched on next      *
*                                   recursive call                          *
*     current_string             temporary string for searching in a        *
*                                specific directory                         *
*                                                                           *
****************************************************************************/

/****************************************************************************
*  Definition of any forward-referencing functions                          *
****************************************************************************/

char *DATE();

/****************************************************************************
*  Global variables                                                         *
****************************************************************************/

filename check_string;        /* this string "remembers" user input        */
char     datestring[40];      /* print output string for dates             */
char     continue_pgm = continue;  /* switch to stop program               */
char     continue_dir = yes;       /* continue this directory switch       */
char     user_want_prompt = no;    /* switch to allow deletion of files    */
int      nbr_lines_on_screen = 0; /* counts nbr lines already printed      */
char     attr_mask_switch = 0x00; /* for selection by attribute            */
char     *first_switch;           /* addr of first command line switch     */
char     *first_switch_copy;      /* copy first command line switch addr   */

/****************************************************************************
*  MAIN() - Beginning of the code                                           *
****************************************************************************/

main(argc,argv)

int argc;
char *argv[];                     /* could also be: char **argv;           */

{
   filename  directory_string;    /* directory to be searched              */
   char      *incoming_filename;  /* addr of filename in command line arg  */
   char      *last_backslash;     /* addr of last backslash in cmd line    */
   char      *last_colon;         /* addr of last colon command line       */
   char      *incoming_string;    /* addr of first command line argument   */
   int       last_directory_char; /* last character in directory string    */
   register  int  i;              /* Sub to use with attribute structure   */

   /*************************************************************************
   *  Example:  The following argument will start on the A: drive.  The     *
   *            BRIEF directory in the root will be searched and all files  *
   *            and sub-directories in BRIEF will be displayed.             *
   *                                                                        *
   *     A:\BRIEF\*.*                                                       *
   *     ^^      ^^                                                         *
   *     ||      || *incoming_filename                                      *
   *     ||      | *last_backslash                                          *
   *     || *last_colon                                                     *
   *     | *incoming_string                                                 *
   *                                                                        *
   *              *.*                                                       *
   *              ^                                                         *
   *              | check_string                                            *
   *                                                                        *
   *     A:\BRIEF\*.*                                                       *
   *     ^        ^                                                         *
   *     |        | last_directory_character (a subscript)                  *
   *     |        | \0 (null)                                               *
   *     | directory_string                                                 *
   *************************************************************************/

   /*************************************************************************
   *  Check number of incoming arguments and if incorrect, do error message *
   *************************************************************************/

   if (argc < 2) {
      printf ("Syntax is: WHERE [starting drive:\path]filename.ext [switches]\n");
      printf ("           Switchs (in any order) mean:\n");
      printf ("             p - stop and prompt at each file or directory\n");
      printf ("             d - directories are prompted too\n");
      printf ("             a - stop on modified files\n");
      printf ("             s - stop on system files\n");
      printf ("             h - stop on hidden files\n");
      printf ("             r - stop on read only files\n");
   }
   else {
      /*******************************************************************
      *  Incoming_string is set to the first argument in the command line*
      *  which should be the file spec.                                  *
      *  The incoming_string is then searched for the last occurrence of *
      *  a backslash to find the end of the directory name.              *
      *  Argv points to the array of ptrs, each pointer pointing to a    *
      *  separate command line argument.  *(argv+0) IS the pointer  to   *
      *  the first argument (i.e. the program name WHERE).  *(argv+1) IS *
      *  the pointer to the filespec.  *(argv+2) IS the pointer to the   *
      *  first program switch                                            *
      *******************************************************************/

      incoming_string = *(++argv);           

      if (argc >= 3) {
         first_switch = *(++argv);  /* first_arg points at first switch */
         first_switch_copy = first_switch;  /* need temp pointer        */
         /* if p is a pointer, then strchr("?",p) wont't work, but      */
         /*                         strchr("?",*p) will                 */
         /* convert the first switch to lower case to simplify checking.*/
         /* this second switch would not be needed if a function were   */
         /* used to accomplish this conversion.                         */
         for ( ; *first_switch_copy != '\0' ; first_switch_copy++ )
            *first_switch_copy = tolower(*first_switch_copy);
         if (strchr(first_switch,'p')) {
            user_want_prompt = yes;
         }
         for (i = 0; i < SIZE(atr); i++) {
            if (strchr(first_switch,atr[i].a_name)) {
               attr_mask_switch |= atr[i].a_mask;
            }
         }
      }
      if (attr_mask_switch == 0x00) {
         attr_mask_switch = 0xff;
      }
      last_backslash = strrchr(incoming_string,backslash);
      last_colon     = strchr(incoming_string,colon);

      /*******************************************************************
      *  If there was not a backslash but there was a colon, set the     *
      *    backslash pointer to the colon pointer.                       *
      *    (this reduces the number of further checks needed)            *
      *                                                                  *
      *  If there was not a backslash (and therefore the beginning       *
      *     directory is the root directory)                             *
      *     begin                                                        *
      *        copy command line argument into check_string              *
      *        copy root directory into directory_string                 *
      *     end                                                          *
      *  else                                                            *
      *     (if there was a backslash and therefore a beginning          *
      *      directory specified in the command line)                    *
      *     begin                                                        *
      *        set the incoming_filename to the next character           *
      *           past the backslash                                     *
      *        copy the incoming_filename into check_string              *
      *        copy the command line argument into directory_string      *
      *        terminate directory_string just after the last backslash  *
      *           (therefore leaving only the directory name in the      *
      *            string)                                               *
      *     end                                                          *
      *******************************************************************/

      if ((last_backslash == NULL) && (last_colon != NULL))
         last_backslash = last_colon;

      if ((last_backslash == NULL) && (last_colon == NULL)) {
         /* With no colon or backslash, argument is a filespec             */
         strcpy(check_string,incoming_string);
         strcpy(directory_string,"\\");
      }
      else {
         if (last_backslash < last_colon) {
            /* Here the colon followed the backslash                       */
            printf("\nError: A colon was in the input after a backslash\n");
            continue_pgm = stop;
         }
         else {
            /* Here we had only backslash or colon, or backslash following */
            /*   colon if both were present                                */
            incoming_filename = last_backslash + 1;
            strcpy(check_string,incoming_filename);
            strcpy(directory_string,incoming_string);
            last_directory_char = incoming_filename - incoming_string;
            directory_string[last_directory_char] = end_of_string;
            if (strlen(check_string) == 0)
               strcpy(check_string,"*.*");
         }
      }

      /*******************************************************************
      *  Start up the recursive functions                                *
      *******************************************************************/

      if (continue_pgm == continue) {
         printf ("\nStarting WHERE at \"%s\" looking for \"%s\"\n\n",directory_string,check_string);
         if (user_want_prompt == yes) {
            printf("  Valid responses: D - Delete  C - Change attribute  Q - Quit the program\n");
            printf("                   N - Next directory                                    \n");
            printf("                   Any other key continues to the next file\n\n");
         }
         LOOK(directory_string); /* directory_string ends in a backslash
                                    or else is only a drive letter + colon */
      }
   }
   return;
}

/****************************************************************************
*  LOOK is the recursive procedure in WHERE that is called once for each    *
*     subdirectory that is found                                            *
*  The argument (directory_string) is an array, and as such its reference   *
*     as an argument causes a pointer to it (i.e. the array) to be          *
*     generated and passed as the argument.                                 *
*  The structure current_DTA is defined in this function, so while here,    *
*     any references to it can be made 'directly', or by name without any   *
*     an indirection operator.  But in functions called by this one, where  *
*     the current_DTA is passed as an argument, references to the structure *
*     must be made as (*current_DTA) or by whatever other name the DTA is   *
*     referred.  See the GET_FILES function as an example.                  *
****************************************************************************/

LOOK(directory_string)

char *directory_string;

{
   struct DTA current_DTA;       /* used to return data from DOS           */
   filename newdirectory_string; /* the directory to be searched on the
                                    next call to LOOK()                    */
   filename current_string;      /* temporary filename string for      
                                    searching the directories              */
   filename chmod_dir_string;    /* temporary filename string for      
                                    changing directory attribute           */
   register  int  i;             /* Sub to use with attribute structure    */
   char user_response;           /* holds keyboard response to prompt      */
   char *last_dir_backslash;     /* ptr to last backslash in
                                    chmod_dir_string                       */

   /*************************************************************************
   *  Form current_string by copying directory_string and then              *
   *     concatenating "*.*" to look through all files                      *
   *  First we look through all directories, then when there are no more    *
   *     directories the files are scanned.                                 *
   *************************************************************************/

   strcpy(current_string,directory_string);
   strcat(current_string,"*.*");

   /*************************************************************************
   *  Set the Disk Transfer Area (DTA) in DOS to the current_DTA            *
   *     structure                                                          *
   *  Get the first subdirectory in this directory                          *
   *************************************************************************/

   SET_DTA(&current_DTA);

   GET_FIRST(current_string,directory_type);

   /*************************************************************************
   *  While there are more subdirectories in this directory, and we are     *
   *     to continue                                                        *
   *     begin                                                              *
   *        double check for proper directories (see text of article)       *
   *        if a directory                                                  *
   *           begin                                                        *
   *              set up the newdirectory_string for the next call to       *
   *                 LOOK (see text)                                        *
   *              call LOOK                                                 *
   *              reset Disk Transfer Address (see text of article)         *
   *           end                                                          *
   *        look for next directory                                         *
   *     end                                                                *
   *************************************************************************/

   while ((not(outregs.x.cflag))     &&
          (continue_dir == continue) &&
          (continue_pgm == continue)   ) {
      if ( (current_DTA.DTA_attr & directory_only) &&
           (current_DTA.DTA_filename[0] != '.'   )   ) {
         strcpy(newdirectory_string,directory_string);
         strcat(newdirectory_string,current_DTA.DTA_filename);
         strcat(newdirectory_string,"\\");
         if (strchr(first_switch,n_directory)) {
            for (i = 0; i < SIZE(atr); i++) {
               if (atr[i].a_mask & current_DTA.DTA_attr)
                  putchar(atr[i].a_name);
               else
                  putchar('-');
            }
            printf("            %s  %s%s\n",
               DATE(&(current_DTA.DTA_date)), directory_string,
               current_DTA.DTA_filename);
            if (user_want_prompt == no) {
               ++nbr_lines_on_screen;
               if (nbr_lines_on_screen >= full_screen) {
                  nbr_lines_on_screen = 0;
                  printf("Press any key to continue...\n");
                  getch();
               }
            }
            else {
               user_response = getch();
               if (strchr("Qq",user_response))
                  continue_pgm = stop;
               else {
                  if (strchr("Nn",user_response))
                     continue_dir = stop;  /*
                  else {
                     if (strchr("Cc",user_response)) {
                        printf("Directory string is: %s\n",directory_string);
                        strcpy(chmod_dir_string,directory_string);
                        strcat(chmod_dir_string,current_DTA.DTA_filename);
                        last_dir_backslash = strrchr(chmod_dir_string,backslash);
                        *last_dir_backslash = end_of_string;
                        CHANGE_FILE_ATTRIBUTE(chmod_dir_string,current_DTA.DTA_attr);
                     }
                  }  */
               }
            }
         }
         if (continue_pgm == continue) {
            if (continue_dir == continue) {
               LOOK(newdirectory_string);
               SET_DTA(&current_DTA);
               continue_dir = continue;
            }
            else
               continue_dir = continue;
         }
         else {
            outregs.x.cflag = carry_set;
            outregs.x.ax    = no_more_files;
         }
      }
      GET_NEXT();
   }

   /*************************************************************************
   *  If there are no more subdirectories in this directory                 *
   *     look for files                                                     *
   *  else                                                                  *
   *     print an error message                                             *
   *************************************************************************/

      if (continue_pgm == continue) {
         if (outregs.x.ax == no_more_files)
            GET_FILES(directory_string,&current_DTA);
         else
            printf("Problem with looking thru %s\n",directory_string);
      }

   return;
}

/****************************************************************************
*  GET_FILES is called once per directory to look for the actual files      *
*     the search string                                                     *
****************************************************************************/

GET_FILES(directory_string,current_DTA)

char *directory_string;
struct DTA *current_DTA;

{
   filename current_string;      /* temporary filename string for      
                                    searching the directories              */
   filename asciiz_file_name;    /* holds fully qualified asciiz name      */
   char user_response;           /* holds keyboard response to prompt      */
   char del_response;            /* holds keyboard response to del msg     */
   register  int  i;             /* Sub to use with attribute structure    */
   long nbr_bytes_this_dir = 0;  /* Sub to use with attribute structure    */

   /*************************************************************************
   *  Form current_string by copying directory_string into it and then      *
   *     concatenating the check_string onto the end                        *
   *************************************************************************/


   strcpy(current_string,directory_string);
   strcat(current_string,check_string);
   continue_dir = continue;

   /*************************************************************************
   *  Get the first file that matches current_string                        *
   *************************************************************************/

   GET_FIRST(current_string,no_type);

   /*************************************************************************
   *  While there are more files that match the search string;              *
   *     begin                                                              *
   *        print the file information                                      *
   *        handle prompting if requested                                   *
   *        get the next file if not directed to stop                       *
   *     end                                                                *
   *************************************************************************/

   while ( (not(outregs.x.cflag))) {

      /* (*current_DTA).DTA_filename needs the parens because the . binds  */
      /* more tightly than the * does.  It can also be written as          */
      /* current_DTA -> DTA_filename, which says that 'current_DTA' is a   */
      /* pointer to the structure in which DTA_filename is a member.       */

      /* Loop for each possible attribute bit, printing its symbol if the  */
      /* bit is set, otherwise printing a '-'.                             */

      if ( (attr_mask_switch & (*current_DTA).DTA_attr) ||
           (attr_mask_switch == 0xff)                   || 
           (attr_mask_switch == a_directory) ) {
         for (i = 0; i < SIZE(atr); i++) {
            if (atr[i].a_mask & (*current_DTA).DTA_attr)
               putchar(atr[i].a_name);
            else
               putchar('-');
         }

         nbr_bytes_this_dir += (*current_DTA).DTA_size;
         printf("%10ld  %s  %s%s\n", (*current_DTA).DTA_size,
            DATE(&((*current_DTA).DTA_date)), directory_string,
            (*current_DTA).DTA_filename);

         if (user_want_prompt == no) {
            ++nbr_lines_on_screen;
            if (nbr_lines_on_screen >= full_screen) {
               nbr_lines_on_screen = 0;
               printf("Press any key to continue...\n");
               getch();
            }
         }
         else {
            user_response = getch();
            if (strchr("Dd",user_response)) {
               strcpy(asciiz_file_name,directory_string);
               strcat(asciiz_file_name,(*current_DTA).DTA_filename);
               printf("\n      Do you really want to delete %s (Y/N)?\n\n",asciiz_file_name);
               del_response = getch();
               if (strchr("Yy",del_response)) {
                  DELETE_FILE(asciiz_file_name);
                  if (not(outregs.x.cflag))
                     printf("      %s  was DELETED\n\n",asciiz_file_name);
               }
            }
            else {
               if (strchr("Qq",user_response))
                  continue_pgm = stop;
               else {
                  if (strchr("Nn",user_response))
                     continue_dir = stop;
                  else {
                     if (strchr("Cc",user_response)) {
                        strcpy(asciiz_file_name,directory_string);
                        strcat(asciiz_file_name,(*current_DTA).DTA_filename);
                        CHANGE_FILE_ATTRIBUTE(asciiz_file_name,(*current_DTA).DTA_attr);
                     }
                  }
               }
            }
         }
      }
   
      if ((continue_pgm == continue) &&
          (continue_dir == continue)   )
         GET_NEXT();
      else {
         outregs.x.cflag = carry_set;
         outregs.x.ax    = no_more_files;
      }
   }

   /*************************************************************************
   *  If error in looking for a file, print error message and return        *
   *************************************************************************/

   if (outregs.x.ax != no_more_files)
      printf("Problem with looking for %s\n",current_string);
   else {
      if (user_want_prompt == no) {
         if (nbr_lines_on_screen >= full_screen) {
            nbr_lines_on_screen = 0;
            printf("Press any key to continue...\n");
            getch();
         }
      }
      if (strchr(first_switch,n_directory)) {
         ++nbr_lines_on_screen;
         printf("     %10ld  bytes found in directory %s\n",nbr_bytes_this_dir,directory_string);
      }
   }
   return;
}

/****************************************************************************
*  GET_NEXT does an interrupt 21H, function 4FH                             *
****************************************************************************/

GET_NEXT()

{
   inregs.x.ax = 0x4f00;
   intdos(&inregs,&outregs);
   return;
}

/****************************************************************************
*  SET_DTA does an interrupt 21H, function 1AH                              *
*     The DS:DX pair is set to the addr of the current_DTA data structure   *
*     The current_DTA comes in as an address.  Although a structure ref is  *
*     not automatically converted to a pointer (as in the case of an array),*
*     the caller sends the address by SET_DTA(&...).                        *
****************************************************************************/

SET_DTA(current_DTA)

struct DTA *current_DTA;

{
   inregs.x.ax = 0x1a00;
   inregs.x.dx = current_DTA;
   intdos(&inregs,&outregs);
   return;
}

/****************************************************************************
*  GET_FIRST does an intrrupt 21H, function 4EH                             *
*     The CX register is set to either normal or directory type (see text)  *
*     The DS:DX pair irs set to the address of the search string            *
****************************************************************************/

GET_FIRST(search_string,filetype)

char *search_string;
int  filetype;

{
   inregs.x.ax = 0x4e00;
   inregs.x.cx = filetype;
   inregs.x.dx = search_string;
   intdos(&inregs,&outregs);
   return;
}

/****************************************************************************
*  DELETE_FILE does a DOS interrupt 21H, function 41H                       *
*     The DS:DX points to an ASCIIZ string specifying the file which is to  *
*     be deleted.                                                           *
****************************************************************************/

DELETE_FILE(asciiz_file_name)

char *asciiz_file_name;

{
   inregs.x.ax = 0x4100;
   inregs.x.dx = asciiz_file_name;
   intdos(&inregs,&outregs);
   return;
}

/****************************************************************************
*  CHANGE_FILE_ATTRIBUTE does a DOS interrupt 21H, function 43H             *
*     The DS:DX points to an ASCIIZ string specifying the file which is to  *
*     have its attribute changed.  First though, the user is prompted for   *
*     the new attribute to be used.                                         *
*  Trying to change the attribute of a directory returns error code 5,      *
*     access denied.                                                        *
*  The (strchr(attr_response,???)) does not work with ??? equal to a DEFINE *
*     value of '\v', '\a', '\n', or '\r'.  It does work with DEFINE         *
*     values of '\s', '\u', '\h', and '\w'.  Changing them to literals      *
*     solves the problem.                                                   *
****************************************************************************/

CHANGE_FILE_ATTRIBUTE(asciiz_file_name,current_attribute)

char *asciiz_file_name;
char current_attribute;

{
   char attr_response[7];           /* holds keyboard response to prompt   */
   char *p_attr_response;           /* pointer to keyboard response        */

   printf("\n      Please enter up to 4 desired attribute settings:");
   printf("\n        (S)ystem or (U)ser, (H)idden or (V)isible");
   printf("\n        (R)ead only or (W)rite allowed, and");
   printf("\n        (A)rchive bit set or (N)ot to be archived\n\n");

   attr_response[0] = 5;                   /* max nbr input characters,    */
                                           /* including the CR             */
   p_attr_response = cgets(attr_response); /* rtn ptr to start of chars    */
                                           /* convert to lower case        */
   for ( ; *p_attr_response != '\0' ; p_attr_response++ )
      *p_attr_response = tolower(*p_attr_response);
   inregs.x.cx = current_attribute;

   if (strchr(attr_response,n_ronly))
      inregs.x.cx = inregs.x.cx | a_ronly;
   if (strchr(attr_response,n_write))
      inregs.x.cx = inregs.x.cx & 0x00FE;

   if (strchr(attr_response,n_hidden))
      inregs.x.cx = inregs.x.cx | a_hidden;
   if (strchr(attr_response,n_visible))
      inregs.x.cx = inregs.x.cx & 0x00FD;

   if (strchr(attr_response,n_system))
      inregs.x.cx = inregs.x.cx | a_system;
   if (strchr(attr_response,n_user))
      inregs.x.cx = inregs.x.cx & 0x00FB;

   if (strchr(attr_response,n_archive))
      inregs.x.cx = inregs.x.cx | a_archive;
   if (strchr(attr_response,n_not_archive))
      inregs.x.cx = inregs.x.cx & 0x00DF;

   inregs.x.ax = 0x4301;
   inregs.x.dx = asciiz_file_name;
   intdos(&inregs,&outregs);
   if (outregs.x.cflag) {
      printf("Error code from CHMOD was %d\n",outregs.x.ax);
   }
   return;
}

/****************************************************************************
*  DATE takes the date field from the current DTA structure and returns a   *
*     string containing the information in formatted ASCII                  *
****************************************************************************/

char *DATE(dateptr)

struct msdos_date *dateptr;

{
   sprintf(datestring, "%02d-%02d-%2d  %02d:%02d  %s",
      dateptr -> ms_month,  dateptr -> ms_day,
      dateptr -> ms_year+80, (dateptr ->ms_hour)%12,
      dateptr -> ms_min, time_of_day[((dateptr -> ms_hour)/12)]);
   return(datestring);
}
