/* BU.C - A File Backup Utility for CP/M-80 & CP/M-86
 *
 * Copyright:	Ian Ashdown
 *		byHeart Software
 *		2 - 2016 West First Avenue
 *		Vancouver, B.C. V6J 1G8
 *		Canada
 *
 * Acknowledgment: DeSmet C code and suggestions for program
 * 		   improvement courtesy of Dr. Dobb's Journal
 *		   Contributing Editor Anthony Skjellum
 *			
 * Version:	1.2	Written for Aztec CII v1.06b (CP/M-80)
 *			and DeSmet C88 v2.2 (CP/M-86)
 *
 * Date:	December 31st, 1983 (Version 1.0)
 *		September 7th, 1984 (Version 1.1)
 *		November 7th, 1985 (Version 1.2)
 *
 * Version Modifications:
 *
 * 24/09/84 - "read()" and "write()" accept maximum of 32767
 *	      bytes, not 32768. Functions "copy_file()" and
 *	      "verify_file()" modified accordingly.
 * 07/11/85 - function "copy_file()" incorrectly copied file
 *	      when last invocation of "read()" exactly filled
 *	      input file buffer or input file was of zero length.
 *	      Statement added to break copy loop on "read()"
 *	      returning zero.
 *
 * BU utilizes the undocumented "archive" file attribute feature
 * of CP/M-80 Versions 2.x and CP/M-86 to automatically detect
 * files that have been changed since the disk was last "backed
 * up" and copy them (with verification) to a backup disk. This
 * program performs the same action as the "A" option of PIP 
 * under Digital Research's MP/M 2, for which the Archive
 * attribute is documented.
 *
 * Usage: BU x[:afn] y [-AFHQSn]
 *
 *	  where x = drive name of disk to be backed up
 *		y = drive name of backup disk
 *
 *	  and the optional arguments are:
 *
 *		-A	  All files, regardless of status
 *		-F	  Fast copy (without verification)
 *		-H	  Hard disk (files may be split)
 *		-Q  	  Query each file before backup
 *		-S	  System attribute copied to backup
 *		-n	  Backup USER 'n' files only (0-31)
 *		afn	  Any legal CP/M ambiguous fileref
 *			  (can only be used with -n option)
 */

#include "stdio.h"
#include "ctype.h"	/* Contains macro for "isdigit()" */

/*** DEFINITIONS ***/

#define AZTEC	1	/* Aztec CII v1.06b (CP/M-80) */
#define DESMET	0	/* DeSmet C88 v2.2 (CP/M-86) */

#if DESMET
#define movmem(src,dest,len)	_mov(len,src,dest)
#endif

#define ERROR	   -1
#define DEL	   -1	/* Deleted fileref flag */
#define ALL	   -1	/* All user numbers flag */
#define TRUE	   -1
#define FALSE	    0
#define SUCCESS     0
#define O_RDONLY    0
#define USER_ERR    0	/* Specified fileref must only be used
			   with -number command-line option */
#define BAD_FREF    1	/* Illegal file reference */
#define BAD_ARGS    2	/* Illegal command line */
#define BAD_OPT     3	/* Illegal option */
#define BAD_USER    4	/* Illegal user number */
#define BAD_DRV     5	/* Illegal drive names */
#define SAME_DRV    6	/* Same drive name for output as input */
#define OPN_ERR     8	/* File open error */
#define READ_ERR    9	/* File read error */
#define CLS_ERR    10	/* File close error */
#define BAD_VFY    11	/* File verify error */
#define DIR_IO	    6	/* BDOS Direct I/O service */
#define RESET_DRV  13	/* BDOS Reset All Drives service */
#define SEL_DRV	   14	/* BDOS Select Drive service */
#define SRCH_F	   17	/* BDOS Search Next service */
#define SRCH_N	   18	/* BDOS Search Next service */
#define GET_DRV	   25	/* BDOS Get Default Drive service */
#define SET_DMA	   26	/* BDOS Set DMA Address service */
#define SET_ATT	   30	/* BDOS Set File Attributes service */
#define USER_CODE  32	/* BDOS Get/Set User Code service */
#define MAX_USER   32	/* 32 user codes under CP/M (see DR's
			   documentation for BDOS Service 32) */

