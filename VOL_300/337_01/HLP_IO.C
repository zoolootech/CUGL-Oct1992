/* Copyright (c) James L. Pinson 1990,1991  */

/**********************   HLP_IO.C   ***************************/

/*
link with:
makehelp.c
hlp_menu.c
*/

#include "mydef.h"
#include "help.h"
#include <stdio.h>
#include  <io.h>
#include <fcntl.h>

#if defined TURBOC

#include <alloc.h>

#endif

#if defined QUICKC

#include <sys\types.h>
#include <malloc.h>
#include <string.h>

#endif

#include <sys\stat.h>

/* get_name allows the user to select a file from a "point and shoot"
   menu containing all the file names matching the wild card
   filename */


void get_name(char *filename)
{
extern struct screen_structure scr;
extern struct window_structure w[];
extern struct hlp help;

int done=FALSE;
char ch,ext;
int handle;
int i;
int name_win;  /* window handle for name window */
int dir_win;   /* window handle for directory window */
char temp[80];
char dir_name[80];
char orig_dir[80];
int return_code;
long offset;
int page_size;
int header_size;

header_size=sizeof(int)*2;

name_win= win_make(1,1,78,4,STD_FRAME,"",scr.normal,scr.normal);

 while(!done){
  for(;;){  /* loop to get file name */
    cls();
    print(1,1, "Please name a file (new or existing), or");
    print(1,2,"(Press Enter to select from list.");
    gotoxy(1,3);
    getcwd(dir_name,79);  /* get the current directory */
    /* if it doesn't have a backslash then add it */
    if(dir_name[strlen(dir_name)-1]!='\\')
       strcat(dir_name,"\\");              /* add it */
    scr.current=scr.inverse;
    ext=getfield(dir_name,78,strlen(dir_name),scr.current);
    trim(dir_name);   /* remove spaces from ends of string */
    scr.current=scr.normal;
     if(ext==ESCAPE){  /* escape */
            strcpy(filename,"");
            win_delete(name_win);
            return;
     }

     /* remove any extension from name */
     i=pos(dir_name,".");
     if (i>=0) dir_name[i]='\0';
    /* Did the user enter the name of a path or drive? */
    if(dir_name[strlen(dir_name)-1]=='\\'
       || dir_name[strlen(dir_name)-1]==':'){
    strcpy(orig_dir,dir_name);  /* save the path */
    strcat(dir_name,"*.hlp");
    strcpy(filename,"");
    dir_win= win_make(10,2,13,10,STD_FRAME," Files: ",
                       scr.normal,scr.normal);
    dir(dir_name,filename);
    win_delete(dir_win);

    if(strlen(filename)>0){ /* was a file selected? (in temp) */
      strcpy(temp,orig_dir);
      strcat(temp,filename);
      strcpy(filename,temp);
      break;    /* we have a name (break for(;;) loop) */
     }
       else  filename[0]='\0'; /* no file selected from list */

    } /* end (is path) */

    else{ /* not path, must be name */
     strcpy(filename,dir_name);
     break;
    }
  }   /* end for(;;) loop to get file name*/

 /* entry was not a path or directory, let's try to open it */
 /* add the .hlp extension if none was specified */

 if(pos(filename,".")==-1)strcat(filename,".hlp");

  /* open for append-create if not exist */

  handle= open(filename,O_RDWR|O_BINARY);

  if (handle==(-1)) { /* create new file ? */
    cls();
    print(1,1,"The file does not exist:");
    print(1,2,"Create a new file y/n ? ");
     for(;;){   /* find out if user wants a new file */
      get_key(&ch,&ext);
      if (toupper(ch)=='Y'){
           return_code=get_size(); /* prompt user for
                                      size of help screen */
           if (return_code==ESCAPE) break;
           handle= open(filename,O_CREAT|O_BINARY|O_RDWR,
                    S_IREAD|S_IWRITE);

           if(handle==(-1)){
              cls();
              print(1,1,"Error opening file!");
              print(1,2,"Press any key to continue.");
              get_key(&ch,&ext);
              break;
           }else
           {
            /* write the header */
            write(handle,(char *)&help,header_size);
            close(handle);
            help.number_pages=0; /* new file, no pages yet */
            done=TRUE;
            break;  /* escape the for(;;) loop */
           }

      } /* end of if (toupper(ch)=='Y')*/

       if (toupper(ch)=='N'){
        putch(ch);
        strcpy(filename,"");
        break;
       }

     } /* end for(;;) which asks if user wants to create file */

  }   /* end if (handle==(-1))  (create new file?) */

 else{ /* file opened (new or existing), now read header */
   /* read the header */
   read(handle,(char *)&help,header_size);

   /* calculate page size */
   page_size=sizeof(char)*(help.width*help.height);

   /* how many bytes in file? */
   offset=lseek(handle,(long)0,SEEK_END);

   /* calculate the number of pages */
   help.number_pages=(offset-(long)header_size)/(long)page_size;

   close(handle);
   done=TRUE;
   /*break;*/
 } /* end (read header) */
     print(1,1,"x");
 }  /* end while(!done); */

 win_delete(name_win);
}


