/*XSHOWFNS.C   VERS:- 01.00  DATE:- 09/26/86  TIME:- 09:37:48 PM */
/*
%CC1 $1.C 
%CLINK XSHOWR XSHOWFNS WILDEXP -F CHARFUN STRFUN -S -E5500
*/
/* 
Description:

Functions for XSHOWR.C


By J.A. Rupley, Tucson, Arizona
Coded for BDS C compiler, version 1.50a
*/

#include <bdscio.h>

#include <xshow.h>


        /*page eject*/

int down_block()
{
	gotop_display(0);
	backup(2);
	if (disp_pntr > mid_addr)
		disp_pntr = mid_addr;
	else
	{
		if (prev_block())
		{
			disp_pntr = start_addr;
			return -1;
		}
		else
		    disp_pntr++;
	}
	disp_skip(0);
	return 0;
}


int up_block()
{
	gotop_display(0);
	if (disp_pntr < mid_addr && end_addr > mid_addr)
		disp_pntr = mid_addr;
	else
		if (next_block())
		{
			while (!disp_skip(0))
				;
			if (backup(TLENGTH))
				disp_pntr = start_addr;
			return -1;
		}
	disp_skip(0);
	return 0;
}


        /*page eject*/

int gotop_display(display)
int display;
{
	if (!top_flag)
		backup(TLENGTH);
	if (display)
		return disp_screen();
	top_flag = 1;
	return 0;
}


int gobot_display(display)
int display;
{
	int temp;

	if (top_flag)
	{
		temp = TLENGTH;
		while (temp--)
			disp_skip(0);
	}
	if (display)
		return disp_screen();
	top_flag = 0;
	return 0;
}


int disp_screen()
{
	int i, temp;

	CLEAR;
	for (temp = -TLENGTH; temp; temp++)
		if (disp_skip(1))
			break;
	CUR_BOTTOM;
	top_flag = 0;
	return temp;
}


int backup(lines)
int lines;
{
	char c;

	lines++;
	while (lines--)
	{
		c = 0;
		while (c != '\n')
		{
			c = (*disp_pntr & 0x7f);
			if (pntr_change(-1))
				return -1;
		}
	}
	disp_skip(0);
	return 0;
}


int disp_skip(display)
int display;
{
	int charcount;
	int temp;
	char c;

	c = charcount = 0;
	while (c != '\n')
	{
		if ((c = (*disp_pntr & 0x7f)) >= 0x20)
		{
			if (display && ++charcount > tabcount
			    && charcount <= (tabcount + TWIDTH))
				b_putchar(c);
		}
		else
		    {
			switch (c)
			{
			case CPMEOF :
				return -1;
			case '\n' :
				if (display)
					b_putchar('\n');
			case '\r' :
				charcount = 0;
				if (display)
					b_putchar('\r');
				break;
			case '\t' :
				if (display)
				{
					temp = (TAB_SPACES - charcount % TAB_SPACES);
					while (temp--)
						if (++charcount > tabcount && charcount <= (tabcount + TWIDTH))
							b_putchar(SPACE);
				}
				break;
			default :
				if (display && ++charcount > tabcount
				    && charcount <= (tabcount + TWIDTH - 1))
				{
					b_putchar('^');
					++charcount;
					b_putchar(c += 0x40);
				}
			}
		}
		if (pntr_change(1))
			return -1;
	}
	return 0;
}


        /*page eject*/

int load_buffer()
{
	int temp;

	seek(fd, 0, 0);
	if ((temp = read(fd, start_addr, NSECS * 2)) == ERROR)
		error("Read error in load_buffer().");

	start_flag = 1;
	disp_pntr = start_addr;
	disp_sec = temp;
	end_set(temp);
	return 0;
}


int next_block()
{
	int temp;

	if (end_flag)
		return -1;

	movmem(mid_addr, start_addr, NSECS * SECSIZ);

	seek(fd, disp_sec, 0);
	if ((temp = read(fd, mid_addr, NSECS)) == ERROR)
		error("Read error in next_block().");

	start_flag = 0;
	disp_pntr = mid_addr;
	disp_sec += temp;
	end_set(temp + NSECS);
	return 0;
}


