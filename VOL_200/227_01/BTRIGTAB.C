/*
 *	b t r i g t a b . c
 *	-------------------
 *	This program prints all sine and cosine values within the range
 *	0 to 360 degrees to stdout. The values are seperated by spaces,
 *	whereby all sine values are printed first.
 */
#include <stdio.h>
#include <math.h>


#define	PI_D_180	0.017453293

void	usage(void)
{
fprintf(stderr, "Usage: btrigtab tab\n");
fprintf(stderr, "       tab = sin or cos\n");
exit(1);
/*NOTREACHED*/
}


void	main(argc, argv)
int	argc;
char	*argv[];
{
float		i;
register int	csin = 0;

if(argc != 2)
	usage();
if(!strcmp(argv[1], "sin"))
	csin = 1;
else if(strcmp(argv[1], "cos"))
	usage();
for(i = 0 ; i <= 360 ; ++i)
	printf("\t\t\t%f,\n", csin ? sin(i * PI_D_180) : cos(i * PI_D_180));
}
