/****************************************************************
* RBBSCIO.C							*

Release 5: RBBS 4.1 Edit 00  27 Feb 84
Release 2: RBBS 4.0 Edit 19  30 Jan 84
Release 1: RBBS 4.0 Edit 18  29 Jan 84

* This file contains the functions:

* outstr	calls bputs after checking for pause/abort.  Has
                a variety of CRLF options

* instr		calls getinp after checking for pending typeahead

* getinp	line editor input

* backrub	bputs BS-SP-BS

* backspace	outputs n backspaces

* space		outputs n spaces

* bputs		outputs string via bios with tab expansion and
                maintains character count

* putic		outputs mask character string.  called by getinp

* hlplined	help text, called by ^V from getinp

* linedisp	outputs string of real or mask characters -
                called internally by getinp

* subdel	internal getinp function for BS and DEL

* crlf		displays n CRLFs

* upstr		UPPERcases string, removing leading and trailing blanks

* capstr	Capitalizes string, removing leading and trailing blanks

* getyn		prompts with string, gets a Y or N or CR reply

****************************************************************/
#include	<bdscio.h>
#include	"rbbs4.h"
/****************************************************************/
/* Some assumed globals (in order of appearance):

char	pp[SECSIZ];	/* Previous Prompt string		*/
int	ocnt;		/* Output character count		*/
int	colcnt;		/* Horizontal cursor position		*/
int	belflg;		/* BELL Flag				*/
int	cflg;		/* Character Flag:
				1 = don't allow typeahead
				2 = use Mask Characters
				3 = output CRLF after input	*/
char	sav[SECSIZ];	/* Typeahead buffer			*/
int	icnt;		/* Input character count		*/

