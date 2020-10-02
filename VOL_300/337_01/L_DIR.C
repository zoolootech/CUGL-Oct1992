/* Copyright (c) James L. Pinson 1990,1991  */

/**********************    L_DIR.C    ***************************/

#include "mydef.h"
#include "stddef.h"
#include "dos.h"
#include "stdio.h"

#define DIR_NAME_LENGTH 13  /* length of directory entry
                               (includes '\0') */
#if defined QUICKC

#include "malloc.h"
#include "memory.h"

#endif

#if defined TURBOC

#include "alloc.h"
#include "mem.h"
#include "string.h"
#include "stdlib.h"
#include "dir.h"

#endif


/*****************************************************************

 Usage:   int dir(char *filespec,char *selection);


  char *filespec=  Wildcard specification for directory
                   ("*.*" ,"*.c") etc.

  char *selection= Pointer to a location suitable for storage of
                   a DIR_NAME_LENGTH character array.  The name of
                   the selected file is copied to this location. If
                   the user presses Escape, no selection is made
                   and *selection = "".

  Returns a 1 if memory is unavailable, otherwise returns a 0.

  Allows list selection of directory files specified by filespec.
  Makes use of the list_select() function. Allows point-and-shoot
  or Speed key selection.   The list is displayed in the currently
  active (top) window.

*****************************************************************/

int dir(char *filespec, char *selection)
{
extern struct screen_structure scr;
extern struct window_structure w[];

int done;               /* flag */
int filecount=file_count(filespec);

#if defined TURBOC
   struct ffblk DTA;
#endif

#if defined QUICKC
   struct find_t DTA;
#endif

char **list;      /* a pointer to a character pointer */

int i,j,number;

/* allocate array */

/* note: we allocate an extra list pointer for the null terminator */
   list =(char **)malloc ((filecount+1)*sizeof(char *));

 if (list==NULL)return(1);

 /* null the list of pointers */
 for(i=0;i<=filecount;i++) list[i]=NULL;

  /* get the first directory listing */
  /* different techniques used by Turbo C an QuickC  */

#if defined TURBOC
   done=findfirst(filespec,&DTA,0);
#endif

#if defined QUICKC
   done= _dos_findfirst(filespec,_A_NORMAL,&DTA);
#endif

/* the first step is to set pointer array to memory allocated
   for each directory entry */


i=0;
 while(!done){  /* loop to get any additional entries */

  if(i==filecount) break;    /* stop if we exceed our pointers */
  /* allocate memory */
  list[i]=  malloc (DIR_NAME_LENGTH*sizeof(char));

  if(list[i] !=NULL){ /* if we have memory for directory entry */

/* copy the directory listing to the allocated space */

#if defined TURBOC
         sprintf(list[i],"%-12s",DTA.ff_name);
#endif

#if defined QUICKC
         sprintf(list[i],"%-12s",DTA.name);
#endif


       /* get the next directory listing */
#if defined TURBOC
        done=findnext(&DTA);
#endif

#if defined QUICKC
        done=_dos_findnext(&DTA);
#endif

     }  /* end if != NULL */
      else return (1);   /* error allocating memory */
    i++;                   /* inc index */

 } /* end while(!done) */

 /* now we call on list_select() to process directory list */

    if(i>0){  /* if any entries were found */

     number=list_select(list);  /* Call list_select so user can
                                   make selection.  Upon return,
                                   "number" equals the file selected
                                   or -1 if user "Escaped". */
    if(number==-1) strcpy(selection,"");
     else
       strcpy(selection, list[number]);

    for(j=0;j<=filecount-1;j++){   /* free up allocated memory */
     if (list[j]!= NULL){
      free(list[j]);
      list[j]=NULL;
     }
     else
        break;
    }
     if (list!=NULL){
       free (list);
       list=NULL;
     }
  }
  return(0);
}


int file_count(char *filespec)
{
extern struct screen_structure scr;
extern struct window_structure w[];

int done;               /* flag */
int count=0;

#if defined TURBOC
   struct ffblk DTA;
#endif

#if defined QUICKC
   struct find_t DTA;
#endif

#if defined TURBOC
   done=findfirst(filespec,&DTA,0);
#endif

#if defined QUICKC
   done= _dos_findfirst(filespec,_A_NORMAL,&DTA);
#endif
   if(!done)count++;

/* first step is set pointer array to memory allocated
   for each directory entry */


 while(!done){       /* if we have at least one entry,
                        loop to get the rest */

/* get the next directory listing */

#if defined TURBOC
        done=findnext(&DTA);
#endif

#if defined QUICKC
        done=_dos_findnext(&DTA);
#endif

  if(!done) count++;

 } /* end while(!done) */

 return (count);
}
