/*	file	demo3.c	*/
/*	Hex dump of float variables	*/
#include	fprintf.h
extern	float	atof();
extern	char	*ftoa();
int	n;
long	ln,*pl;
float	fn,*pf = &fn;
extern	int	portc,portd;

main() {
	portd = 188; portc = 189;	/* must be YOUR hardware ports */
#asm
	.Z80
	LD	HL,(pf)
	LD	(pl),HL
	.8080
#endasm
	for(n = -130; n < 130; ++n) {
		fn = n;
		printf("\n%.1f\t=",fn);
		printf("\t0%lxH",*pl);
	}
	printf("\n\nEND\n\n");
}
/*	This program was originally created as a tool for the development
	of several pieces of Mchip80.   With small changes, the program
	can be used to disassemble any float value, revealing AM9511
	format expressed as a string of hex characters.   It is useful
	when output from this program is redirected to a disk file.	*/