/*** GLOBAL VARIABLES ***/

char ent_drv,	/* Entry drive code */
     ent_user,	/* Entry user code */
     cur_user;	/* Current user code */

/*** MAIN BODY OF CODE ***/

main(argc,argv)
int argc;
char *argv[];
{
  char in_dsk,		/* Drive name of input disk */
       out_dsk,		/* Drive name of output (backup) disk */
       in_drv,		/* Drive code of input disk */
       out_drv,		/* Drive code of output disk */
       seg_no,		/* Segment number for split files */
       in_file[15],	/* Fileref of current input file */
       out_file[15],	/* Fileref of current output file */
       c,		/* Scratch variable */
       *s,		/* Scratch string pointer */
       *buffer,		/* Pointer to directory entry returned */
			/* by "srch_file()" */
       *srch_file(),
       *malloc();

  /* File control blocks of current input and output files */

  static char in_fcb[33],	/* (Automatically initialized */
	      out_fcb[33];	/* to zero by compiler) */

  /* Structure for linked list of filerefs */

  struct file_ref
  {
    char name[12];		/* File reference */
    struct file_ref *next;	/* Pointer to next instance */
  } root,			/* Start of linked list */
    *fref_1,			/* Scratch pointers to */
    *fref_2;			/* linked list instances */

  /* Initialized file control block for "srch_file()". This FCB
     is for a fully ambiguous fileref that causes "srch_file()"
     to return all directory entries for the current default
     drive. */

  static char fcb[] = {'?','?','?','?','?','?','?','?',
		       '?','?','?','?','?',0,0,0};

  int file_cnt = 0,	/* Count of file to be backed up */
      dup_flag,		/* Duplicate fileref flag */
      all_files,	/* All_files flag (cmd-line option) */
      fast_copy,	/* Fast copy flag (cmd-line option) */
      hard_disk,	/* Hard disk flag (cmd-line option) */
      query,		/* Query flag (cmd-line option) */
      system,		/* System flag (cmd-line option) */
      user_no,		/* User number (cmd-line option) */
      next_flag = FALSE;/* Flag to indicate to "srch_file()"
			   that a "search next" is required */

  register int i,j;	/* Loop indices */

  long begin,		/* Input file position variables */
       end;

  /* Display program header */

  printf("\nBU Version 1.2");
  printf("                                 Copyright 1985");
  printf(" byHeart Software\n\n");

  /* Initialize command-line options */

  all_files = FALSE;	/* Copy only non-archived files */
  fast_copy = FALSE;	/* Copy files with verification */
  hard_disk = FALSE;	/* Files will not be split across backup
			   disks if remaining capacity of backup
			   disk is less than current file size */
  query = FALSE;	/* Backup without query */
  system = FALSE;	/* Assign directory attribute to all
			   backup files */
  user_no = ALL;	/* Backup files in all user areas */  

  /* Parse command line for user-selected options (if any) */
		    
  if(argc < 3)
    error(BAD_ARGS,NULL);	/* Illegal command line */
  if(argc > 3)
  {
    i = 3;  /* Start with third command-line argument */
    while(i < argc)
    {
      if(*argv[i] != '-')
	error(BAD_OPT,argv[i]);  /* Missing leading '-' */
      s = argv[i]+1;
      while(*s)
      {
	if(*s == 'A')	    /* Check for all_files option */
	  all_files = TRUE;
	else if(*s == 'F')  /* Check for fast copy option */
	  fast_copy = TRUE;
	else if(*s == 'H')  /* Check for hard disk option */
	  hard_disk = TRUE;
	else if(*s == 'Q')  /* Check for query option */
	  query = TRUE;
	else if(*s == 'S')  /* Check for system option */
	  system = TRUE;
	else if(isdigit(*s))  /* Check for user number option */
	{
	  user_no = *s++ - '0';
	  if(isdigit(*s))
	    user_no = user_no * 10 + *s++ - '0';
	  if(user_no < 0 || user_no > 31)
	    error(BAD_USER,argv[i]);
	  continue;
	}
	else
	  error(BAD_OPT,argv[i]);
	s++;
      }
    i++;
    }
  }

  /* Validate input parameters */

  if(*(argv[1]+1) != '\0')  /* Check for specified fileref */
  {
    if(user_no == ALL)	     /* Can only use with specified */
      error(USER_ERR,NULL);  /* user number (-n option) */

    /* Modify "fcb[]" to incorporate fileref */

    if(copy_fref(fcb,argv[1]) == ERROR)
      error(BAD_FREF,argv[1]);
  }
  if(*argv[1] < 'A' || *argv[1] > 'P' ||
      *argv[2] < 'A' || *argv[2] > 'P')
    error(BAD_DRV,NULL);	/* Illegal drive names */
  if(*argv[1] == *argv[2])
    error(SAME_DRV,NULL);	/* Drive names are same */

  /* Save entry drive and user codes */

  ent_drv = bdos(GET_DRV);
  ent_user = bdos(USER_CODE,0xff);

  /* Calculate input and output drive codes */

  in_drv = (in_dsk = *argv[1]) - 'A' + 1;
  out_drv = (out_dsk = *argv[2]) - 'A' + 1;

  /* Set default drive to input drive */

  bdos(SEL_DRV,in_drv-1);

  /* Set user code to "user_no" if -n option specified */

  if(user_no != ALL)
    bdos(USER_CODE,user_no);

  /* Read first 12 bytes of updated active directory entries into
     linked list of filerefs. If first byte of entry is 0xe5,
     then file has been erased. */

  root.next = NULL;	/* Initialize linked list root */
  fref_1 = &root;	/* Initialize linked list pointer */
  while(buffer = srch_file(fcb,next_flag))
  {
    /* Bit 7 of third filetype byte (t3') in directory entry
       is the Archive attribute indicator. The BDOS sets this
       bit to zero whenever it updates a directory entry. */

    if(buffer[0] != 0xe5 &&
      (buffer[0] == user_no || user_no == ALL) &&
      (!(buffer[11] & 0x80) || all_files))
    {
      fref_1->next =	/* Allocate space for fileref instance */
	  (struct file_ref *) malloc(sizeof(struct file_ref));
      fref_1 = fref_1->next;	/* Assign space to instance */
      movmem(buffer,fref_1->name,12);  /* Move fileref to */
				       /* linked list instance */
      fref_1->next = NULL;  /* Indicate current end of list */
    }
    next_flag = TRUE;	/* Only first call to "srch_file()" */
			/* should be made with "next_flag" */
  }			/* set to FALSE */

  /* If no files have been backed up ... */

  if(!root.next)  /* Null "root.next" indicates no files have */
    {		  /* been changed */
      printf("NO FILES HAVE BEEN UPDATED");
      if(user_no != ALL)
	printf(" in user area %d\n",user_no);
      else
	putchar('\n');
      reset();	/* Reset user and drive codes to entry values */
      exit(0);
    }

  /* There may be duplicate filerefs in linked list due to some
     files occupying more than one extent on the disk. These
     duplicates must be marked as "deleted" in the list.
     (Duplicate filerefs with different user codes are valid.) */

  /* For all filerefs ... */

  fref_1 = &root;  /* Initialize a linked list pointer */
  while(fref_1->next)
  {
    fref_1 = fref_1->next;  /* Root instance is NULL entry */
    dup_flag = FALSE;	    /* Reset duplicate fileref flag */

    /* For all preceding filerefs ... */

    fref_2 = &root;  /* Initialize another linked list pointer */
    fref_2 = fref_2->next;  /* Skip root instance */
    while(fref_2->next != fref_1->next)
    {
      /* Compare filerefs (ignore deleted filerefs and different
	 user codes) */

      if(fref_2->name[0] != DEL && fref_1->name[0] == fref_2->name[0])
	if(!strncmp(fref_1->name+1,fref_2->name+1,11))
	{
	  dup_flag = TRUE;	/* Indicate duplicate fileref */
	  break;
	}
      fref_2 = fref_2->next;
    }
    if(dup_flag == TRUE)
      fref_1->name[0] = DEL;	/* Delete if duplicate fileref */
    else
      file_cnt++;		/* Increment file count */
  }

  /* Display file copy header */

  printf("Number of files to be copied: %d\n\n",file_cnt);
  printf("User:    Files being copied to Drive %c:\n\n",
	 out_dsk);

  /* Initialize current input and output fileref templates */

  in_file[0] = in_dsk;
  out_file[0] = out_dsk;
  in_file[1] = out_file[1] = ':';
  in_file[10] = out_file[10] = '.';
  in_file[14] = out_file[14] = '\0';

  /* Initialize current input and output FCB templates */

  in_fcb[0] = in_drv;
  out_fcb[0] = out_drv;

  /* For all validated filerefs do ... */

  for(cur_user = 0; cur_user < MAX_USER; cur_user++)
  {
    if(user_no != ALL)
      if(cur_user != user_no)
	continue;
    bdos(USER_CODE,cur_user);	/* Set user code to "cur_user" */
    fref_1 = &root;	/* Initialize linked list pointer */
    while(fref_1->next)
    {
      fref_1 = fref_1->next;	/* Root instance is NULL entry */
      if(fref_1->name[0] == cur_user)
      {
	/* Update the current input and output FCB's */

	movmem(fref_1->name+1,in_fcb+1,11);
	movmem(fref_1->name+1,out_fcb+1,11);

	/* Reset the Read-Only and System attribute bits of the
	   FCB's so that the file can be copied and displayed
	   (unless the "system" flag is TRUE) */

	out_fcb[9] &= 0x7f;	/* Read-Only attribute */
	if(!system)
	  out_fcb[10] &= 0x7f;	/* System attribute */

	/* Set the Archive attribute bit of the FCB's to indicate
	   that the file has been backed up */

	in_fcb[11] |= 0x80;
	out_fcb[11] |= 0x80;

	/* Move the fileref from the FCB's to the initialized
	   input and output fileref templates */

	movmem(in_fcb+1,in_file+2,8);	/* Filename move */
	movmem(out_fcb+1,out_file+2,8);
	movmem(in_fcb+9,in_file+11,3);	/* Filetype move */
	movmem(out_fcb+9,out_file+11,3);

	/* Strip high order bits off filerefs to form proper
	   ASCII characters */

	for(j = 2; j <= 13; j++)
	{
	  in_file[j] &= 0x7f;
	  out_file[j] &= 0x7f;
	}

	/* Display the filerefs */

	printf(" %2d      %s --> %s",cur_user,in_file,out_file);

	/* Query operator for backup if indicated by "query"
	   flag */

	if(query)
	{
	  printf("  O.K. to backup?  ");
	  if((c = in_chr()) == 'y' || c == 'Y')
	    puts("Yes");
	  else
	  {
	    puts("No");
	    continue;	/* Go do next fileref if "No" */
	  }
	}
	else
	  putchar('\n');

	/* Copy file from the input disk to the output disk */

	if(hard_disk)	/* Split file across backup disks if */
	{		/* necessary */
	  begin = 0L;	/* Initialize file position pointer */
	  seg_no = 0;	/* and split file segment number */
	  do
	  {
	    /* Reset the Read-Only attribute of the output file
	       (if it exists) so that the input file can be
	       copied to it */

	    bdos(SET_ATT,out_fcb);

	    end = copy_file(in_file,out_file,begin);
	    if(!fast_copy)  /* Verify file unless -F selected */
	      verify_file(in_file,out_file,begin);

	    /* Set the Read-Only attribute of the output file */

	    out_fcb[9] |= 0x80;
	    bdos(SET_ATT,out_fcb);
	    out_fcb[9] &= 0x7f;    /* ... and reset the fcb */
	    if(end != NULL)
	    {
	      /* File has been partially written on current
		 backup disk - new disk required to continue */

	      new_disk(out_file,hard_disk);

	      /* Append segment number to filename of output
		 fileref (e.g. - B:FILE.TYP will become
		 B:FILE--01.TYP) */

	      seg_no++;
	      for(j = 2; j <= 7; j++)	/* Change spaces to */
		if(out_file[j] == ' ')	/* '-' character */
		  out_file[j] = '-';
	      out_file[8] = seg_no/10 + '0';  /* Append segment */
	      out_file[9] = seg_no%10 + '0';  /* number */

	      /* Display filerefs again */

	      printf(" %2d      %s --> %s\n",
		  cur_user,in_file,out_file);
	    }
	    begin = end;
	  }
	  while(end != NULL);  /* Loop until file is written */
	}
	else
	{
	  /* Reset the Read-Only attribute of the output file
	     (if it exists) */

	  bdos(SET_ATT,out_fcb);

	  if(copy_file(in_file,out_file,0L) != NULL)
	  {
	    /* Disk was full - erase partially written file, back
	       up fileref pointer and rewrite file to new disk */

	    unlink(out_file);
	    new_disk(out_file,hard_disk);
	    i--;
	    continue;
	  }
	  if(!fast_copy)  /* Verify file unless -F selected */
	    verify_file(in_file,out_file,0L);

	  /* Set the Read-Only attribute of the output file */

	  out_fcb[9] |= 0x80;
	  bdos(SET_ATT,out_fcb);
	}

	/* Set the Archive attribute of the input file to
	   indicate that the file was successfully backed up */

	bdos(SET_ATT,in_fcb);
      }
    }
  }
  reset();	/* Reset user and drive codes to entry values */
}

