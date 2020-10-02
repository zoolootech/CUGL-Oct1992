/*
	CPP V5 -- system module.

	Source:  sys.c
	Started: October 7, 1985
	Version:
		May 26, 1988
		August 1, 1989
			Stamp out the extra newline bug in sysnlput().

	Written by Edward K. Ream.
	This software is in the public domain.

	See the read.me file for disclaimer and other information.
*/

#include "cpp.h"

#ifdef MICRO_SOFT
#include <fcntl.h>
#include <sys\types.h>
#include <sys\stat.h>
#include <io.h>
#include <time.h>
#endif

#ifdef TURBOC
#include <fcntl.h>
#include <sys\stat.h>
#include <io.h>
#include <conio.h>
#include <time.h>
#endif

/*
	Define the format of a file node.
	The node contains all information pertaining to the file.
	The fil_name and f_line are used to update preprocessor globals.
*/
typedef enum { NULL_STAT,
	CLOSED_STAT, INPUT_STAT, OUTPUT_STAT, LOCAL_STAT, EOF_STAT
} en_stat;

static struct FN {
	struct FN * f_next;	/* pointer to next node		*/
	char *	fil_name;	/* file name			*/
	int	f_line;		/* line number			*/
	char *	f_buffer;	/* file buffer			*/
	int	f_bufsize;	/* size of file buffer: bytes	*/
	int	f_handle;	/* handle to file.		*/
	char *  f_bufp;		/* pointer into file buffer	*/
	int	f_bufc;		/* characters in buffer		*/
	en_stat	f_type;		/* status of file		*/
	int	f_lastc;	/* restart character		*/
};
#define FN_SIZE (sizeof(struct FN))

static struct FN * in_list   = NULL;	/* List of input nodes.		*/
static struct FN * outn_list = NULL;	/* List of output nodes.	*/

/*
	The following global variables are copies of the fields in FN.
	Global variables are used as an efficiency measure.
*/
static char *	s_ip = NULL;    /* Copy of f_bufp for input file.	*/
static int	s_ic = 0;    	/* Copy of f_bufc for input file.	*/
static char *	s_op = NULL;	/* Copy of f_bufp for output file.	*/
static int	s_oc = 0;	/* Copy of f_bufc for output file.	*/

#define MAX_INLEVEL 20	/* Max depth of nested #includes.	*/
#define IBUF_SIZE 2048	/* Size of each input buffer.		*/
#define OBUF_SIZE 2048	/* Size of the output buffer.		*/

/*
	Define "pushed back" characters used by sysn1().
*/
static int	push_back = -1;		/* Used by macro logic.		*/
static int	file_put_back = -1;	/* Used by non-macro logic.	*/

/*
	Define variables used by time and date routines.
*/
static	long ltime;
static	struct tm *newtime;
static char time_buf [30];
static char date_buf [30];

/*
	Declare static functions in this module.
*/
static void	raw_close	(int);
static int	raw_creat	(char *);
static int	raw_open	(char *);
static int	raw_read	(int, char *, int);
static int	raw_write	(int, char *, int);
static int	syscstat	(void);
static void	sysn1		(void);
static struct FN *sys_new	(int);
static void	sys_release	(void);

/*
	Close a file opened with raw_open() or raw_creat().
*/
static void
raw_close(handle)
int handle;
{
	TRACEP("raw_close", printf("(handle: %d)\n", handle));

	close (handle);
}

/*
	Open the file for writing only.
	Return a handle (int) or ERROR.
*/
static int
raw_creat(name)
char *name;
{

	TRACEP("raw_creat", printf("(%s)\n", name));

	chmod(name, S_IREAD | S_IWRITE);
	return creat(name, S_IREAD | S_IWRITE);
}

/*
	Open the file for reading only.
	Return a handle (int) or ERROR.
*/
static int
raw_open(name)
char *name;
{
	TRACEP("raw_open", printf("(%s)\n", name));

	return open(name, O_RDONLY | O_BINARY);
}

/*
	Read n bytes from the file described by handle into buffer[].
	Return the number of bytes read.
*/
static int
raw_read(handle, buffer, n)
int handle;
char *buffer;
int n;
{
	int result;

	TRACEP("raw_read", printf("(handle: %d, buffer: %lx, n: %d)\n",
		handle, buffer, n));
		
	result = read (handle, buffer, n);

	TRACEP("raw_read", printf("returns %d\n", result));
	return result;
}