int save_page(int page)  /* saves the current help page */
{
extern struct hlp help;
extern struct screen_structure scr;
extern struct window_structure w[];

int file_handle;
char *buffer=NULL;
char far *temp;
char far *scrn_ptr;
int i,j;
int page_size;
int header_size;

 page_size=sizeof(char)*(help.width*help.height);
 header_size=sizeof(int)*2;

 win_pop_top(help.edit);
 scr.current=win_what_attr(help.edit);
 cursor(NORMAL_CURSOR);

 /* allocate space, allow for \0 terminator */
 buffer=(char *)malloc ((help.width*help.height)*sizeof(char));
 temp=buffer;    /* set temp pointer = pointer */

 for(i=0;i<help.height;i++){  /* get each row */

  scrn_ptr=(char far *)(scr.buffer+(scr.top+i-1)*(scr.columns*2)
               +2*(scr.left-1));
    for(j=0;j<help.width;j++){
      *temp=*scrn_ptr;
      temp++;scrn_ptr+=2; /* adjust pointers, skipping attributes */
    }
 }

 /* save image to file */
 file_handle=open (help.filename,O_RDWR|O_BINARY);
 lseek(file_handle,(long)(header_size+ page_size*page),SEEK_SET);
 write(file_handle,buffer,page_size);
 close(file_handle);

 if(buffer!=NULL)free(buffer);
 win_cls(help.edit); win_redraw_all();win_pop_top(help.menu);
 return(1);
}


int load_page(int page)   /* load a help page from the file */
{
extern struct hlp help;
extern struct screen_structure scr;
extern struct window_structure w[];

int file_handle;
char *buffer;
char *buffer_ptr;
char far *scrn_ptr;
int page_size;
int header_size;
int i,j;

page_size=sizeof(char)*(help.width*help.height);
header_size=sizeof(int)*2;

buffer=NULL;
buffer=(char*)malloc(page_size);

 /* load help page into buffer */
 file_handle=open (help.filename,O_RDWR|O_BINARY);
 lseek(file_handle,(long)(header_size+ page_size*page),SEEK_SET);
 read(file_handle,buffer,page_size);
 close(file_handle);

 /* copy buffer to active window */

 /* scan the help window for characters */

 buffer_ptr=buffer;    /* set temp pointer = buffer pointer */
 for(i=0;i<help.height;i++){  /* get each row */
   scrn_ptr=(char far *)(scr.buffer+(scr.top+i-1)
              *(scr.columns*2)+2*(scr.left-1));

   for(j=0;j<help.width;j++){
     *scrn_ptr=*buffer_ptr;    /* copy char from buffer to screen */
     buffer_ptr++;scrn_ptr+=2; /* increment both pointers */
   }
 }

if(buffer!=NULL) free(buffer);
return(0);
}

/* append a new help page to the file */

int append(char *filename, char *buffer, int page_size)
{
int file_handle;

 /* write record use file_handle int */

 file_handle=open (filename,O_RDWR|O_APPEND|O_BINARY);
 write(file_handle,buffer,page_size);
 close(file_handle);

 return (0);
}
