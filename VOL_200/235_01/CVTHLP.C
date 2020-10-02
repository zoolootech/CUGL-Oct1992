/*  004  12-Jan-87  cvthlp.c

        This program converts a text help file to "compiled" format
        used by OverView.

        Copyright (c) 1987 by Blue Sky Software.  All rights reserved.
*/

#define VERSION "Cvthlp 1.01 12-Jan-87"

#include <stdio.h>
#include <fcntl.h>

unsigned long offset = 0;

char inbuf[4096], outbuf[4096];

int verbose = 0;
FILE *inf, *outf;
char line[81], *infile, *outfile;

#define TXTLINES 24
#define MENUENTS 20

unsigned char nmenu, ntext;
char menuent[MENUENTS][81], textlin[TXTLINES][81], header[81];

char *strchr();
long ftell(), findframe();


/******************************************************************************
                                M A I N
 *****************************************************************************/

main(argc,argv)
int argc;
char **argv;
{
   /* process the command line arguments */

   do_args(argc,argv);

   /* assume user does't know what he is doing if I/O is to/from tty */

   if (isatty(fileno(inf)) || isatty(fileno(outf)))
      usage();

   setmode(fileno(outf),O_BINARY);     /* need binary mode on output */

   /* use big buffers size for in/out files */

   setvbuf(inf,inbuf,_IOFBF,sizeof(inbuf));
   setvbuf(outf,outbuf,_IOFBF,sizeof(outbuf));

   /* convert the file */

   pass1();                    /* pass 1, transform text file to frame fmt */

   fclose(inf);                /* close pass 1 files */
   fclose(outf);

   if ((outf = fopen(outfile,"r+")) == NULL) {
      perror("Can't reopen out file");
      exit(2);
   }

   setmode(fileno(outf),O_BINARY);             /* need binary mode on output */
   setvbuf(outf,outbuf,_IOFBF,sizeof(outbuf)); /* use a big buffer */

   pass2();                    /* pass 2, fixup offsets in output file */

   fclose(outf);
}


/******************************************************************************
                                P A S S 1
 ******************************************************************************/

static int
pass1() {              /* transform text help file to frame format */

   int i;

   *line = ' ';                /* force an initial file read */

   while (read_txt_frame()) {

      /* write the help frame data to the output file */

      fputs(header,outf);              /* output the frame header line */

      fputc(nmenu,outf);               /* # menu entries */

      for (i = 0; i < nmenu; i++) {    /* do all menu entries */
         fwrite(&offset,sizeof(offset),1,outf);    /* offset to ent's frame */
         fputs(menuent[i],outf);                   /* keyword/prompt string */
      }

      fputc(ntext,outf);               /* # text lines */

      for (i = 0; i < ntext; i++)      /* do all text lines */
         fputs(textlin[i],outf);
   }
}


/******************************************************************************
                      R E A D _ T X T _ F R A M E
 ******************************************************************************/

static int
read_txt_frame() {            /* read a text help frame */

   if (feof(inf))
      return(0);

   if (*line != '#')                       /* read till next header line */
      while (readlin() && *line != '#')    /* (doesn't read if already there */
         if (verbose)
            fprintf(stderr,"Skipping: %s\n",line);

   if (*line == '#') {
      strcpy(header,line);             /* save header line less # */

      if (verbose)
         fprintf(stderr,"Header: %s",header);

      /* read the text and menu segments */

      nmenu = ntext = 0;

      readlin();

      if (strnicmp(line,"TEXT",4) == 0) {
         read_txt_text();
         readlin();
      }

      if (strnicmp(line,"MENU",4) == 0) {
         read_txt_menu();
         readlin();
      }

      if (nmenu == 0 && ntext == 0)
         fprintf(stderr,"No menu or text section for %s",header);

      return(1);
   }

   return(0);
}


/*****************************************************************************
                          R E A D _ T X T _ T E X T
 *****************************************************************************/

static int
read_txt_text() {      /* read text section */

   while (readlin() && strnicmp(line,"TXET",4) != 0 && ntext < TXTLINES)
      strcpy(textlin[ntext++],line);

   if (ntext == TXTLINES && strnicmp(line,"TXET",4) != 0) {
      fprintf(stderr,"More than %d lines of text for %s",TXTLINES,header);
      while (readlin() && strnicmp(line,"TXET",4) != 0)
         ;
   }
}


/*****************************************************************************
                        R E A D _ T X T _ M E N U
 *****************************************************************************/

static int
read_txt_menu() {      /* read menu section */

   while (readlin() && strnicmp(line,"UNEM",4) != 0 && nmenu < MENUENTS)
      strcpy(menuent[nmenu++],line);

   if (nmenu == MENUENTS && strnicmp(line,"UNEM",4) != 0) {
      fprintf("More than %d menu entries for %s",MENUENTS,header);
      while (readlin() && strnicmp(line,"UNEM",4) != 0)
         ;
   }
}


/******************************************************************************
                              R E A D L I N
 ******************************************************************************/