****************************************************************/
/****************************************************************/
outstr(output,flag)
char	*output;
int	flag;		/* 0 = no special action
			   1 = CRLF after output only
			   2 = CRLF before and after output
			   3 = CRLF before output
			   4 = CRLF after output and put into pp
			   5 = put into pp only - no CRLFs	*/
{
	char	b1;

	if (bios(2))
	{
		b1 = toupper(bios(3));
		if ((b1 == 0x0B) || (b1 == 'K'))
			return TRUE;
		if ((b1 == 0x13) || (b1 == 'S'))
			b1 = bios(3);
	}
	if ( (flag >= 2) && (flag <= 4 ) )
		crlf(1);
	bputs(output);		/* Print the string already		*/
	if ( (flag == 1) || (flag == 2) )
		crlf(1);
	if ( (flag >= 4) )
		strcpy(pp,output);/* Save in pp for possible reprompt	*/
	ocnt += strlen(output);
	return FALSE;
}
/****************************************************************/
instr(oldline,input,limit)
char	*input;
char	*oldline;
int	limit;
{
	int	cmdflg;
	char	temp[SECSIZ];

	if ( (belflg) && !(*sav) )
	{
		bios(4,7);
		++ocnt;
	}
	cmdflg = TRUE;
	if (!*sav)
	{				/* If no pending input	*/
		strcpy(sav,oldline);
		getinp(limit);
		if (cflg == 3)
			crlf(1);
		cmdflg = FALSE;
	}
	if ( (cflg == 1) || (index(sav,";") == ERROR) )
	{
		strcpy(temp,sav);
		setmem(sav,SECSIZ,0);
	}
	else
		if (sscanf(sav,"%s;%s",temp,sav) == 1)
			setmem(sav,SECSIZ,0);
	*(temp+limit) = 0;
	strcpy(input,temp);
	if (cmdflg)
		bputs(input);
	icnt += strlen(input);
	cflg = 0;
}
/****************************************************************/
getinp(limit)			/* This code is not too bad	*/
int	limit;
{
	int	chc;		/* Input character count	*/
	int	dc;		/* DEL flag			*/
	char	ic;		/* Integer Mask Character	*/
	char	nch;		/* Input character		*/
	int	i;		/* Loop counter			*/
	int	j;		/* Loop counter			*/
	int	k;
	int	l;
	char	kbuff[SECSIZ];	/* Kill buffer			*/
	int	kcnt;		/* kbuff pointer		*/
	int	iflg;		/* Insert Mode Flag		*/
	int	sf;		/* Semicolon Flag		*/
	int	tabcnt;		/* TAB space expansion counter	*/
	int	yankcnt;	/* Yank buffer counter		*/

	iflg = -1;		/* Initialize Insert Mode Flag	*/
	ic = 0x31;		/* Initialize Mask Character	*/
	kcnt = 0;		/* Initialize kbuff pointer	*/
	*kbuff = 0;		/* Initialize kbuff		*/
	sf = FALSE;
	tabcnt = 0;		/* Initialize tabexp counter	*/
	yankcnt = 0;		/* Initialize yank buffer cntr	*/
	colcnt = 0;		/* Initialize column count	*/

	if ( (chc = strlen(sav)) )	/* Initialize character count	*/
	{
		if (cflg != 2)
			bputs(sav);
		else
			ic = putic(0x31,chc);
	}
	dc = FALSE;		/* Initialize DEL Flag		*/

    while (TRUE)
	{
	if (yankcnt)
	{
		nch = *(kbuff+(--yankcnt));
		kcnt = yankcnt;
	}
	else
		if (!tabcnt)
			nch = bios(3);	/* Get a character from console	*/
		else
		{
			nch = ' ';	/* Get a space instead		*/
			--tabcnt;
		}
	switch(nch)
	{
		case '\r':		/* CR  Done		*/
			return;

		case '\t':
			tabcnt = 5 - (colcnt % 5);
			break;

		case 0177:		/* DEL	cancel char	*/
			if (!chc || !iflg)
				break;
			if ( cflg != 2)
			{
				bputs("\b\\\b");
				dc = TRUE;	/* Set DEL Flag	*/
			}
			else
				backrub(1);
			subdel(&chc,&iflg,&ic);
			if ( (sf) && (chc <= sf) )
				sf = FALSE;
			break;


		case '\b':		/* BS  cancel char	*/
			if (!chc || !iflg)
				break;
			backrub(1);
			dc = FALSE;		/* Clear DEL flg*/
			subdel(&chc,&iflg,&ic);
			if ( (sf) && (chc <= sf) )
				sf = FALSE;
			break;


		case 0x04:		/* ^D  cancel current char	*/
			if (!chc || (iflg == -1) )
				break;
			dc = FALSE;		/* Clear DEL flg*/
			if ( iflg == sf )
				sf = FALSE;
			movmem(sav+iflg+1,sav+iflg,chc-iflg);
			--chc;
			bputs(sav+iflg);
			space(1);
			backspace(chc-iflg+1);
			if (chc == iflg)
				iflg = -1;
			break;		/* last char	*/


		case 0x17:		/* ^W  delete previous word	*/
			if ( !chc || !iflg || (cflg == 2) )
				break;
			dc = FALSE;		/* Clear DEL flg*/
			j = 0;
			if ( iflg == -1)
			{
				k = chc;
				while ( (chc) && (*(sav+chc-1) == ' ') )
				{
					++j;
					--chc;
				}
				while ((chc) && (*(sav+chc-1) != ' ') )
				{
					++j;
					--chc;
				}
				backrub(j);
				while (j--)
				{
					*(kbuff+kcnt++) = *(sav+(--k));
					*(sav+k) = 0;	/* Wipe out	*/
				}			/* last char	*/
				if ( (sf) && (chc <= sf) )
					sf = FALSE;
			}
			else
			{
				k = chc;
				l = iflg;
				while ( (iflg) && (*(sav+iflg-1) == ' ') )
				{
					++j;
					--iflg;
					--chc;
				}
				while ( (iflg) && (*(sav+iflg-1) != ' ') )
				{
					++j;
					--iflg;
					--chc;
				}
				backrub(j);
				for ( i = l-1; i >= iflg; i--)
					*(kbuff+kcnt++) = *(sav+i);
				movmem(sav+l,sav+iflg,k-l+1);
				bputs(sav+iflg);
				space(j);
				backspace(k-iflg);
				if(sf && (sf >= iflg) && (sf <= k))
					sf = FALSE;
			}
			break;


		case 0x19:		/* ^Y  Yank Kill Buffer		*/
			if ( (!chc && !kcnt) || (cflg == 2) )
				break;
			if ( iflg == -1)
			{
				yankcnt = kcnt;
				break;
			}
			else
			{
				k = 0;
				for ( i = kcnt-1; i >= 0; i--)
				{
					j = *(kbuff+i);
					if ( (j == ';') && (!sf) && (cflg != 1) )
						sf = chc;
					if ( (chc == SECSIZ-3) || ((chc == limit) && (!sf)))
					{
						bios(4,7);
						++ocnt;
						break;
					}
					movmem(sav+iflg,sav+iflg+1,++chc-iflg);
					*(sav+iflg++) = j;
					++k;
				}
				bputs(sav+iflg-k);
				backspace(chc-iflg);
				kcnt = 0;
				*kbuff = 0;
				dc = FALSE;
			}
			break;


		case '\f':		/* ^L  Redisplay on same line	*/
			if (!chc)
				break;
			i = chc;
			if ( iflg > 0 )
				i = iflg;
			if ( chc && iflg )
				backrub(i);
			ic = linedisp(&chc,&iflg);
			dc = FALSE;
			break;


		case 0x12:		/* ^R  Redisplay on next line	*/
			crlf(1);
			bputs(pp);
			ocnt += strlen(pp);
			ic = linedisp(&chc,&iflg);
			dc = FALSE;
			break;


		case 0x15:		/* ^U  Cancel and redisplay prompt */
			bputs(sav+iflg);
			bputs(" #\r\n");
			bputs(pp);
			if (!kcnt)
				for ( i = chc-1; i >= 0; i--)
					*(kbuff+kcnt++) = *(sav+i);
			iflg = -1;
			chc = 0;
			*sav = 0;
			ic = 0x31;
			dc = FALSE;
			sf = FALSE;
			colcnt = 0;
			break;


		case 0x18:		/* ^X  Cancel line inplace	*/
			if (!chc)
				break;
			if ( iflg >= 0 )
				bputs(sav+iflg);
			backrub(chc);
			if (!kcnt)
				for ( i = chc-1; i >= 0; i--)
					*(kbuff+kcnt++) = *(sav+i);
			iflg = -1;
			chc = 0;
			*sav = 0;
			ic = 0x31;
			dc = FALSE;
			sf = FALSE;
			colcnt = 0;
			break;


		case 0x0b:		/* ^K  Kill to end of line	*/
			if (!chc || (iflg == -1) )
				break;
			bputs(sav+iflg);
			backrub(chc-iflg);
			for ( i = chc-1; i >= iflg; i--)
				*(kbuff+kcnt++) = *(sav+(--chc));
			*(sav+chc) = 0;
			if ( (sf) && (chc <= sf) )
				sf = FALSE;
			iflg = -1;
			break;


		case 0x02:		/* ^B  Move back inplace	*/
			if (!chc || !iflg || (cflg == 2) )
				break;
			bios(4,'\b');
			++ocnt;
			--colcnt;
			if ( iflg >= 0 )
				--iflg;
			else
				iflg = chc - 1;
			break;


		case 0x06:		/* ^F  Move forward inplace	*/
			if ( iflg == -1 )
				break;
			bios(4,*(sav+iflg++));
			++ocnt;
			++colcnt;
			if (chc == iflg)
				iflg = -1;
			break;


		case 0x01:		/* ^A  Move to beginning	*/
			if (!chc || !iflg || (cflg == 2) )
				break;
			i = chc;
			if ( iflg > 0 )
				i = iflg;
			backspace(i);
			iflg = 0;
			break;


		case 0x05:		/* ^E  Move to end of line	*/
			if ( (!chc) || (iflg == -1) )
				break;
			else
				bputs(sav+iflg);
			iflg = -1;
			break;

		case 0x16:		/* ^View  Help!			*/
			hlplined();
			crlf(1);
			bputs(pp);
			colcnt = 0;
			ic = linedisp(&chc,&iflg);
			dc = FALSE;
			break;

		default:
			if (nch < 0x20)
				break;
			if ( (nch == ';') && (!sf) && (cflg != 1) )
				sf = chc;
			if ( (chc == SECSIZ-3) || ((chc == limit) && (!sf)))
			{
				bios(4,7);
				++ocnt;
				break;
			}
			if ( iflg >= 0 )
			{
				movmem(sav+iflg,sav+iflg+1,++chc-iflg);
				*(sav+iflg++) = nch;
			}
			else
			{
				*(sav+chc++) = nch;
				*(sav+chc) = 0;
			}
			if (dc)
			{
				bios(4,'\n');
				++ocnt;
			}
			if (cflg != 2)
			{
				bios(4,nch);
				++ocnt;
				++colcnt;
				if ( iflg >= 0 )
				{
					bputs(sav+iflg);
					backspace(chc-iflg);
				}
			}
			else
			{
				bios(4,ic++);
				++ocnt;
				++colcnt;
				if (ic == 0x3a)
					ic = 0x30;
				if ( iflg >= 0 )
				{
					ic = putic(ic,chc-iflg+1);
					backspace(chc-iflg+1);
				}
			}
			dc = FALSE;
	}
    }
}
/****************************************************************/
backrub(i)
int	i;
{
	while (i--) bputs("\b \b");
}
/****************************************************************/
backspace(i)
int	i;
{
	while (i--)
	{
		bios(4,'\b');
		++ocnt;
		--colcnt;
	}
}
/****************************************************************/
space(i)
int	i;
{
	while (i--)
	{
		bios(4,' ');
		++ocnt;
		++colcnt;
	}
}
/****************************************************************/
bputs(s)
char	*s;
{
	int	j;

	while (*s)
	{
		if (*s == '\r')
			colcnt = 0;
		if ( (*(s-1) == '\r') && (*s == '\n') )
			colcnt = -1;
		if ( (*(s-1) != '\r') && (*s == '\n') )
			--colcnt;
		if (*s == '\t')
		{
			*s++;
			j = 8 - (colcnt % 8);
			space(j);
		}
		else
		{
			if (*s == '\b')
				--colcnt;
			else
				++colcnt;
			bios(4,*s++);
		}
		++ocnt;
	}
}
/****************************************************************/
char	*putic(ic,cnt)
char	ic;
int	cnt;
{
	while (cnt--)
		{
			bios(4,ic++);
			++ocnt;
			++colcnt;
			if ( ic == 0x3a )
				ic = 0x30;
		}
	return ic;
}
/****************************************************************/
hlplined()
{
	if (cflg != 2)
		bufinmsg("RCIOHLP0");
	else
		bufinmsg("RCIOHLP2");
}
/****************************************************************/
char	*linedisp(chc,iflg)
int	*chc;
int	*iflg;
{
	char	ic;

	if (cflg != 2)
		bputs(sav);
	else
		ic = putic(0x31,*chc);
	if ( *iflg >= 0 )
		backspace(*chc-*iflg);
	return ic;
}
/****************************************************************/
subdel(chc,iflg,ic)
int	*chc;
int	*iflg;
char	*ic;
{
	if (--*ic == 0x2f)
		*ic = 0x39;
	if ( *iflg == -1)
		*(sav+(--*chc)) = 0;
	else
	{
		--*iflg;
		movmem(sav+*iflg+1,sav+*iflg,*chc-*iflg);
		--*chc;
		if ( cflg != 2)
			bputs(sav+*iflg);
		else
			*ic = putic(*ic,*chc-*iflg);
		space(1);
		backspace(*chc-*iflg+1);
	}
}
/****************************************************************/
crlf(i)
int	i;
{
	while(i--) 
		bputs("\r\n");
}
/****************************************************************/
upstr(s)	/* Capitalize (one word) string			*/
char	*s;	/* and flush ALL spaces - DS			*/
{
 	int	i;
	char	*p1,*p2;

/* Delete ALL spaces from string */

	for ( p1 = p2 = s; *p2; p2++)
	{
		if ( *p2 != ' ')
			*(p1++) = *p2;
	}
	*p1 = 0;	/* Terminate possibly shortened string	*/

/* Convert first char to upper, remainder of string to lower	*/

	p1 = s;		/* Re-set pointer to start of string	*/
	*p1 = toupper(*(p1++));
	while ( *p1 = tolower(*(p1++)) );

	return s;	/* Return original pointer		*/
}
/****************************************************************/
capstr(s)	/* Strips leading and trailing spaces		*/
char	*s;	/* & raises string to CAPS			*/
{
	int	i;
	char	*p;

	p = s;
	while (*p == ' ')
		for ( i = 0; *(p+i); i++)
			*(p+i) = *(p+i+1);
	while (*p = toupper(*p)) p++;
	--p;
	while (*p == ' ')
	{
		*p = 0;
		p--;
	}
	return s;
}
/****************************************************************/
getyn (prom)		 	/* --<PROMPT 4 Y/N ANSWER>-- */
char	*prom;
{
	char	ans;
	char	ans2[2];
	char	prompt[SECSIZ];

	sprintf(prompt,"%s? (Y/N)? ",prom);
	outstr(prompt,5);
	do
	{
		instr("",ans2,1);
		ans = toupper(*ans2);
		if (!*ans2)
		{
			ans = 'Y';
			space(1);
		}
	}	while (ans != 'Y' && ans != 'N');

	if ( ans == 'Y' )
		outstr("\bYES",1);
	else
		outstr("\bNO",1);
	return (ans == 'Y' ? YES : NO);
}
/***************************************************************/

	while ( *p1 = tolower(*(p1++)) );

	return s;	/* Return original 