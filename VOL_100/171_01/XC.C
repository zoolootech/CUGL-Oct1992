/*************************************************************************/
/*   9-26-83   Microsoft C 1.04  Conversion    WHR			 */
/*     -  \t between line numbers and text to fix indenting problem.	 */
/*     -  added option -e for output to Epson in condensed print.	 */
/*     -  toupper() and isupper() are macros, not functions.		 */
/*     -  eliminate side effect in toupper(*++arg) in main().		 */
/*     -  change alloc() to malloc().					 */
/*     -  add #define NAMES that are not in stdio.h			 */
/*     -  MS-C requires () in statement A?(c=B):(c=C)	error or not??	 */
/*									 */
/*   4-30-83   Computer Innovations C-86 1.31 Conversion    WHR 	 */
/*     -  #include filename changed to allow a disk drive prefix, D:	 */
/*     -  convert if(fprintf(...) == ERROR) lst_err(); to fprintf(..);	 */
/*     -  convert if(fopen(...) == ERROR) statements to == NULL.	 */
/*     -  C86 requires () in statement A?(c=B):(c=C)	error or not??	 */
/*     -  remove getc() == ERROR check in fil_chr().			 */
/*     -  convert file conventions from BDS C to C-86.			 */
/*     -  comment out BDS unique statements, mark revised statements.	 */
/*	  keep all BDS statements to document conversion effort.	 */
/*									 */
/**  4-19-83   BDS C Version file XC.CQ copied from Laurel RCPM    WHR	 */
/*************************************************************************/
/*							   */
/*    XC  -  A 'C' Concordance Utility                     */
/*							   */
/*    Version 1.0   January, 1982			   */
/*							   */
/*    Copyright (c) 1982 by Philip N. Hisley		   */
/*							   */
/*    Released for non-commercial distribution only	   */
/*							   */
/*    Abstract: 					   */
/*							   */
/*    'XC' is a cross-reference utility for 'C' programs.  */
/*    Its has the ability to handle nested include files   */
/*    to a depth of 8 levels and properly processes nested */
/*    comments as supported by BDS C. Option flags support */
/*    the following features:				   */
/*							   */
/*    - Routing of list output to disk			   */
/*    - Cross-referencing of reserved words		   */
/*    - Processing of nested include files		   */
/*    - Generation of listing only			   */
/*							   */
/*    Usage: xc <filename> <flag(s)>			   */
/*							   */
/*    Flags: -i 	   = Enable file inclusion	   */
/*	     -l 	   = Generate listing only	   */
/*	     -r 	   = Cross-ref reserved words	   */
/*	     -o <filename> = Write output to named file    */
/*							   */
/*    Please report bugs/fixes/enhancements to: 	   */
/*							   */
/*	      Philip N. Hisley				   */
/*	      548H Jamestown Court			   */
/*	      Edgewood, Maryland 21040			   */
/*	      (301) 679-4606				   */
/*	      Net Addr: PNH@MIT-AI			   */
/*							   */
/*							   */
/***********************************************************/

/**BDS***  #include "bdscio.h"  */                          /* BDS C */

#include <stdio.h>					      /* WHR */
#include <ctype.h>					      /* WHR */

#define  NULL	    0
#define  FALSE	    0					      /* WHR */
#define  TRUE	    1					      /* WHR */
#define  CPMEOF   0x1A		      /* end of file */       /* WHR */
#define  ERROR	  (-1)					      /* WHR */

#define  MAX_REF    5		/* maximum refs per ref-block */



#define  MAX_LEN    20		/* maximum identifier length  */
#define  MAX_WRD   749		/* maximum number of identifiers */
#define  MAX_ALPHA  53		/* maximum alpha chain heads */
#define  REFS_PER_LINE	8	/* maximum refs per line */
#define  LINES_PER_PAGE 60
#define  FF 0x0C		 /* formfeed */

struct	id_blk {
		 char  id_name[MAX_LEN];
		 struct id_blk *alpha_lnk;
		 struct rf_blk *top_lnk;
		 struct rf_blk *lst_lnk;
	       } oneid;