/*
	Write n bytes from buffer[] to the file described by handle.
	Return the number of bytes written.
*/
static int
raw_write(handle, buffer, n)
int handle;
char *buffer;
int n;
{
	TRACEP("raw_write", printf("(handle: %dx, buffer: %lx, n: %d)\n",
		handle, buffer, n));
		
	return write (handle, buffer, n);	
}

/*
	Close all files and exit.

	Do not call fatal() or sysabort() from inside
	sysiclose(), sysoclose(), or sys_release().
*/
void
sysabort()
{
	/* Close the output file. */
	if (outn_list != NULL) {
		sysoclose();
	}

	/* Close all input files. */
	while(in_list != NULL) {
		sysiclose();
	}
	sysend();
	exit(BAD_EXIT);
}		

/*
	Put a non-newline to the output file.
	This is the second most called routine after sysnext().
*/
void
syscput(c)
char c;
{
	struct FN * fnp;

	*s_op++ = c;
	s_oc++;
	if (s_oc == OBUF_SIZE) {

		fnp = outn_list;
		if (raw_write(fnp->f_handle, fnp->f_buffer, OBUF_SIZE) != 
			OBUF_SIZE) {
			error("Disk full.");
			return;
		}
		
		s_oc = 0;
		s_op = fnp -> f_buffer;
	}
}

/*
	Open a file for output.  Only one such file may be open at a time.

	Return TRUE if all went well.
*/
bool
syscreat(name)
register char * name;
{
	register struct FN * fnp;
	int handle;

	TRACEP("syscreat", printf("(%s)\n", name));

	fnp = outn_list;
	if (fnp == NULL) {
		/* Allocate node for output file. */
		fnp = outn_list  = sys_new(OBUF_SIZE);
	}
	else if (fnp -> f_type != CLOSED_STAT) {
		syserr("syscreat: Can't happen.");
	}
	
	/* Actually open the file. */
	handle = raw_creat(name);
	if (handle == ERROR) {
		return FALSE;
	}
	fnp -> f_handle = handle;
	fnp -> f_type = OUTPUT_STAT;

	/* The output buffer is empty. */
	s_oc = 0;
	s_op = fnp -> f_buffer;

	return TRUE;
}

/*
	Return 0 if no character is ready from the keyboard.
	Otherwise, return the character itself.
*/
static int
syscstat()
{
	if (kbhit()) {
		return fgetchar() & 0x7f;
	}
	else {
		return 0;
	}
}

/*
	Get console status and pause if the user has hit control S.
	Abort if user has hit control C.
*/

#define CONTROL_C 3

void
syscsts()
{
	int c;

	c = syscstat();
	if (c == CONTROL_C) {
		printf("\nCompilation aborted by operator.\n");
		sysabort();
	}
}

/*
	Return the print string of the current date.
*/
char *
sysdate()
{
	char *p;

	time(&ltime);	
	newtime = localtime(&ltime);
	p = asctime(newtime);
	if (strlen(p) != 25) {
		return "";
	}
	else {
		strcpy(date_buf, "\"");
		p[11] = '\0';
		strcat(date_buf, p);
		p[24] = '\0';
		strcat(date_buf, p + 20);
		strcat(date_buf, "\"");

		TRACEPN("sysdate", printf("returns: %s\n", date_buf));
		return date_buf;
	}
}

/*
	Shut down the system.  This routine is called just before doing an
	exit().  Do any last minute things here.
*/
void
sysend()
{
	TICK("sysend");
}

/*
	Push c back into the file input stream, not the macro input stream.
*/
void
sys_fpb(c)
int c;
{
	file_put_back = c;
}

