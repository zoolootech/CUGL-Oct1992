/**   ******** coefstat.c ********
obtain printout of and test of floating point constants
so that they can be initialized as static variables 
	C. Calhoun
	257 South Broadway
	Lebanon, Ohio 45036   513 932 4541 / 433 7510
6 July 1981    */
	main ()
{
   char *atof(),op1[5],s[80],op2[5];
   char title[80], *s2;
   char *gets();
while (1) {
   printf ("\nEnter Coef. ");
	gets(s);
   printf ("\nEnter Title");
	gets(title);
   atof(op1,s);
   printf ("\n Four octal equivalents \\%O\\%O\\%O\\%O\\%O for %s",
	op1[0],op1[1],op1[2],op1[3],op1[4],title);
   	}
}
                  REVISION 1 13 JULY 1981


      The CTRIG programs have been revised to take advantage of the ability to

insert '\0' into string constants which make it possible to use string constant

as pseudo-static floating point constants.  In addition, the values of the

constants have been revised to improve performance.  An added arctangent

function arctan2()
 /* simple program to test sine, cosine and tangent functions */

main()
{
char *sine(),*cosine(),*degtorad(),*radtodeg();
char s[80],angd[5],angr[5],angd2[5],result[5];
char result2[5],*itof(),*gets();
char *atof(), *fpadd(), initang[5], incrang[5], endang[5];
char result3[5],result4[5],*arctan();
char *arctan2(), result5[5], angatd1[5], angatd2[5];
int scanf(), fpcomp(), index;
int *quadrant;

	while (1)
	{printf("\nEnter initial angle in degrees ");
	 gets(s);
	 atof(initang,s);
	 printf("\nEnter increment angle in degrees ");
	 gets(s);atof(incrang,s);
	 printf("\nEnter end angle in degrees ");
	 gets(s);at