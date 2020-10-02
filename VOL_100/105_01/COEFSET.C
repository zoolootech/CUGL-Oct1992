/**   ******** coefset.c ********
obtain printout of and test of floating point constants
so that they can be initialized with initb 
	C. Calhoun
	257 South Broadway
	Lebanon, Ohio 45036   513 932 4541 / 433 7510
2 May 1981   */
	main ()
{
   char *atof(),op1[5],s[80],op2[5];
   char *gets();
while (1) {
   printf ("\nEnter Coef. ");
	gets(s);
   atof(op1,s);
   printf ("\n Four decimal equivalents %D %D %D %D %D ",
	op1[0],op1[1],op1[2],op1[3],op1[4]);
   sprintf(s,"%d,%d,%d,%d,%d \200",op1[0],op1[1],op1[2],op1[3],op1[4]);
	printf("\n String Value %s",s);
   initb(op2,s);
	printf("\n NEW VALUE %-25.17e",op2);
   	}
}