struct	rf_blk {
		 int  ref_item[MAX_REF];
		 int  ref_cnt;
	       } onerf;

struct id_blk *id_vector[MAX_WRD];

struct alpha_hdr { struct id_blk *alpha_top;
		   struct id_blk *alpha_lst;
		 };

struct alpha_hdr alpha_vector[MAX_ALPHA];

int	linum;		/* line number */
int	edtnum; 	/* edit line number */
int	fil_cnt;	/* active file index */
int	wrd_cnt;	/* token count */
int	pagno;		/* page number */
int	id_cnt; 	/* number of unique identifiers */
int	rhsh_cnt;	/* number of conflict hits */
int	filevl; 	/* file level  */
int	paglin; 	/* page line counter */
int	prt_ref;
char	act_fil[MAX_LEN];
char	lst_fil[MAX_LEN];
char	gbl_fil[MAX_LEN];
/*BDS**** char	  l_buffer[BUFSIZ]; */			     /* BDS C */
FILE   *l_buffer;					     /*   WHR */
int	i_flg,o_flg,r_flg,l_flg;
int	debug;
char	Epson[]  = "\x1B\x40\x0F\x1BQ\x84";                  /* WHR */
int	e_flg;						     /* WHR */
/*-------------------------------------------*/

main(argc,argv)
     int     argc;
     char    **argv;
{
     char  *arg;
     char *strcpy();
/*** int  toupper();   ***/		     /* Microsoft C   macro */
     char cc;				     /* Microsoft C */

     if (argc < 2) use_err();
     i_flg=r_flg=o_flg=l_flg=FALSE;
     debug = FALSE;
     strcpy(gbl_fil,*++argv);
     --argc;
     if(gbl_fil[0] == '-')
	  use_err();
     while(--argc != 0)
     {	  if(*(arg=*++argv) == '-')
    /*****{    switch( toupper(*++arg) )     *** side effect in Microsoft C */
	  {    switch( cc=*++arg, toupper(cc) ) 	     /* Microsoft C */
	       {   case 'I':  i_flg++;
			      break;
		   case 'R':  r_flg++;
			      break;
		   case 'L':  l_flg++;
			      break;
		   case 'O': {o_flg++;
			      if(--argc == 0) use_err();
			      strcpy(lst_fil,*++argv);
			      if(lst_fil[0] == '-') use_err();
			      if(debug) printf("lst_fil=>%s<",lst_fil);
			      break;
			     }
		   case 'D':  debug++;
			      break;
		   case 'E':  e_flg++;                            /* WHR */
			      o_flg++;
			      strcpy(lst_fil,"LPT1:");
			      break;
		   default:   use_err();
	       }
	  }
     else use_err();
     }
     if(debug) printf("\ni_flg=%d, r_flg=%d, l_flg=%d", i_flg,r_flg,l_flg);
     if(debug) printf("\no_flg=%d, debug=%d", o_flg,debug);
     if (o_flg)
/*BDS{	  if ( (l_buffer = fopen(lst_fil,"w")) == ERROR) ** output file **BDS*/

     {	  if ( (l_buffer = fopen(lst_fil,"w")) == NULL)   /*** output file ***/
	  {    printf("ERROR: Unable to create list file - %s\n",lst_fil);
	       exit(0);
	  }
	  printf("\nXC ....... 'C' Concordance Utility  v1.0\n");
	  printf("    Microsoft C 1.04,  Conversion 9-26-83\n\n");  /* WHR */
	  if (e_flg) fprintf(l_buffer,"%s",Epson);
     }
								     /* WHR */
  /**BDS***   _allocp = NULL;**/ /* initialize memory allocation pointer */
     prt_ref = FALSE;
     for(linum=0;linum < MAX_WRD;linum++) {
	  id_vector[linum] = NULL;
     }
     for(linum=0;linum < MAX_ALPHA;linum++)
     {
	  alpha_vector[linum].alpha_top =
	       alpha_vector[linum].alpha_lst = NULL;
     }
     fil_cnt = wrd_cnt = linum = 0;
     filevl=paglin=pagno=edtnum=0;
     id_cnt=rhsh_cnt=0;
     proc_file(gbl_fil);
     if(!l_flg) {
	 prnt_tbl();
	 printf("\nAllowable Symbols: %d\n",MAX_WRD);
	 printf("Unique    Symbols: %d\n",id_cnt);
     }
     if(o_flg) {
	  nl();
	  fprintf(l_buffer,"\nAllowable Symbols: %d\n",MAX_WRD);   /* WHR */
	  fprintf(l_buffer,"Unique    Symbols: %d\n",id_cnt);      /* WHR */
	  fprintf(l_buffer,"%c%c",FF,CPMEOF);                      /* WHR */
	  /*BDSif(fprintf(l_buffer,"%c",CPMEOF) == ERROR) lst_err();  **/
	  fflush(l_buffer);
	  fclose(l_buffer);
     }
}/*main.
----------------------------------------*/

