/*
**	header for 8048 disassembler
**	Copyright (c) 1990, Michael G. Panas
*/

/* #define	MSC	/* Microsoft C complier flag */

/*
**	table entry for each opcode
*/
struct table {
	char	type;		/* opcode type */
	char	len;		/* opcode length */
	char	width;		/* second tab zone field width */
	char	fill;
	char	*text;		/* text to insert */
};

/*
**	type field defines
*/
#define	NOP	0		/* no operation on text */
#define	IMM	1		/* immediate data on this opcode */
#define	CAL	2		/* call instuction */
#define	BRA	3		/* short (8 bit ) branch */
#define	JMP	4		/* long jump */
#define	RET	5		/* unconditional return */
#define	DBC	6		/* byte constant */

/*
**	symbol table entry structure
*/
struct symbol {
	char	flags;		/* symbol table flags */
	char	fill[3];
	int	address;	/* address of this symbol */
};

/*
**	defines for flag field
*/
#define	USED	1		/* entry used flag */
#define	PRINT	2		/* label printed */

#define	SYMS	600		/* number of symbols (max) in table */
