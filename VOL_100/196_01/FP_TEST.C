/* [FP-TEST.C of JUGPDS Vol.19]
*****************************************************************
*								*
*	Written by  Hakuo Katayose (JUG-CP/M No.179)		*
*		    49-114 Kawauchi-Sanjuunin-machi		*
*	            Sendai, Miyagi 980                          *
*		    Phone: 0222-61-3219				*
*								*
*	Edited & tested by Y. Monma (JUG-C/M Disk Editor)       * 
*								*
*****************************************************************
*/

/* Test Program for fp64(Func_No,a,b,c); */

#include <bdscio.h>
#include <dio.h>

#define	FPGETK	0  /* get constant internally defined in fp64 */
#define	FPADD	1	/* addition:       c = a + b	*/
#define	FPSUB	2	/* subtraction:    c = a - b	*/
#define	FPMUL	3	/* multiplication: c = a * b 	*/ 
#define	FPDIV	4	/* division:       c = a / b	*/
#define	FPCMP	5	/* comparison:     c = 1 if a > b,
				           c = 0 if a = b,
				           c = -1 if a < b	*/
#define	FPNEG	6	/* negation:	   c = - a	*/
#define	FPSFT	7	/* shift:	   c = a * 2**2 */ 
#define	FPHLF	8	/* half: 	   c = a / 2 	*/
#define	FPDBL	9	/* double:	   c = b * 2	*/

#define	FPCNV	10	/* format conversion for output */
#define	FPIN	11	/* format conversion for input  */

#define	FPSQRT	12	/* square root:   c = sqrt(a)	*/
#define	SINCOS	13	/* b = cos(a), c = ain(a), a is in radian */
#define	ATAN2	14	/* c = arctan(a/b)	*/
#define	EXP	15	/* c = 2**b	*/	
#define	LOG	16	/* c = log[2]b  */

#define	LLADD	21
#define	LLSUB	22
#define	LLMUL	23
#define	LLDIV	24

#define	LLCMP	25
#define	LLNEG	26
#define	LLMOV	27
#define	LLSFTL	28
#define	LLSFTR	29
#define	ATOLL	30
#define	LLTOA	31
#define	LLTEN	32


#define	FPTST1	255
#define	FPTST2	254

main(argc, argv)
char **argv;
{
	char	a[8], b[8], c[8], t[8], z[8];
	char	s[40], one[8];
	int	i;

/*
	dioinit(&argc, argv);
*/
	puts("** Test of fp64(Func_No,a,b,c); **\n");
	putchar('\n');
	test_exp();
	test_log();
	test_sincos();
	test_atan2();
	test_sqrt();
/*	test_			; additinal test module 
	dioflush();
*/
}


test_exp()
{
	char	a[8], b[8];
	char	s[40];
	int	i;

	for (;;) {
		puts("Input data A for fp64(EXP,A,0,C);\n");
		puts("Simple <CR> proceeds next function.\n");
		gets(s);
		if (strlen(s) <= 0)
			break;
		fp64(FPIN, s, 0, a);
		fp64(EXP, a, 0, b);
		elist(a);	elist(b);	putchar('\n');
	}
}


test_log()
{
	char	a[8], b[8];
	char	s[40];
	int	i;

	for (;;) {
		puts("Input data A for fp64(LOG,A,0,C): ");
		gets(s);
		if (strlen(s) <= 0)
			break;
		fp64(FPIN, s, 0, a);
		fp64(LOG, a, 0, b);
		elist(a);	elist(b);	putchar('\n');
	}
}


test_sincos()
{
	char	a[8],b[8],c[8],z[8];
	int	i;

	puts("*******  sin  cos  *****\n");
	fp64(FPIN, "2", 0, a);
	fp64(FPIN, "2", 0, z);
	for (i = 0; i <= 18; i++) {
		fp64(SINCOS, a, b, c);
		elist(a);	elist(b);	elist(c);
		putchar('\n');
		fp64(FPDIV, a, z, a);
	}
}


test_atan2()
{
	char	a[8], b[8], c[8], z[8];
	int	i;

	puts("*****  atan  *****\n");
	fp64(FPIN, "2", 0, b);
	fp64(FPIN, "0.1", 0, a);
	fp64(FPIN, "0.1", 0, z);
	for (i = 0; i < 10; i++) {
		fp64(ATAN2, a, b, c);
		elist(c);	elist(a);	elist(b);	putchar('\n');
		fp64(FPADD, a, z, a);
		}
	elist(c);
	putchar('\n');	putchar('\n');
}

test_sqrt()
{
	char	a[8], b[8];
	char	s[40];

	for (; ; ) {
		puts("Input data A for fp64(FPSQRT,A,0,C): ");
		gets(s);
		if (strlen(s) <= 0)
			break;
		fp64(FPIN, s, 0, a);
		elist(a);	putchar('\n');
		fp64(FPSQRT, a, 0, b);	elist(b);	putchar('\n');
	}

}


flist(s)
char *s;
{
	char	*p;
	int	ep, i;

	for(i = 0; i < 8; i++) printf("%02x|",*s++);
	puts("   ");
}


elist(s)
char *s;
{
	char	*p;
	int	ep, i;

	p = fp64(FPCNV, s);
	ep = *p++ + *p++ * 256;
	printf("%15sE%4d   ", p, ep);
}