strcmp(s,t)
     char s[], t[];
{
     int i;
     i=0;
     while(s[i] == t[i])
	  if(s[i++] == '\0') return (0);
     return (s[i]-t[i]);
}/*strcmp.
----------------------------------------*/


char *strcpy(s1,s2)
     char *s1, *s2;
{
     char  *temp;
     temp=s1;
     while (*s1++ = *s2++);
     return (temp);
}/*strcpy.
----------------------------------*/


lst_err()
{
     printf("\nERROR: Write error on list output file - %s\n",
     lst_fil);
     exit(0);
}/*lst_err.
-----------------------------------*/


use_err()
{
     printf("\nERROR: Invalid parameter specification\n\n");
     printf("Usage: xc <filename> <flag(s)>\n\n");
     printf("Flags: -i             = Enable file inclusion\n");
     printf("       -l             = Generate listing only\n");
     printf("       -r             = Cross-reference reserved words\n");
     printf("       -o <filename>  = Write output to named file\n");
     exit(0);
}/*use_err.
-------------------------------------------*/


proc_file(filnam)
     char    *filnam;

{
     FILE  *buffer;	   /* allocated buffer pointer */	    /* WHR */
  /***BDS***   char  *buffer;	     * allocated buffer pointer */
  /***BDS***   char  *sav_buffer;    * saved alloc buffer pointer */
     char  token[MAX_LEN]; /* token buffer */
     int   eof_flg;	   /* end-of-file indicator */
     int   tok_len;	   /* token length */
     int   incnum;	   /* included line number */
     char *strcpy();

     strcpy(act_fil,filnam);
  /*   BDS C stuff						 * WHR *
  **   if ((sav_buffer = buffer =  alloc(BUFSIZ)) == 0) {
  **	   printf("\nERROR: Unable to allocate file buffer for %s\n",filnam);
  **	   exit(0);
  **  }
  **  if (fopen(filnam,buffer)) == ERROR)   *** input file ***
  */
     if ((buffer = fopen(filnam,"r")) == NULL)  /*** input file ***/ /* WHR */
     {	  printf("\nERROR: Unable to open input file: %s\n",filnam);
	  exit(0);
     }
     if(filevl++ == 0) prt_hdr();
     eof_flg = FALSE;

     do {
	  if(get_token(buffer,token,&tok_len,&eof_flg,0))
	  {    if (debug) printf("\ntoken: %s   length: %d\n",token,&tok_len);
	       if (chk_token(token))
	       {
		     /* #include processing changed to accept drive:   WHR */
		    if (strcmp(token,"#include") == 0)
		    {	 if (get_token(buffer,token,&tok_len,&eof_flg,1))
			 {    if (debug) printf("\ntoken: %s   length: %d\n",
						  token,&tok_len);
			      if (!i_flg) continue;
			      else
			      {    incnum=edtnum;
				   edtnum=0;
				   nl();
				   proc_file(token);
				   edtnum=incnum;
				   strcpy(act_fil,filnam);

				   continue;
			      }
			 }
		    }
		    put_token(token,linum);
	       }
	  }
     } while (!eof_flg);

     filevl -= 1;
     fclose(buffer);
 /***	 free(sav_buffer);  */
}/*proc_file.
-------------------------------------------*/