int prev_block()
{
	int temp;

	if (start_flag)
		return -1;

	if ((temp = disp_sec % NSECS) == 0)
		temp = NSECS * 3;
	else
		temp += NSECS * 2;

	if (temp >= disp_sec)
	{
		load_buffer();
		disp_pntr = mid_addr - 1;
		return 0;
	}

	movmem(start_addr, mid_addr, NSECS * SECSIZ);

	seek(fd, (disp_sec -= temp), 0);
	if ((read(fd, start_addr, NSECS)) == ERROR)
		error("Read error in prev_block().");

	start_flag = 0;
	disp_sec += 2 * NSECS;
	disp_pntr = mid_addr - 1;
	end_set(2 * NSECS);
	return 0;
}


void end_set(offset)
int offset;
{
	char *temp_pntr;

	end_addr = start_addr + offset * SECSIZ;
	*end_addr = CPMEOF;

	if (disp_sec != end_sec)
		end_flag = 0;
	else
	    {
		if (offset)
			temp_pntr = end_addr - SECSIZ;
		else
			temp_pntr = start_addr;
		while (*temp_pntr != CPMEOF)
			temp_pntr++;
		if (*--temp_pntr != '\n')
		{
			*++temp_pntr = '\n';
			*++temp_pntr = CPMEOF;
			end_addr = temp_pntr;
		}
		end_flag = 1;
	}
}


        /* page eject*/

int change_load(n)
int n;
{
	int temp;

	end_flag = 0;

	if ((n < 0) && ((temp = disp_sec % NSECS) != 0))
	{
		temp = disp_sec - temp;
		n++;
	}
	else
		temp = disp_sec;

	if ((temp += n * NSECS) >= end_sec)
	{
		if ((end_sec % NSECS) == 0)
			disp_sec = end_sec - NSECS * 2;
		else
			disp_sec = end_sec - NSECS - end_sec % NSECS;
		if (disp_sec <= 0)
			load_buffer();
		else
		{
			seek(fd, disp_sec, 0);
			next_block();
			next_block();
			disp_skip(0);
			if ((temp - end_sec) < NSECS)
				return 1;
		}
		while (!up_block())
			;
		return 1;
	}
	else
	    if ((temp -= 2 * NSECS) >= NSECS)
	{
		seek(fd, (disp_sec = temp), 0);
		next_block();
		next_block();
	}
	else
	{
		load_buffer();
		if (temp < 0)
			return -1;
		else
		    disp_pntr = mid_addr;
	}
	disp_skip(0);
	return 0;
}


        /* page eject*/

int read_pattern()
{
	int i;
	char *tt;

	CUR_UP;
	CUR_UP;
	CUR_UP;
	CUR_UP;
	ERA_EOP;
	printf("\n:::Old decoded pattern: %s", pattern);
	printf("\n:::Old input string...: %s", old_pattern);
	printf("\n:::Enter string + -cr-: ");
	if (strlen(gets(input_pattern)))
	{
		strcpy(old_pattern, input_pattern);
		upper_case = 0;
		if (!pat_decode(input_pattern, pattern))
			return -1;
		for (tt = pattern; upper_case && *tt; tt++)
			*tt = toupper(*tt);
	}
	else
	{
		printf("\n\nDo you want to abort?  Y / (N = any other char):");
		if (toupper(b_getchar()) == 'Y')
			return -1;
	}
	return 0;
}


void srch_display()
{
	p_srch1 = f_pntr;
	p_srch2 = n_pntr;
	backup(10);
	if (disp_sec < f_sec)
	{
		p_srch1 += NSECS * SECSIZ;
		p_srch2 += NSECS * SECSIZ;
	}
	srch_out = 2;
	REV_ON;
	disp_screen();
	srch_out = 0;
	REV_OFF;
}