/*** FUNCTIONS ***/

/* Search for first or next directory entry */

char *srch_file(fcb_ptr,next_flag)
char *fcb_ptr;		/* Pointer to file control block */
int next_flag;		/* Flag to indicate "search next" */
{
  static char sf_cur[32],  /* Current directory entry buffer */
              sf_fcb[36];  /* File control block buffer */

  int index,  /* Index of directory entry in DMA buffer */
      *ptr;   /* Pointer to directory entry in DMA buffer */

  bdos(SET_DMA,0x80);	/* Set DMA address to 80h */
  if(!next_flag)
  {
    movmem(fcb_ptr,sf_fcb,16);	/* Initialize FCB buffer */
    if((index = bdos(SRCH_F,sf_fcb)) == 0xff)  /* Find first */
      return NULL;	/* Return NULL if unsuccessful */
  }
  else
    if((index = bdos(SRCH_N,NULL)) == 0xff)  /* Find next */
      return NULL;	/* Return NULL if unsuccessful */

  /* BDOS services 17 and 18 leave four consecutive directory
     entries of 32 bytes each in the 128-byte DMA buffer and also
     returns an index value of 0, 1, 2 or 3 to indicate the
     correct directory entry in the accumulator. The "bdos()"
     function returns this index value. */

  ptr = 0x80 + index * 32;  /* Calculate pointer to directory
			       entry */
  movmem(ptr,sf_cur,32);  /* Move directory entry to current
			     directory entry buffer */
  return sf_cur;
}