get_token(g_buffer,g_token,g_toklen,g_eoflg,g_flg)
     FILE    *g_buffer;
     char    *g_token;
     int     *g_toklen;
     int     *g_eoflg;
     int     g_flg;

    /*
     *	 'getoken' returns the next valid identifier or
     *	 reserved word from a given file along with the
     *	 character length of the token and an end-of-file
     *	 indicator
     *
     */

{
     int     c;
     char    *h_token;
     char    tmpchr;
     char    tmpchr2;		      /* WHR fix for B:filename.ext */

     h_token = g_token;

 gtk:			       /* top of loop, get new token */
     *g_toklen = 0;
     g_token = h_token;

    /*
     *	Scan and discard any characters until an alphabetic or
     *	'_' (underscore) character is encountered or an end-of-file
     *	condition occurs
     */

     while(  (!isalpha(*g_token = rdchr(g_buffer,g_eoflg,g_flg)))
	     &&  !*g_eoflg	&&  *g_token != '_'
	     && *g_token != '0' &&  *g_token != '#' );
     if(*g_eoflg) return(FALSE);

     *g_toklen += 1;

    /*
     *	 Scan and collect identified alpanumeric token until
     *	 a non-alphanumeric character is encountered or and
     *	 end-of-file condition occurs
     */

     if(g_flg) {
	  tmpchr  = '.';
	  tmpchr2 = ':';                   /* WHR fix for B:filename.ext */
     }
     else {
	  tmpchr  = '_';
	  tmpchr2 = '_';                   /* WHR fix for B:filename.ext */
     }
     while( (isalpha(c=rdchr(g_buffer,g_eoflg,g_flg)) ||
	    isdigit(c) || c == '_' || c == tmpchr || c == tmpchr2)
	    && !*g_eoflg)		   /* WHR fix for B:filename.ext */
     {	  if(*g_toklen < MAX_LEN)
	  {    *++g_token = c;
	       *g_toklen += 1;
	  }
     }


    /*
     *	    Check to see if a numeric hex or octal constant has
     *	    been encountered ... if so dump it and try again
     */


     if (*h_token == '0') goto gtk;


    /*
     *	    Tack a NULL character onto the end of the token
     */

     *++g_token = NULL;

    /*
     *	    Screen out all #token strings except #include
     */

     if (*h_token == '#' && strcmp(h_token,"#include")) goto gtk;

     return (TRUE);
}/*get_token.
-----------------------------------------*/

fil_chr(f_buffer,f_eof)
     FILE *f_buffer;
     int *f_eof;
{
     int fc;
     fc=getc(f_buffer);
/*   if(fc == ERROR)	   **** BDS stuff ****			     ** WHR **
**   {	  printf("\nERROR: Error while processing input file - %s\n",
**	  act_fil);
**	  exit(0);
**   }
*/
     if (fc == CPMEOF || fc == EOF) {
	  *f_eof = TRUE;
	  fc = NULL;
     }
     return(fc);
}/*fil_chr.
----------------------------------------*/

rdchr(r_buffer,r_eoflg,rd_flg)
     int     *r_eoflg;
     FILE    *r_buffer;
     int     rd_flg;

/*
	'rdchr' returns the next valid character in a file
	and an end-of-file indicator. A valid character is
	defined as any which does not appear in either a
	commented or a quoted string ... 'rdchr' will correctly
	handle comment tokens which appear within a quoted
	string
*/