int pat_decode(pat, str)
char *pat;
char *str;
{
	int c;
	char *bad;

	j = i = 0;
	while (c = pat[i++])
	{
		if (c == '!')
		{
			for (c = pat[i++], bad = "*?!"; *bad; bad++)
				if (*bad == c)
					error("Bad pattern after !.");
			decode(c, pat, str, 1);
		}
		else
		    decode(c, pat, str, 0);
	}
	str[j] = 0;
	return j;
}


void decode(c, pat, str, not_flag)
int c, not_flag;
char *pat, *str;
{
	switch (c)
	{
	case '$' :
		upper_case = TRUE;
		break;
	case '*' :
		str[j++] = SKIPANY;
		break;
	case '?' :
		str[j++] = SKIPONE;
		break;
	case '@' :
		str[j++] = ANYALPHA;
		break;
	case '#' :
		str[j++] = ANYNUMBER;
		break;
	case '^' :
		c = pat[i++];
		if (c > 0x7f || c <= 0x40)
			error("Illegal ^ character.");
		str[j++] = toupper(c) - 0x40;
		break;
	case '\\' :
		c = pat[i++];
		if (c == 0)
			error("\\ requires character.");
		switch (toupper(c))
		{
		case 'N' :
			c = '\n';
			break;
		case 'F' :
			c = '\f';
			break;
		case 'R' :
			c = '\r';
			break;
		case 'T' :
			c = '\t';
			break;
		case 'B' :
			c = '\b';
			break;
		}
	default :
		str[j++] = c;
	}
	if (not_flag)
		str[j - 1] |= 0x80;
}


void set_srch(to, from)
int to, from;
{
	char *temp_pntr;
	int temp_sec;

	switch (from)
	{
	case SRCH_CHAR :
		temp_sec = disp_sec;
		temp_pntr = disp_pntr;
		break;
	case FIRST :
		temp_sec = f_sec;
		temp_pntr = f_pntr;
		break;
	case NEXT :
		temp_sec = n_sec;
		temp_pntr = n_pntr;
		break;
	}

	switch (to)
	{
	case SRCH_CHAR :
		disp_sec = temp_sec;
		change_load(0);
		disp_pntr = temp_pntr;
		break;
	case FIRST :
		f_sec = temp_sec;
		f_pntr = temp_pntr;
		break;
	case NEXT :
		n_sec = temp_sec;
		n_pntr = temp_pntr;
		break;
	}
}


        /* page eject*/

int find()
{
	int temp;

	i = 0;
	j = 0;
	set_srch(SRCH_CHAR, FIRST);
	temp = PAUSE_COUNT * 1000;

	switch (setjmp(jbufsrch))
	{
	case NOTFOUND :
		return NOTFOUND;
	case FOUND :
		set_srch(NEXT, SRCH_CHAR);
		return FOUND;
	}

loop :
	if (!temp--)
	{
		if (KBHIT)
		{
			return NOTFOUND;
		}
		temp = PAUSE_COUNT * 1000;
		printf("\rSearching......");
	}

	switch (search(*disp_pntr & 0x7f))
	{
	case NOTFOUND :
	case NOTMATCH :
		i = 0;
		j = 0;
		if (disp_pntr > f_pntr)
		{
			disp_pntr = ++f_pntr;
			goto loop;
		}
		else
		{
			if (disp_sec != f_sec)
			{
				disp_sec = f_sec;
				change_load(0);
			}
			disp_pntr = f_pntr;
			if (pntr_change(1))
				return NOTFOUND;
			f_sec = disp_sec;
			f_pntr = disp_pntr;
			if (*disp_pntr == 0x1a)
				return NOTFOUND;
			goto loop;
		}
	case MATCH :
		j++;
		if (!i++)
		{
			f_sec = disp_sec;
			f_pntr = disp_pntr;
		}
		if (pntr_change(1))
			return NOTFOUND;
		if (!pattern[i])
			longjmp(jbufsrch, FOUND);
		if (*disp_pntr == 0x1a)
			return NOTFOUND;
		goto loop;
	}
}