/* Copy file starting at "offset" from beginning */

copy_file(in_file,out_file,offset)
char *in_file,		/* Input fileref */
     *out_file;		/* Output fileref */
long offset;		/* Input file position offset */
{
  register int in_cnt,	/* Character counts for unbuffered I/O */
	       out_cnt;
  int fd_in,		/* Input file descriptor */
      fd_out,		/* Output file descriptor */
      full_disk = FALSE;  /* Full disk flag */

  char *buffer,	   /* Input file buffer */
       *buff_ptr,  /* Pointer to current position in "buffer[]" */
       *malloc();

  unsigned buf_size = 32640;  /* Initial memory allocation size */

  /* "read()" accepts a maximum of 32767 bytes at a time. Allocate
     as much memory as possible up to this limit for the input
     buffer, using 128 byte decrements. */

  do
    if(buffer = malloc(buf_size))
      break;
  while(buf_size -= 128);

  /* Open input file for unbuffered Read-Only access */

  if((fd_in = open(in_file,O_RDONLY)) == ERROR)
    error(OPN_ERR,in_file);

  /* Create the output file by first deleting it (if it
     exists), then opening it for unbuffered Write-Only
     access. */

  if((fd_out = creat(out_file,NULL)) == ERROR)
    error(OPN_ERR,out_file);

  /* Initialize input file position pointer to "offset" */

  lseek(fd_in,offset,0);

  /* Copy input file to output file by buffering data
     through "buffer[]" */

  do
  {
    if((in_cnt = read(fd_in,buffer,buf_size)) == ERROR)
      error(READ_ERR,in_file);
    if(in_cnt == 0)	/* End of file */
      break;
    buff_ptr = buffer;  /* Initialize "buffer[]" pointer */
    out_cnt = 0;	/* and "out_cnt" */
    do
    {
      /* Write contents of "buffer[]" to output file in 128
	 byte records until either the buffer is written or a
	 write error occurs. Since the 0x1a (^Z) character CP/M
	 uses as an EOF marker is a valid file character for non-
	 ASCII files, "read()" always reads the last 128 byte
	 record of a file under CP/M. */

      if(write(fd_out,buff_ptr,128) != 128)
      {
	/* The standard implementation of "write()" does not
	   distinguish between a full disk or directory and a
	   write error in its returned error code. Thus, it is
	   assumed that an error means a full disk/directory. */

	full_disk = TRUE;
	break;
      }
      buff_ptr += 128;  /* Increment "buffer[]" ptr */
      out_cnt += 128;  /* Update count of chars written */
    }
    while(in_cnt > out_cnt);	/* Until end of "buffer[]" */
    offset += out_cnt;	/* Update input file position pointer */
    if(full_disk == TRUE)
      break;
  }
  while(in_cnt == buf_size);	/* Until end of file */
  free(buffer);		/* Deallocate buffer space */
  if(close(fd_in) == ERROR)	/* Close the files */
    error(CLS_ERR,in_file);
  if(close(fd_out) == ERROR)
    error(CLS_ERR,out_file);

  /* If full disk return new offset for input file, else */
  /* return NULL to indicate completion of file copy operation */

  return (full_disk ? offset : NULL);
}