{
     int     c;
     int     q_flg;	     /* double quoted string flag */
     int     q1_flg;	     /* single quoted string flag */
     int     cs_flg;	     /* comment start flag */
     int     ce_flg;	     /* comment end flag */
     int     c_cnt;	     /* comment nesting level */
     int     t_flg;	     /* transparency flag */

     q_flg = FALSE;
     q1_flg = FALSE;
     cs_flg = FALSE;
     ce_flg = FALSE;
     t_flg = FALSE;
     c_cnt  = 0;

rch:


    /*
     *	 Fetch character from file
     */

     c = fil_chr(r_buffer,r_eoflg);

     if (*r_eoflg) return(c);	/* EOF encountered */
     if (c == '\n')
	   nl();
     else
	  if (o_flg)
	       fprintf(l_buffer,"%c",c);                        /* WHR */
      /* BDSC  {    if (fprintf(l_buffer,"%c",c) == ERROR)      ** WHR **
       *	     lst_err();
       *       }
       */
	  else
	       printf("%c",c);

     if (rd_flg) return(c);

     if (t_flg) {
	  t_flg = !t_flg;
	  goto rch;
     }

     if (c == '\\') {
	  t_flg = TRUE;
	  goto rch;
     }
	/*
	If the character is not part of a quoted string
	check for and process commented strings...
	nested comments are handled correctly but unbalanced
	comments are not ... the assumption is made that
	the syntax of the program being xref'd is correct
	*/

     if (!q_flg  &&  !q1_flg) {
	  if (c == '*'  &&  c_cnt  &&  !cs_flg) {
	       ce_flg = TRUE;
	       goto rch;
	  }
	  if (c == '/'  &&  ce_flg) {
	       c_cnt -= 1;
	       ce_flg = FALSE;
	       goto rch;
	  }
	  ce_flg = FALSE;
	  if (c == '/') {
	       cs_flg = TRUE;
	       goto rch;
	  }
	  if (c == '*'  &&  cs_flg) {
	       c_cnt += 1;
	       cs_flg = FALSE;
	       goto rch;
	  }
	  cs_flg = FALSE;

	  if (c_cnt) goto rch;
     }

	/*
	Check for and process quoted strings
	*/

     if ( c == '"'  &&  !q1_flg) {      /* toggle quote flag */
	  q_flg =  !q_flg;
	  if(debug) printf("\nq_flg toggled to: %d\n" ,q_flg);
	  goto rch;
     }
     if (q_flg) goto rch;

     if (c == '\'') {        /* toggle quote flag */
	  q1_flg = !q1_flg;
	  if(debug) printf("\nq1_flg toggled to: %d\n" ,q1_flg);
	  goto rch;
     }
     if (q1_flg) goto rch;

	/*
	Valid character ... return to caller
	*/

     return (c);
}/*rdchr.
-----------------------------------------------*/


chk_token(c_token)
     char    *c_token;
{
     char  u_token[MAX_LEN];
     int   i;

     {
      if (r_flg) return(TRUE);
      i = 0;
      do { u_token[i] = toupper(c_token[i]);
      } while (c_token[i++] != NULL);

      switch(u_token[0]) {
	case 'A': if (strcmp(u_token,"AUTO") == 0) return(FALSE);
		  break;
	case 'B': if (strcmp(u_token,"BREAK") == 0) return(FALSE);
		  break;
	case 'C': if (strcmp(u_token,"CHAR") == 0) return (FALSE);
		  if (strcmp(u_token,"CONTINUE") == 0) return (FALSE);
		  if (strcmp(u_token,"CASE") == 0) return (FALSE);
		  break;

	case 'D': if(strcmp(u_token,"DOUBLE") == 0) return(FALSE);
		  if(strcmp(u_token,"DO") == 0) return(FALSE);
		  if(strcmp(u_token,"DEFAULT") == 0) return(FALSE);
		  break;
	case 'E': if(strcmp(u_token,"EXTERN") == 0) return(FALSE);
		  if(strcmp(u_token,"ELSE") == 0) return(FALSE);
		  if(strcmp(u_token,"ENTRY") == 0) return(FALSE);
		  break;
	case 'F': if(strcmp(u_token,"FLOAT") == 0) return(FALSE);
		  if(strcmp(u_token,"FOR") == 0) return(FALSE);
		  break;
	case 'G': if(strcmp(u_token,"GOTO") == 0) return(FALSE);
		  break;
	case 'I': if(strcmp(u_token,"INT") == 0) return(FALSE);
		  if(strcmp(u_token,"IF") == 0) return(FALSE);
		  break;
	case 'L': if(strcmp(u_token,"LONG") == 0) return(FALSE);
		  break;
	case 'R': if(strcmp(u_token,"RETURN") == 0) return(FALSE);
		  if(strcmp(u_token,"REGISTER") == 0) return(FALSE);
		  break;
	case 'S': if(strcmp(u_token,"STRUCT") == 0) return(FALSE);
		  if(strcmp(u_token,"SHORT") == 0) return(FALSE);
		  if(strcmp(u_token,"STATIC") == 0) return(FALSE);
		  if(strcmp(u_token,"SIZEOF") == 0) return(FALSE);
		  if(strcmp(u_token,"SWITCH") == 0) return(FALSE);
		  break;
	case 'T': if(strcmp(u_token,"TYPEDEF") == 0) return(FALSE);
		  break;
	case 'U': if(strcmp(u_token,"UNION") == 0) return(FALSE);
		  if(strcmp(u_token,"UNSIGNED") == 0) return(FALSE);
		  break;
	case 'W': if(strcmp(u_token,"WHILE") == 0) return(FALSE);
		  break; }
	}
  return (TRUE);
}/*chk_token.
---------------------------------------------*/


  /*
   *	Install parsed token and line reference in linked structure
   */

