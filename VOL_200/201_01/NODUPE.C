/*--------------------------------------------------------------------------*/
/*									    */
/* nodupe.c  - module for use with TSR programs which will check	    */
/*	       environments in memory to see if the same file has already   */
/*	       been run.  Keeps user from installing resident programs	    */
/*	       more than once.						    */
/*									    */
/*     version 1.0 - 3/6/86						    */
/*									    */
/*     version 1.1 - 3/9/86						    */
/*	   changed environment buffer to heap storage so it can be	    */
/*	   disposed of later.						    */
/*									    */
/* The program works by first finding the name of the program just invoked  */
/* in the environment area set up for it by DOS.  It then traces back to    */
/* the start of command.com by working backwards along the linked list of   */
/* parents.  The pointer to a program's parent process is contained at      */
/* offset 16H in the program segment prefix. Tracing this chain back until  */
/* it points to itself will locate the psp of command.com. From there, the  */
/* program traces the DOS memory control blocks forward. It searches for a  */
/* program segment prefix by looking for the bytes CD 20, which are the     */
/* INT 20 instruction at the beginning of every psp. When a psp is located, */
/* then the environment area for that program is located by using the	    */
/* pointer at offset 2C. The name of this program is then found and com-    */
/* pared to the name of the current program. If a match is found, then the  */
/* routine returns a TRUE value, indicating it has found a matching name.   */
/* If the names don't match, the routine continues checking blocks until    */
/* it finds a match or reaches the end of the memory control block chain.   */
/*									    */
/* This code is designed to be included in terminate-and-stay-resident	    */
/* (TSR) programs, or to be linked in as an external module. If it is	    */
/* linked as an external module, it will require the inclusion of the	    */
/* external declaration such as:					    */
/*									    */
/*     extern  int  check_dupe();					    */
/*									    */
/*---------------------------------------------Brian Irvine-----------------*/

#include   <stdio.h>

/* The pointer to the parent which invoked a process is located at offset   */
/* 0x16 in the PSP of the current process.				    */

#define    PARENT  0x16
#define    ENVIRON 0x2C
#define    PSPID   0x20CD
#define    SIZEOFF 3
#define    OWNROFF 1

typedef    char bool;

unsigned find_head ();		       /* finds beginning of mcb chain */
unsigned freeall ();
char   *malloc ();
char   *find_name ();		       /* returns pointer to name of current */
				       /* program */
bool   check_end ();

/*----- Check to see if this program is already in memory ------------------*/
/*									    */
/*     Check environments in memory to see if the calling program has been  */
/*     installed already.  Return 0 if no match is found, return 1 if a     */
/*     matching name was found. 					    */
/*									    */
/*--------------------------------------------------------------------------*/

