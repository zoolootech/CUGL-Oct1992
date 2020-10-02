/* Copyright (c) James L. Pinson 1990,1991  */

/**********************   READHLP.C   ***************************/

#include "mydef.h"
#include <stdio.h>
#include <io.h>
#include <fcntl.h>

#if defined TURBOC
#include <alloc.h>
#endif

#if defined QUICKC

#include <malloc.h>
#include <sys\types.h>

#endif

void read_help( void)
{
extern struct screen_structure scr;
extern struct window_structure w[];
extern struct help_structure hlp;

static int in_use=FALSE;
int max_pages;
int page;
char ch;
char ext;

/* save text attribute for existing window */
char old_attr=scr.current;    
int  old_bold_caps=scr.bold_caps; /* save old bold caps setting */

char *buffer;
char far *buffer_ptr;
char far *scrn_ptr;
int buff_size;
int header_size;
int i,j;

int handle;
long offset;
struct header_struct{
          int width;
          int height;
          }hlp_header;

if(in_use)return;  /* don't invoke help if help already active */
if(hlp.page==0)return; /* return if page =0 (disable help) */

in_use=TRUE;       /* now mark in_use as true */

scr.bold_caps=FALSE; /* we don't want bold caps on */

page=hlp.page-1;


handle= open(hlp.filename,O_RDWR|O_BINARY);

if (handle==(-1)) { /* if file does not exist */
 win_center (40,5,STD_FRAME," Warning: ",hlp.frame_attr,\
          hlp.interior_attr);

  cls();
  print(1,1,"The help file ");print_here(hlp.filename);
  print_here(" is not found");
  print(1,3,"Press any key to continue.");
  get_key(&ch,&ext);
  win_delete_top();

  /* restore the attribute previously in use */
  scr.current=old_attr; 
  in_use=FALSE;         /* show that help is no longer active */
  return;
}

/* read the header */
read(handle,(char *)&hlp_header,2*sizeof(int)); 
offset=lseek(handle,(long)0,SEEK_END); /* how many bytes in file? */
max_pages=(offset-(long)(2*sizeof(int)))/(long)
          (hlp_header.width*hlp_header.height);
if (page>=max_pages) page=max_pages-1;

alt_screen(ON);
win_make (hlp.x,hlp.y,hlp_header.width,hlp_header.height,
          STD_FRAME,hlp.message,hlp.frame_attr,hlp.interior_attr);
alt_screen(OFF);
cursor(NO_CURSOR);

buff_size=sizeof(char)*(hlp_header.width*hlp_header.height);
header_size=sizeof(int)*2;

buffer=NULL;
/* allocate memory for the help buffer */
buffer=(char*)malloc(buff_size);  

/* here we begin the loop to read pages (PgUp,PgDn, or Esc) */
 for(;;){

   /* load help page into buffer */
   lseek(handle,(long)(header_size+ buff_size*page),SEEK_SET);
   read(handle,buffer,buff_size);

   /* copy buffer to active window */

      /* scan the help window for characters */

      buffer_ptr=buffer;    /* set temp pointer = buffer pointer */
      for(i=0;i<hlp_header.height;i++){  /* get each row */

         scrn_ptr=(char far *)(scr.buffer+(scr.top+i-1)*
                  (scr.columns*2)+2*(scr.left-1));

          for(j=0;j<hlp_header.width;j++){

           /* copy char from buffer to screen */
           *scrn_ptr=*buffer_ptr;      
            buffer_ptr++;scrn_ptr+=2; /* increment both pointers */
          }
       }

   get_key(&ch,&ext);

   if(ext==PGUP) page--;    /* adjust page according to user input */
   if (ext==PGDN) page++;
   if (ch==ESCAPE) break;
    if(page>max_pages-1)
     page=max_pages-1;
    if(page<0) page=0;
 }

/* finish up before exit */

close(handle);
if(buffer!=NULL) free(buffer);

win_delete_top();

/* restore the attribute previously in use */
scr.current=old_attr;          
scr.bold_caps= old_bold_caps;  /* restore bold_caps setting */
in_use=FALSE;                  /* show help no longer in use */
}


void get_key(char *ch, char *ext)
{
  for(;;){
   *ch=' ';*ext=' ';

   *ch=getch();      /* get the character */

    if(!*ch){       /* if the character is zero (a special key) */
      *ext=getch(); /* get the extension */
      if (*ext==F1) read_help();
    }

    if (*ext!=F1) break;  /* if F1 pressed we must read again */
  }
}