int pntr_change(direction)
int direction;
{
	if (direction > 0)
	{
		if (++disp_pntr >= end_addr)
		{
			if (next_block())
			{
				disp_pntr--;
				return -1;
			}
		}
	}
	else
	    {
		if (--disp_pntr < start_addr)
			if (prev_block())
			{
				disp_pntr++;
				return -1;
			}
	}
	return 0;
}


        /*page eject*/

int search(c)
int c;
{
	int tempx, isave;
	char *temp;

	if (j > 2 * MAXLINE)
		return NOTFOUND;
	if (pattern[i] & 0x80)
		switch (pattern[i])
		{
		case ANYALPHA | 0x80 :
			if (!isalpha(c))
				return MATCH;
			else
			    return NOTMATCH;
		case ANYNUMBER | 0x80 :
			if (!isdigit(c))
				return MATCH;
			else
			    return NOTMATCH;
		case SKIPANY :
			if (!pattern[++i])
				longjmp(jbufsrch, FOUND);
			isave = i;
loop :
			i = isave;
loop2 :
			switch (search(*disp_pntr & 0x7f))
			{
			case MATCH :
				j++;
				if (!pattern[++i])
				{
					i--;
					return MATCH;
				}
				if (pntr_change(1))
					longjmp(jbufsrch, NOTFOUND);
				if (c == 0x1a)
					longjmp(jbufsrch, NOTFOUND);
				goto loop2;
			case NOTMATCH :
				j++;
				if (pntr_change(1))
					longjmp(jbufsrch, NOTFOUND);
				if (c == 0x1a)
				{
					j--;
					if (pntr_change(-1))
						longjmp(jbufsrch, NOTFOUND);
					return NOTMATCH;
				}
				goto loop;
			case NOTFOUND :
				return NOTFOUND;
			}
		case SKIPONE :
			i++;
			j++;
			if (pntr_change(1))
				longjmp(jbufsrch, NOTFOUND);
			if (c == 0x1a)
				longjmp(jbufsrch, NOTFOUND);
			switch (search(*disp_pntr & 0x7f))
			{
			case MATCH :
				return MATCH;
			case NOTMATCH :
				j--;
				if (pntr_change(-1))
					longjmp(jbufsrch, NOTFOUND);
				return NOTMATCH;
			case NOTFOUND :
				return NOTFOUND;
			}
		default :
			if (upper_case && (c >= 'a')
			    && (c <= 'z'))
				if ((pattern[i] & 0x7f) != (c - 0x20))
					return MATCH;
			else
			    return NOTMATCH;
			else
			    if ((pattern[i] & 0x7f) != c )
				return MATCH;
			else
			    return NOTMATCH;
		}
	else
	    switch (pattern[i])
	{
	case 0 :
		longjmp(jbufsrch, FOUND);
	case ANYALPHA :
		if (isalpha(c))
			return MATCH;
		else
		    return NOTMATCH;
	case ANYNUMBER :
		if (isdigit(c))
			return MATCH;
		else
		    return NOTMATCH;
	default :
		if (upper_case && (c >= 'a')
		    && (c <= 'z'))
			if (pattern[i] == (c - 0x20))
				return MATCH;
		else
		    return NOTMATCH;
		else
		    if (pattern[i] == c)
			return MATCH;
		else
		    return NOTMATCH;
	}
}


        /* page eject */

int bad_type(name)
char *name;
{
	char *ss, *tt, *bad;

	bad = ".COM.OBJ.BAD.LOG.SYS.OV?.SEN.REL.CRL.LBR.ARC.NDX.DBF.ZIP.ZBA.HEX.DIC.IO .?Q*.";

	for (; *name; name++)
		if (*name == '.')
			break;
	for (; *bad; bad++)
		if (*bad == *name)
			for (ss = bad, tt = name;; ss++, tt++)
			{
				while (*tt == ' ')
					tt++;
				while (*ss == ' ')
					ss++;
				if (!*tt)
				{
					if ((*ss == '.') || (*ss == '*'))
						return -1;
					else
					    break;
				}
				if ((*ss == '?') || (*ss == '*'))
					continue;
				if (*ss != *tt)
					break;
			}
	return 0;
}


