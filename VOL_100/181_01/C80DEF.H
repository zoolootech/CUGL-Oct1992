/* c80def.h-standard definitions for C/80 from Software Toolworks */

#define TRUE 1
#define FALSE 0

#define ALLOC_ERR -1
#define BLANK ' '
#define EOS '\0'
#define ESCAPE '\\'
#define NEWLINE '\n'
#define NIL 0
#define NO 0
#define NULL '\0'
#define TAB '\t'
#define CR 0x0d
#define LF 0x0a

/* file-related definitions */
#define READ "r"
#define READ_BINARY "rb"
#define UPDATE "u"
#define UPDATE_BINARY "ub"
#define WRITE "w"
#define WRITE_BINARY "wb"
#define EOF -1
#define CPMEOF 0x1a

/* these are to make c80 compatible with newer versions of c */
#define FILE int
extern int fin,fout;
#define STDERR 0
#define stderr 0
#define STDIN fin
#define stdin fin
#define STDOUT fout
#define stdout fout

#define void int
#define	boolean int