put_token(p_token,p_ref)
     char *p_token;
     int  p_ref;
{
     int  hsh_index;
     int  i;
     int  j;
     int  d;
     int  found;
     struct id_blk *idptr;
     struct rf_blk *rfptr;
     struct id_blk *alloc_id();
     struct rf_blk *alloc_rf();
     struct rf_blk *add_rf();

     if (l_flg) return;
     j=0;
     for (i=0; p_token[i] != NULL; i++)  /* Hashing algorithm is far from */
     {					 /* optimal but is adequate for a */
	 j = j * 10 + p_token[i];	 /* memory-bound index vector!	  */
     }
     hsh_index = abs(j) % MAX_WRD;

     found = FALSE;
     d = 1;
     do
     {	  idptr = id_vector[hsh_index];
	  if (idptr == NULL)
	  {    id_cnt++;
	       idptr = id_vector[hsh_index] = alloc_id(p_token);
	       chain_alpha(idptr,p_token);
	       idptr->top_lnk = idptr->lst_lnk = alloc_rf(p_ref);
	       found = TRUE;
	  }
	  else
	  if (strcmp(p_token,idptr->id_name) == 0)
	  {    idptr->lst_lnk = add_rf(idptr->lst_lnk,p_ref);
	       found = TRUE;
	  }
	  else
	  {    hsh_index += d;
	       d += 2;
	       rhsh_cnt++;
	       if (hsh_index >= MAX_WRD)
		    hsh_index -= MAX_WRD;
	       if (d == MAX_WRD)
	       {    printf("\nERROR: Symbol table overflow\n");
		    exit(0);
	       }
	  }
     } while (!found);
}/*put_token.
--------------------------------------------*/


chain_alpha(ca_ptr,ca_token)
     struct id_blk *ca_ptr;
     char  *ca_token;
{
     char  c;
     int   f;
     struct id_blk *cur_ptr;
     struct id_blk *lst_ptr;
/*** int isupper();   ****/	       /** Microsoft C	 macro **/


     c = ca_token[0];
     if (c == '_')  c = 0;
     else
     /**  isupper(c) ? c=1+((c-'A')*2) : c=2+((c-'a')*2) ;     error or not??
      **  A good one for the puzzle book! Is the () required around (c=..)?
      **  C86 and Microsoft C both req the ()'s, BDS C did not.
      **  Is it required because = has lower precedence than ?: ????
      **/
	  isupper(c) ? (c=1+((c-'A')*2)) : (c=2+((c-'a')*2)) ;

     if(alpha_vector[c].alpha_top == NULL)
     {	  alpha_vector[c].alpha_top =
		  alpha_vector[c].alpha_lst = ca_ptr;
	  ca_ptr->alpha_lnk = NULL;
	  return;
     }

    /*	check to see if new id_blk should be inserted between
     *	the alpha_vector header block and the first id_blk in
     *	the current alpha chain
     */

     if(strcmp(alpha_vector[c].alpha_top->id_name,ca_token) >0)
     {	  ca_ptr->alpha_lnk=alpha_vector[c].alpha_top;
	  alpha_vector[c].alpha_top=ca_ptr;
	  return;
     }

     if(strcmp(alpha_vector[c].alpha_lst->id_name,ca_token) < 0)
     {	  alpha_vector[c].alpha_lst->alpha_lnk = ca_ptr;
	  ca_ptr->alpha_lnk = NULL;
	  alpha_vector[c].alpha_lst=ca_ptr;
	  return;
     }

     cur_ptr = alpha_vector[c].alpha_top;
     while(strcmp(cur_ptr->id_name,ca_token) < 0)
     {	  lst_ptr = cur_ptr;
	  cur_ptr = lst_ptr->alpha_lnk;
     }

     lst_ptr->alpha_lnk = ca_ptr;
     ca_ptr->alpha_lnk = cur_ptr;
     return;
}/*chain_alpha.
-----------------------------------------*/




     struct id_blk