/* Compare portion of input file starting at "offset" from begin-
   ning of file with output file */

verify_file(in_file,out_file,offset)
char *in_file,		/* Input fileref */
     *out_file;		/* Output fileref */
long offset;		/* Input file position offset */
{
  register int match_cnt;	/* Scratch variable */

  int out_cnt,	/* Character counts for unbuffered I/O */
      fd_in,	/* Input file descriptor */
      fd_out;	/* Output file descriptor */

  char *buffer,		/* Dynamically-allocated buffer */
       *in_ptr,		/* Input file buffer pointer */
       *out_ptr,	/* Output file buffer pointer */
       *malloc();

  unsigned buf_size = 65280;  /* Initial memory allocation size */

  /* "read()" and "write()" accept a maximum of 32767 bytes at a
     time. Allocate as much memory as possible up to this limit
     for both the input and output buffers, using 256 byte
     decrements (128 bytes for each buffer). */

  do
    if(buffer = malloc(buf_size))
      break;
  while(buf_size -= 256);

  /* Divide "buffer[]" in two for "in_ptr" and "out_ptr" */

  buf_size /= 2;

  if((fd_in = open(in_file,O_RDONLY)) == ERROR)  /* Open files */
    error(OPN_ERR,in_file);
  if((fd_out = open(out_file,O_RDONLY)) == ERROR)
    error(OPN_ERR,out_file);
  lseek(fd_in,offset,0);  /* Initialize file position pointer */

  /* Read in characters from both files and compare */

  do
  {
    in_ptr = buffer;	/* Assign buffer pointers */
    out_ptr = in_ptr + buf_size;
    if(read(fd_in,in_ptr,buf_size) == ERROR)
      error(READ_ERR,in_file);
    if((out_cnt = read(fd_out,out_ptr,buf_size)) == ERROR)
      error(READ_ERR,out_file);
    match_cnt = out_cnt;
    while(match_cnt--)			/* Verify character */
      if(*in_ptr++ != *out_ptr++)	/* by character, and */
      {					/* delete the output */
	if(close(fd_out) == ERROR)	/* file if they fail */
	  error(CLS_ERR,out_file);	/* to match */
	unlink(out_file);
	error(BAD_VFY,out_file);
      }
  }
  while(out_cnt == buf_size);	/* Until end of output file */
  free(buffer);			/* Deallocate buffer space */
  if(close(fd_in) == ERROR)	/* Close the files - verifi- */
    error(CLS_ERR,in_file);	/* cation was successful */
  if(close(fd_out) == ERROR)
    error(CLS_ERR,out_file);
}