int    check_dupe()
{
   unsigned    owner, psp, mcb;
   unsigned    head, blksize;
   unsigned    chkword, envseg;
   char        *env, *name, *lbuf;
   bool        end = FALSE;

   lbuf = malloc ( 256 );      /* get storage for environment strings */
   name = malloc ( 15 );       /* get storage for file name */
   psp = _showcs() - 0x10;
   mcb = psp - 1;
/*     Locate the local environment and copy it to lbuf 		    */

   _lmove ( 2, ENVIRON, psp, &envseg, _showds() );
   _lmove ( 256, 0, envseg, lbuf, _showds() );

/*     Then locate the program name at the end of the pathname		    */

   env = find_name ( lbuf );
   strcpy ( name, env );

   head = find_head (psp);	       /* find the psp of command.com */
   mcb = head - 1;		       /* locate memory control block */

   /* get size of block, add to current segment address + 1    */
   /* to skip over the environment block for command.com.      */

   _lmove ( 2, SIZEOFF, mcb, &blksize, _showds() );
   mcb += blksize;
   mcb++;
   end = check_end (mcb);
   if ( end )
       {
       freeall ();		       /* deallocate memory */
       return ( FALSE );	       /* and return 'not found' if we are.   */
       }

   /* now search for the next psp by looking for bytes CD 20 immediately  */
   /* after the end of the current mcb. 				  */

   while ( 1 )
       {

   /* skip over unused blocks */

       _lmove ( 2, OWNROFF, mcb, &owner, _showds() );
       while ( !owner )
	   {
	   _lmove ( 2, SIZEOFF, mcb, &blksize, _showds() );
	   mcb += blksize;
	   mcb++;
	   end = check_end ( mcb );
	   if ( end )
	       {
	       freeall ();
	       return ( FALSE );
	       }
	   _lmove ( 2, OWNROFF, mcb, &owner, _showds() );
	   }   /* end while */

    /* found a used memory block, so find the environment and copy it */
    /* into the local buffer and find the file name. If it's the same */
    /* as the name we are looking for, return with flag set. */

       _lmove ( 2, 16, mcb, &chkword, _showds() );
       if ( chkword == PSPID )
	   {
	   psp = mcb + 1;
	   _lmove ( 2, ENVIRON, psp, &envseg, _showds() );
	   _lmove ( 256, 0, envseg, lbuf, _showds() );
	   env = find_name ( lbuf );
	   if ( !strcmp ( env, name ) )  /* zero result means env = name */
	       {
	       freeall ();
	       return ( TRUE );
	       }
	   }   /* end if */

       /* fall through to here if no match found and not at end of chain */
       /* so calculate segment address of next mcb */

       _lmove ( 2, SIZEOFF, mcb, &blksize, _showds() );
       mcb += blksize;
       mcb++;
       end = check_end ( mcb );
       if ( end )
	   {
	   freeall();
	   return ( FALSE );
	   }

       }   /* end while */

}


/*--------------------------------------------------------------------------*/
/*			  Support Routines				    */
/*--------------------------------------------------------------------------*/



/*----- Find the psp of Command.com ----------------------------------------*/
/*									    */
/*     Start at the psp of the current program and work backwards using     */
/*     offset 0x16 in the psp to find the parent which invoked this	    */
/*     program.  This should only take one iteration, but may take more.    */
/*     When pointer points to itself, we have found the psp of command.com. */
/*     Return with the segment address of command.com's psp.                */
/*									    */
/*--------------------------------------------------------------------------*/

unsigned   find_head(psp)
unsigned   psp;
{
   unsigned    old_parent, new_parent;

   old_parent = new_parent = 0;
   _lmove ( 2, PARENT, psp, &new_parent, _showds() );
   while ( old_parent != new_parent )
       {
       old_parent = new_parent;
       psp = old_parent;
       _lmove ( 2, PARENT, psp, &new_parent, _showds() );
       }
   return ( new_parent );

}


/*----- Find program name in environment area ------------------------------*/
/*									    */
/*     Routine is passed a pointer to a buffer containing a copy of the     */
/*     environment strings for the current program.  The procedure is to    */
/*     look through environment strings until two nulls are found in a row. */
/*     Skip the next 2 bytes, and will be pointing to the complete pathname */
/*     of the current program.	Find the last '\' character and the next    */
/*     byte will be the first character of the program name.  Return the    */
/*     address of the start of the file name as a pointer to char.	    */
/*									    */
/*--------------------------------------------------------------------------*/

char   *find_name ( buffer )
char   *buffer;
{
   while (1)
       {
       while ( *buffer++ )	       /* look for NULL */
	   ;
       if ( *buffer == NULL )	       /* found one, see if next char is too */
	   {
	   buffer += 4; 	       /* if yes, skip to start of pathname */
	   break;		       /* and get out */
	   }
       }

   return ( rindex (buffer, '\\') + 1 );

}


/*----- Check to see if we are at the end of mcb chain ---------------------*/
/*									    */
/*     Look at the first byte in the memory control block. If it is an 'M'  */
/*     then there is another block after it. If it is a 'Z' it is the end   */
/*     of the linked list. Return TRUE if the end is found, else return     */
/*     FALSE.								    */
/*									    */
/*--------------------------------------------------------------------------*/

bool   check_end ( block )
unsigned   block;
{
   if ( _peek ( 0, block ) == 'M' )
       return ( FALSE );
   else
       return ( TRUE );
}