*alloc_id(aid_token)
     char  *aid_token;
{
     int  ai;
     struct id_blk *aid_ptr;
     char *malloc();					     /* Microsoft C */

/*** if((aid_ptr =  alloc(sizeof(oneid))) == 0) { ***/	     /* Microsoft C */
     if((aid_ptr = (struct id_blk *) malloc(sizeof(oneid))) == 0) { /* MS C */

	  printf("\nERROR: Unable to allocate identifier block\n");
	  exit(0);
     }
     ai=0;
     do {
	  aid_ptr->id_name[ai] = aid_token[ai];
     } while (aid_token[ai++] != NULL);
     return (aid_ptr);
}/*id_blk.
-----------------------------------------*/

     struct rf_blk
*alloc_rf(arf_ref)
     int  arf_ref;
{
    int ri;
    struct rf_blk *arf_ptr;
    char * malloc();				       /* Microsoft C */

/** if((arf_ptr = alloc(sizeof(onerf))) == 0) { **/    /* Microsoft C */
    if((arf_ptr = (struct rf_blk *) malloc(sizeof(onerf))) == 0) { /* MS C */
	 printf("\nERROR: Unable to allocate reference block\n");
	 exit(0);
    }
    arf_ptr->ref_item[0] = arf_ref;
    arf_ptr->ref_cnt = 1;
    for (ri=1;ri<MAX_REF;ri++)
	  arf_ptr->ref_item[ri] = NULL;
    return (arf_ptr);
}/*alloc_rf.
------------------------------------------*/


     struct rf_blk
*add_rf(adr_ptr,adr_ref)

     struct rf_blk *adr_ptr;
     int adr_ref;
{
     struct rf_blk *tmp_ptr;

     tmp_ptr = adr_ptr;
     if(adr_ptr->ref_cnt == MAX_REF) {
     /*** tmp_ptr = adr_ptr->ref_cnt = alloc_rf(adr_ref);    Microsoft C **/
	  adr_ptr->ref_cnt = alloc_rf(adr_ref); 	  /* Microsoft C **/
	  tmp_ptr = (struct rf_blk *) adr_ptr->ref_cnt;   /* Microsoft C **/
     }
     else
     {	  adr_ptr->ref_item[adr_ptr->ref_cnt++] = adr_ref;
     }
     return (tmp_ptr);
}/*rf_blk.
------------------------------------------*/