/* Copy fileref to file control block */

copy_fref(fcb,fref)
char *fcb,		/* Pointer to file control block */
     *fref;		/* Pointer to fileref */
{
  char c;	/* Scratch variable */
  int i,	/* Fileref index variable */
      j,	/* FCB index variable */
      k,	/* Scratch variable */
      done;	/* Loop break flag */

  if(fref[1] != ':' || fref[2] == '\0')
    return ERROR;  /* No drive code separator or null fileref */

  /* Calculate drive code from drive name and put in FCB */

  fcb[0] = fref[0] - 'A' + 1;

  /* Process remainder of fileref */

  done = FALSE;
  for(i = 2,j = 1;i <= 9;i++,j++)	/* Skip drive code in */
  {					/* fileref */
    switch(c = fref[i])
    {
      case '.':		/* Filetype separator */
	if(i == 2)
	  return ERROR;	/* Null filename */
	for( ; j <= 8; j++)	
	  fcb[j] = ' ';	/* Pad filename with trailing blanks */
	done = TRUE;
	break;
      case '*':		/* Match any following string */
	for( ; j <= 8; j++)
	  fcb[j] = '?';	/* Pad filename with trailing */
	i++;		/* question marks */
	done = TRUE;
	break;
      case '\0':	/* End of fileref */
	for( ; j <= 11; j++)  /* Pad FCB with trailing spaces */
	  fcb[j] = ' ';
	return SUCCESS;
      case ',':		/* Illegal filename characters */
      case ';':
      case ':':
      case '=':
      case '[':
      case ']':
      case '_':
      case '<':
      case '>':
	return ERROR;
      default:
	if(c >= '!' && c <= '~')
	  fcb[j] = c;	/* Copy character from fileref to FCB */
	else
	  return ERROR;	/* Nonprintable character or ' ' */
    }
    if(done)
      break;
  }
  c = fref[i];
  if((c = fref[i]) == '\0')	/* End of fileref */
  {
    for( ; j <= 11; j++)  /* Pad FCB with trailing spaces */
      fcb[j] = ' ';
    return SUCCESS;
  }
  else if(c == '.')	/* Filetype separator */
  {
    i++;
    k = i + 2;		/* Set limit of 3 characters */
    for( ; i <= k; i++,j++)
    {
      done = FALSE;
      switch(c = fref[i])
      {
	case '*':	/* Match any following string */
	  for( ; j <= 11; j++)
	    fcb[j] = '?';	/* Pad filetype with trailing */
	  return SUCCESS;	/* question marks */
	case '\0':	/* End of fileref */
	  for( ; j <= 11; j++)  /* Pad FCB with trailing */
	    fcb[j] = ' ';	  /* spaces */
	  return SUCCESS;
	case '.':		/* Illegal filetype characters */
	case ';':
	case ',':
	case ':':
	case '=':
	case '[':
	case ']':
	case '_':
	case '<':
	case '>':
	  return ERROR;
	default:
	  if(c >= '!' && c <= '~')
	    fcb[j] = c;  /* Copy character from fileref */
	  else	   	 /* to FCB */
	    return ERROR;  /* Nonprintable character or ' ' */
      }
    }

    /* Return ERROR if filetype too long */

    return (fref[i] == '\0' ? SUCCESS : ERROR);
  }
  else
    return ERROR;  /* Filename too long  */
}