/*
	Close the current input file and pop back one level.
*/
void
sysiclose()
{
	register struct FN * fnp;

	TICK("sysiclose");

#ifdef DEBUG
	if (t_inlevel < 0) {
		syserr("sysiclose: Bad t_inlevel.");
	}
#endif

	/* Close the current input file. */
	fnp = in_list;	
	raw_close(fnp -> f_handle);

	/* Release the current node. */
	sys_release();
	if (t_inlevel > 0 && in_list == NULL) {
		syserr("sysiclose: NULL in_list.");
	}

	/* Pop back one level. */
	if (in_list != NULL) {
		if (t_inlevel < 0) {
			syserr("sysiclose: Unexpected in_list.");
		}
		fnp = in_list;
		s_ic = fnp -> f_bufc;
		s_ip = fnp -> f_bufp;
		t_file = fnp -> fil_name;
		t_line = fnp -> f_line;
	}
	t_inlevel--;

	if (t_inlevel == -1) {
		ch = END_FILE;
		TRACEPN("sysiclose", printf("restarts. ch=END_FILE\n"));
	}
	else {
		ch = (unsigned char) fnp -> f_lastc;
		TRACEPN("sysiclose", printf("retarts. ch=lastc: %s\n",
			pr_ch(ch)));
	}
		
	TRACEP("sysiclose", printf("exit: t_inlevel=%d\n", t_inlevel));
}

/*
	Allocate space for a new file node and file buffer.
	Fill in all other fields to neutral values.
*/
static struct FN *
sys_new(buf_size)
int buf_size;
{
	register struct FN * fnp;

	TRACEP("sys_new", printf("(buf_size: %d)\n", buf_size));

	fnp		 = (struct FN *) m_alloc(sizeof(struct FN));
	fnp -> f_buffer	 = (char *)      m_alloc(buf_size);
	fnp -> f_bufsize = buf_size;

	fnp -> f_next	= NULL;
	fnp -> fil_name	= NULL;
	fnp -> f_bufp	= fnp -> f_buffer;
	fnp -> f_bufc	= 0;
	fnp -> f_type	= CLOSED_STAT;

	TRACEPN("sys_new", printf("returns %lx\n", fnp));
	return fnp;
}

/*
	Set the global variable ch to the next character from the input stream.
	This is the most often called routine in the whole program.
*/
void
sysnext()
{
	for(;;) {
		sysn1();

		/* Intercept backslash newline combinations here. */
		if (ch == '\\') {

			/* 3/3/89 */
			sysn1();
			while (ch == '\r') {
				sysn1();
			}
			
			if (ch == '\n') {
				t_line++;
				continue;
			}
			else {
				push_back = ch;
				ch = '\\';
				return;
			}
		}
		/* Filter out all carriage returns. */
		else if (ch != '\r') {
			return;
		}
	}
}
		
static void
sysn1()
{
	register int n;
	register struct FN * fnp;

	/* Priority 1: A pushed back character. */
	if (push_back != -1) {
		ch = push_back;
		TRACEP("sysnext", printf("pushback: %s\n", pr_ch(ch)));
		push_back = -1;
		return;
	}

	/* Priority 2:  The rescan buffer. */
	if (m_flag) {
		ch = *p_rescan++;
		if (ch) {
			TRACEP("sysnext", printf("rescan ch: %s\n",
				pr_ch(ch)));
			return;
		}
		m_flag = FALSE;

		/*
			Priority 2A:  The trailing character.
			This is NOT part of the rescan buffer.
		*/
		if (file_put_back != -1) {
			ch = file_put_back;
			TRACEP("sysnext", printf("fpb ch: %s\n",
				pr_ch(ch)));
			file_put_back = -1;
			return;
		}
		/* FALL THROUGH. */
	}

	/* Priority 3:  The current file. */
	if (s_ic > 0) {
		s_ic--;
		ch = *s_ip++;
		TRACEP("sysnext_v", printf("ch: %s\n", pr_ch(ch)));
		return;
	}

	/* A file or local buffer is empty. */
	fnp = in_list;
	switch(fnp -> f_type) {

	case EOF_STAT:
		/* Continue returning END_FILE until the file is closed. */
		ch = END_FILE;
		return;

	case INPUT_STAT:
		n = raw_read(fnp->f_handle, fnp->f_buffer, IBUF_SIZE);
		if (n > 0) {
			s_ic  = n;
			s_ip = fnp -> f_buffer;
			s_ic--;
			ch = *s_ip++;
		}
		else {
			fnp -> f_type = EOF_STAT;
			ch = END_FILE;
		}
		return;

	default:
		TRACEP("sysn1",
			printf("in_list = %lx, in_list -> f_type = %d\n",
				in_list, in_list -> f_type));

		syserr("sysn1: Bad f_type field.");
	}
}