prnt_tbl()
{
     int prf_cnt;
     int pti;
     int pref;
     int lin_cnt;
     struct id_blk *pid_ptr;
     struct rf_blk *ptb_ptr;

     prt_ref = TRUE;
     prt_hdr();
     for (pti=0;pti<MAX_ALPHA;pti++)
     {	  if ((pid_ptr = alpha_vector[pti].alpha_top) != NULL)
	  {    do
	       {    if(o_flg)
			 fprintf(l_buffer,"%-14.13s: ",pid_ptr->id_name);/*WHR*/
	   /* BDSC  {	 if(fprintf(l_buffer,"%-14.13s: ",
	    *			    pid_ptr->id_name) == ERROR)
	    *		 lst_err();
	    *	    }
	    */
		    else
			 printf("%-14.13s: ",pid_ptr->id_name);
		    ptb_ptr=pid_ptr->top_lnk;
		    lin_cnt=prf_cnt=0;
		    do
		    {	 if(prf_cnt == MAX_REF)
			 {    prf_cnt=0;
			 /*** ptb_ptr = ptb_ptr->ref_cnt;  Microsoft C **/
			      ptb_ptr = (struct rf_blk *)ptb_ptr->ref_cnt;
			 }
			 if(ptb_ptr > MAX_REF)
			 {
			      if((pref=ptb_ptr->ref_item[prf_cnt++]) != 0)
			      {    if(o_flg)
				     fprintf(l_buffer,"%-4d  ",pref); /*WHR*/
			  /* BDSC  { if(fprintf(l_buffer,"%-4d  ",pref)
			   *			 == ERROR) lst_err();
			   *	   }
			   */
				   else
					printf("%-4d  ",pref);
				   if (++lin_cnt == REFS_PER_LINE)
				   {	nl();
					if(o_flg)
					   fprintf(l_buffer,"\t\t");   /*WHR*/
			    /* BDSC	{  if(fprintf(l_buffer,"\t\t")
			     *			 == ERROR) lst_err();
			     *		}
			     */
					else
					     printf("\t\t");
					lin_cnt=0;
				   }
			      }
			 }
			 else pref=0;
		    } while (pref);
		    if (lin_cnt = 0)
			 nl();
		    else
		    {	 nl();	  /**  nl(); ***single space****/
		    }
	       } while ((pid_ptr=pid_ptr->alpha_lnk) != NULL);
	  }
     }
}/*prnt_tbl.
---------------------------------------*/


prt_hdr()

{
     if (pagno++ != 0)
     if(o_flg)
	  fprintf(l_buffer,"%c",FF);                          /* WHR */
/*BDS{	  if(fprintf(l_buffer,"%c",FF) == ERROR) lst_err();
 *   }
 */
     else printf("%c",FF);

     if(o_flg)
	  fprintf(l_buffer,
		    "XC ... 'C' Concordance Utility : %s\t\t\t\t Page %d",
		    gbl_fil,pagno);			       /* WHR */
/*BDS{	  if(fprintf(l_buffer,
 *		    "XC ... 'C' Concordance Utility : %s\t\t\t\t Page %d",
 *		    gbl_fil,pagno) == ERROR) lst_err();
 *   }
 */
     else
	  printf("XC ... 'C' Concordance Utility : %s\t\t\t\t Page %d",
		  gbl_fil,pagno);
     if (o_flg)
	  fprintf(l_buffer,"\n\n");                               /* WHR */
/*BDS{	  if(fprintf(l_buffer,"\n\n") == ERROR) lst_err();
 *   }
 */
     else     printf("\n\n");

     nl();
     paglin =3;
     return;
}/*prt_hdr.
--------------------------------------*/


nl()

{
     if (o_flg)
	  fprintf(l_buffer,"\n");                                /* WHR */
/*BDS{	  if(fprintf(l_buffer,"\n") == ERROR) lst_err();
 *   }
 */
     else
	  printf("\n");

     if (++paglin == LINES_PER_PAGE) prt_hdr();
     else
	  if(!prt_ref)
	  {    if(o_flg)
		    fprintf(l_buffer,"%-4d %4d:\t",
				++linum,++edtnum);		 /* WHR */
/*BDS	       {    if (fprintf(l_buffer,"%-4d %4d:  ",
 *				++linum,++edtnum) == ERROR)
 *			 lst_err();
 *	       }
 */
	       else
		    printf("%-4d %4d:\t",++linum,++edtnum);

	       if(o_flg)
		    if(linum % 60 == 1)
			 printf("\n<%d>\t",linum);
		    else
			 printf(".");
	  }
     return;
}/*nl.
-------------------------------------------*/
/*============= end of file xc.c ==========================*/
                                            