/* Error report */

error(n,s)
int n;		/* Error code */
char *s;	/* Pointer to optional string */
{
  switch(n)
  {
    case USER_ERR:
      printf("\007** ERROR - No user number specified **\n");
      break;
    case BAD_FREF:
      printf("\007** ERROR - Illegal file reference: %s **\n",s);
      break;
    case BAD_ARGS:
      printf("\007** ERROR - Illegal command line **\n");
      break;
    case BAD_OPT:
      printf("\007** ERROR - Illegal command line option:");
      printf(" %s **\n",s);
      break;
    case BAD_USER:
      printf("\007** ERROR - User number must be inside range");
      printf(" of 0 to 31 **\n");
      break;
    case BAD_DRV:
      printf("\007** ERROR - Drive names must be inside range");
      printf(" of 'A' to 'P' **\n");
      break;
    case SAME_DRV:
      printf("\007** ERROR - Drive names cannot be equal **\n");
      break;
    case OPN_ERR:
      printf("\007\n** ERROR - Cannot open file %s **\n",s);
      reset();
      exit(0);
    case READ_ERR:
      printf("\007\n** ERROR - Read error on file %s **\n",s);
      reset();
      exit(0);
    case CLS_ERR:
      printf("\007\n** ERROR - Cannot close file %s **\n",s);
      reset();
      exit(0);
    case BAD_VFY:
      printf("\007\n** ERROR - Failed verify on file %s **\n",s);
      reset();
      exit(0);
  }
  printf("\nUsage: BU x[:afn] y [-AFHQSn]\n\n");
  printf("       where x = drive name of disk to be backed up\n");
  printf("             y = drive name of backup disk\n\n");
  printf("       and the optional arguments are:\n\n");
  printf("             -A           All files, regardless of");
  printf(" status\n");
  printf("             -F           Fast copy (without ");
  printf(" verification)\n");
  printf("             -H           Hard disk (files may be");
  printf(" split)\n");
  printf("             -Q           Query each file before");
  printf(" backup\n");
  printf("             -S           System attribute copied to");
  printf(" backup\n");
  printf("             -n           Backup USER 'n' files only");
  printf(" (0-31)\n");
  printf("             -afn         Any legal CP/M ambiguous");
  printf(" fileref\n");
  printf("                          (can only be used with -n");
  printf(" option)\n");
  exit(0);
}