/*
	Put a newline to the output file.
*/
void
sysnlput()
{
	STAT("sysnlput");

	/* 8/1/89 ----- file is open for text, not binary.
	syscput('\r');
	----- */

	syscput('\n');

	/* Give user a chance to stop. */
	syscsts();
}

/*
	Close the output file(s).
*/
void
sysoclose()
{
	register struct FN * fnp;

	TICK("sysoclose");

	fnp = outn_list;
	if (fnp != NULL && fnp -> f_type != CLOSED_STAT) {
		syscput(END_FILE);
		raw_write(fnp -> f_handle, fnp -> f_buffer, s_oc);
		raw_close(fnp -> f_handle);
		fnp -> f_type = CLOSED_STAT;
	}
}

/*
	Open a file for input.
	Return TRUE if all went well.
*/
bool
sysopen(name)
char *name;
{
	struct FN * fnp;
	int handle;

	TRACEP("sysopen", printf("(%s); old t_inlevel = %d\n",
		name, t_inlevel));

	if (m_flag == TRUE) {
		m_flag = FALSE;
		error("Macro expansion truncated following #include.");
	}

	/* Save information about the current level on the stack. */
	if (t_inlevel != -1) {
		if (in_list == NULL) {
			syserr("sysopen: Can't happen.");
		}
		fnp = in_list;
		fnp -> f_line = t_line;
		fnp -> f_bufc = s_ic;
		fnp -> f_bufp = s_ip;
		fnp -> f_lastc = (int) ch;
	}

	/* Enter a new level. */
	if (t_inlevel >= MAX_INLEVEL) {
		fatal("include files nested too deeply");
	}
	else {
		t_inlevel++;
	}

	/* Create a new file node and link to the front of the list. */

	TRACEP("sysopen", printf("set up new file: call sys_new\n"));

	fnp	      = sys_new(IBUF_SIZE);
	fnp -> f_next = in_list;
	fnp -> f_type = INPUT_STAT;
	in_list       = fnp;

	/* Actually open the file. */

	handle = raw_open(name);
	if (handle == ERROR) {

		/* Deallocate the node. */
		TRACEP("sysopen", printf("file open %s fails\n", name));
		sys_release();
		t_inlevel--;
		return FALSE;
	}
	else {
		fnp -> f_handle = handle;
	}

	/* Set the global variables. */
	t_file = str_alloc(name);
	t_line = 1;
	fnp -> fil_name = t_file;

	/* Put the first character of the file into ch. */
	s_ic = 0;
	s_ip = fnp -> f_buffer;
	sysnext();

	TRACEP("sysopen", printf("exit: t_inlevel=%d, in_list=%lx\n",
						t_inlevel, (long)in_list));
	return TRUE;
}

/*
	Push back c so that sysnext() will return it next.
*/
void
syspushback(c)
int c;
{
	push_back = c;
}

/*
	Remove one node from the input list.
*/
static void
sys_release()
{
	register struct FN * fnp;

	TICK("sys_release");

#ifdef DEBUG
	if (in_list == NULL) {
		syserr("sys_release: Can't happen.");
	}
#endif

	/* Remove the node from the input list. */
	fnp     = in_list;
	in_list = fnp -> f_next;
	fnp -> f_type = CLOSED_STAT;

	/* Deallocate the node. */
	if (fnp -> f_buffer != NULL) {
		m_free(fnp -> f_buffer);
	}
	m_free(fnp);

	/* Reset the global variables from the next node. */
	if (in_list == NULL) {
		s_ic = 0;
	}
	else {
		fnp = in_list;
		s_ic = fnp -> f_bufc;
		s_ip = fnp -> f_bufp;
	}
}

/*
	Put one string to the output file.
*/
void
syssput(s)
register char * s;
{
	TRACEP("syssput", printf("(%s)\n", s));

	while (*s) {
		syscput(*s++);
	}
}

/*
	Return a print string of the current time.
*/
char *
systime()
{
	char *p;

	time(&ltime);	
	newtime = localtime(&ltime);
	p = asctime(newtime);
	if (strlen(p) != 25) {
		return "\"\"";
	}
	else {
		strcpy(time_buf, "\"");
		*(p+19) = '\0';
		strcat(time_buf, p+11);
		strcat(time_buf, "\"");
		TRACEPN("systime", printf("returns: %s\n", time_buf));
		return time_buf;
	}
}