void error(format)
char *format;
{
	void b_putchar();

	CLEAR;
	printf("\n\nXSHOW: ");
	_spr(&format, &b_putchar);
	CUR_BOTTOM;
	longjmp(jbuf, 1);
}


void printf(format)
char *format;
{
	void b_putchar();
	_spr(&format, &b_putchar);        /* use "_spr" to form the output */
}


int string_compare(s, t)
char **s, **t;
{
	char *s1, *t1;

	for (s1 = *s, t1 = *t; *s1; s1++, t1++)
		if (*t1 != *s1)
			return (*s1 - *t1);
	return 0;
}


void b_putchar(cc)
int cc;
{
	if (srch_out)
	{
		if (disp_pntr < p_srch1)
		{
			REV_ON;
		}
		else
			if (disp_pntr >= p_srch2 
				&& ((f_sec == n_sec) || (disp_sec == n_sec)))
			{
				srch_out = 0;
				REV_ON;
			}
			else
			{
				REV_OFF;
			}
	}
	if (cc == '\n')
		BIOS_OUTPUT('\r');
	BIOS_OUTPUT(cc);
	if (t || KBHIT)
	{
		if ((t = b_getchar()) == CNTRL_C)
			exit();
		else
			if (t == CNTRL_S)
			{
				t = 0;
				while (KBHIT)
					;
				if ((t = b_getchar()) == CNTRL_S)
					t = 0;
			}
	}
}

int b_getchar()
{
	int temp;

	if (!t)
		temp = BIOS_INPUT;
	else
	{
		temp = t;
		t = 0;
	}
	return temp;
}


int wait_count()
{
	int count;

	for (count = PAUSE_COUNT * 3300; count; count--)
		if (t || KBHIT)
			break;
	CUR_BOTTOM;
	ERA_EOL;
	return count;
}


void delay_mess(format)
char *format;
{
	void b_putchar();

	if (wait_count())
		return;
	CUR_UP;
	ERA_EOL;
	_spr(&format, &b_putchar);
}


void pause_mess()
{
	if (wait_count())
		return;
	printf(":::block= %d+%d/%d+%d pntr= %x  file(%d)= %s -- Instruction??",
	disp_sec / NSECS, disp_sec % NSECS, end_sec / NSECS, end_sec % NSECS, (disp_pntr - start_addr), x, file);
	while (!(t || KBHIT))
		;
}


void help_mess()
{
	CLEAR;

	printf("USAGE:   XSHOW    [d?:]afn.aft1  [[!][du:]afn.aft2  ..etc..]\n");
	printf(" Wild chars; !=NOT operator; multiple files (white space for sepn); du: & d?:.\n");
	printf("\n");
	printf("Instructions:   H (also QH and KH) =  display this help message\n");
	printf("\n");
	printf("D  =  + line               QP  =  set display at previous position    \n");
	printf("S  =  - line               \n");
	printf("C  =  + screen             QC  =  set display at end of file                 \n");
	printf("R  =  - screen             QR  =  set display at start of file               \n");
	printf("X  =  + block              QXn =  + n blocks        |* block = 2K characters *|\n");
	printf("E  =  - block              QEn =  - n blocks        |* use -cr- to terminate *|\n");
	printf("F  =  tab forward                                   |* an  <n> or <s>  entry *|\n");
	printf("A  =  tab backward         QFs =  search for pattern <s>  (-L- for disp + cont)\n");
	printf("L  =  repeat command       QQ  =  continous repeat   (1=fast, 9=slow, 0=freeze)\n");
	printf("\n");
	printf("KX  =  exit to CPM\n");
	printf("KB  =  enter previous file          KNn =  enter file n of file_list         \n");
	printf("KK  =  enter next file              KO  =  display file_list, with ordinals  \n");
	printf("\n");
	printf("           meta characters allowed in search string <s>:\n");
	printf("^=>ctrl char follows   \\=>C-constant or literal ($,*,etc)   !=>not next char\n");
	printf("@=any alpha    #=any digit    *=any string    ?=any char    $=>no case test");

	pause_mess();
}