/* Request a new backup disk to be inserted in the output drive */

new_disk(name,hard_disk)
char *name;
int hard_disk;
{
  char d;

  printf("\007\n         ** BACKUP DISK FULL **\n\n");
  if(hard_disk)
    printf("WARNING: -H option active - FILE WILL BE SPLIT\n\n");
  printf("Insert new disk into drive %c to continue.\n",name[0]);
  printf("Type 'C' when ready or 'A' to abort ... ");
  while((d = in_chr()) != 'c' && d != 'C' && d != 'a' && d != 'A')
    ;
  if(d == 'a' || d == 'A')
  {
    unlink(name);
    exit(0);
  }
  else
    printf("\n\n");
  bdos(RESET_DRV,NULL);		/* Reset drives */
}

/* Reset user and drive codes to entry values */

reset()
{
  bdos(USER_CODE,ent_user);
  bdos(SEL_DRV,ent_drv);
}

/* Get character from current CP/M CON: device without echo */

in_chr()
{
  int c;

  do
    c = bdos(DIR_IO,0xff);
  while(!c);
  return c;
}

/* Additional function required for DeSmet C version of BU86.C */

#if DESMET
bdos(beta,delta)
int beta,
    delta;
{
  return _os(beta,delta);
}
#endif

/*** EXPLANATION OF AZTEC CII STDIO.H FUNCTIONS ***/

/* bdos(bc,de)	| DeSmet equivalent is defined under FUNCTIONS |
 * int bc,de;
 *
 * Calls CP/M's BDOS with 8080 CPU register pair BC set to "bc"
 * and register pair DE set to "de". The value returned by the
 * 8080 CPU accumulator is the return value.
 *
 * movmem(src,dest,length)  | DeSmet equivalent is defined  |
 * char *dest, *src;	    | under DEFINITIONS		    |
 * int length;
 *
 * Moves data from "src" to "dest". The number of bytes is
 * specified by the parameter "length".
 *
 * strncmp(str1,str2,max)  | Identical for DeSmet |
 * char *str1,*str2;
 * int max;
 *
 * Compares "str1" to "str2" for at most "max" characters, and
 * returns NULL if strings are equal, -1 if "str1" is less than
 * "str2", and +1 if "str1" is greater than "str2".
 */

/* End of BU.C */
 Additional function required for DeSmet C version of BU86.C */

#if DESMET
bdos(