static int
readlin() {            /* read a line */

   char buf[512];

   if (fgets(buf,sizeof(buf),inf)) {
      if (strlen(buf) > 80) {
         buf[81] = '\0';
         if (verbose)
            fprintf(stderr,"Truncated: %.65s\n",buf);
      }
      strcpy(line,buf);
      return(1);
   }

   return(0);
}


/******************************************************************************
                                P A S S 2
 ******************************************************************************/

static int
pass2() {              /* reread the frame file and fixup the frame offsets */

   while (fixframe())          /* fixup all the help frames */
      ;
}


/******************************************************************************
                             F I X F R A M E
 ******************************************************************************/

static int
fixframe() {           /* fixup the current help frame */

   unsigned long offsetloc, frameloc;

   if (fgets(header,sizeof(header),outf) == NULL)    /* get frame header */
      return(0);                                     /* done if EOF */

   nmenu = fgetc(outf);                /* get # menu entries */

   while (nmenu--) {

      offsetloc = ftell(outf);         /* this is the loc to fixup */

      fread(&frameloc,sizeof(frameloc),1,outf);  /* read dummy offset */
      fgets(line,sizeof(line),outf);   /* read menu item */

      frameloc = findframe();          /* find the frame for this menu item */

      fseek(outf,offsetloc,SEEK_SET);  /* back to the menu offset location */

      fwrite(&frameloc,sizeof(frameloc),1,outf);    /* offset to ent's frame */

      fseek(outf,0L,SEEK_CUR);         /* seek nowhere just so we can read */

      fgets(line,sizeof(line),outf);   /* reread current menu item */
   }

   /* just skip over the text lines to find next frame */

   skip_txt();

   return(1);                  /* another frame processed */
}


/******************************************************************************
                           F I N D F R A M E
 ******************************************************************************/

static long
findframe() {          /* find the frame whose name is in line */

   int nummenu;
   long frameloc;
   char *cp, name[81];

   if (cp = strchr(line,' ')) {        /* isolate frame name from menu line */
      *name = '\0';
      strncat(name,line,cp-line);
   } else {
      fprintf(stderr,"Bad menu ent in findframe: %s\n",line);
      exit(2);
   }

   /* skip to the end of the current frame */

   nummenu = nmenu;
   while (nummenu--) {
      fread(&frameloc,sizeof(frameloc),1,outf);   /* skip over offset */
      fgets(line,sizeof(line),outf);              /* skip over keyword/prompt */
   }

   skip_txt();         /* skip over text lines */

   while (1) {         /* now find the desired frame */

      frameloc = ftell(outf);                  /* this may be the one */

      if (fgets(line,sizeof(line),outf) == NULL) {   /* error if EOF */
         fprintf(stderr,"Unable to find frame %s\n",name);
         exit(2);
      }

      if (*line != '#') {              /* make sure were at a frame start */
         fprintf(stderr,"Not start of frame: %s\n",line);
         exit(2);
      }

      if (strnicmp(name,line+1,strlen(name)) == 0)      /* is this the one? */
         return(frameloc);                              /* yes, return offset */

      /* not the one, skip to the next one */

      nummenu = fgetc(outf);
      while (nummenu--) {
         fread(&frameloc,sizeof(frameloc),1,outf);   /* skip offset */
         fgets(line,sizeof(line),outf);              /* skip keyword/prompt */
      }

      skip_txt();         /* skip over text lines */
   }
}


/*****************************************************************************
                            S K I P _ T X T
 *****************************************************************************/

static int
skip_txt() {           /* skip over a frame's text lines */

   int numtext;

   numtext = fgetc(outf);
   while (numtext--)
      fgets(line,sizeof(line),outf);
}


/******************************************************************************
                               D O _ A R G S
 ******************************************************************************/

static int
do_args(argc,argv)     /* process the command line arguments */
int argc;
char **argv;
{
   int c;
   extern char *optarg;
   extern int optind;

   while ((c = getopt(argc,argv,"v")) != EOF)
      switch (c) {
         case 'v':
            verbose++;
            break;
         default:
            usage();
            break;
      }

   if (optind < argc) {                 /* next arg is input file name */
      if ((inf = fopen(infile = argv[optind],"r")) == NULL) {
         perror("Unable to open input file");
         exit(2);
      }
      optind++;

   } else              /* input file name was not given */
      usage();

   if (optind < argc) {                 /* next arg is output file name */
      if ((outf = fopen(outfile = argv[optind],"w")) == NULL) {
         perror("Unable to open output file");
         exit(2);
      }
      optind++;

   } else              /* output file name was not given */
      usage();

   if (optind < argc)  /* better not be any more arguments */
      usage();
}


/*****************************************************************************
                               U S A G E
 *****************************************************************************/

static int
usage() {              /* tell user how to use program */

   fputs("\nUsage:  cvthlp [-v] in_file out_file\n\n",stderr);

   fputs("Where: -v turns on verbose messages.\n",stderr);
   fputs("       in_file is the name of the help file to be converted\n",stderr);
   fputs("       out_file is name to give the converted help file\n",stderr);

   exit(2);
}
