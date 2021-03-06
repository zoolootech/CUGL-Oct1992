

/* NEWFLVAL.C a program to evaluate/test second modification
of the floating point package for BDS-C.  Entry from keyboard,
and diagnostic printout, etc.
*/
#include "bdscio.h"
main()
{
	char *atof(), op1[5], s[80];
	char *fpmag(),*fpchs(),*fpasg();
	char result1[5],result2[5],result3[5];
	char *gets();
	int atoi();
	int ftoit(), ftoir();
	int nin;
	printf("\n");
	while (TRUE) {
	 printf("\nEnter integer ");
	 gets(s);
	 nin = atoi(s);
	 itof(op1,nin);
	 copyout(op1);
	 printf("\nEnter float ");
	 gets(s);
	 atof(op1,s);
	 copyout(op1);
	printf("\n Truncate %7d %4x Round %07d %06x ",ftoit(op1),
		ftoit(op1),ftoir(op1),ftoir(op1));
	fpasg(result1,op1); fpmag(result2,op1); fpchs(result3,op1);
	 printf("\n Assign %e  Mag %e  Chg Sign %e ",result1,
		result2,result3);
	printf("\n f formats %018.7f %18.8f %-18f",op1,op1,op1);
			}
}

copyout(oper)
char oper[5];
{	int i;
	printf("\nFLOAT %e hex ",oper);
	for(i=0;i<=4;i++) printf("%X,",oper[i]);
	printf(" dec ");
	for(i=0;i<=4;i++) printf("%D,",oper[i]);
	printf("\n octal ");
	for(i=0;i<=4;i++) printf("\\%o",oper[i]);
	return;
}

  �!  9�DM͐�͐�� ���s#rz��+ !����͐ �͐ ��s#r͐##6 #6͐~#fo����   $ PUTCHA�BDO�WRIT� =� �  �  �  �!  9�DM͐+|��, ! 	n& �� ���͐++|��J ! 	n& �! �� ����͐+++|��i ! 	n& �! �� ����͐++++|�¡ ! 	n}�
 ! �! �� ��! 	n& �! �� ����͐##^#Vr+sz��� ͐ ^#Vr+s�! 	n��s�& ��! �͐ �͐~#fo��	 ��� ���� !����͐##6�#6͐ ��
		The Incredible Superpowerful 
	    Floating Point Package for BDS C v1.4
	    *************************************
	    software written by: Bob Mathias
	    this documentation by: Leor Zolman

	Components of the floating point package:

	    1)	FLOAT.DOC:	This documentation file
	    2)	FLOAT.C:	File of support functions, written in C
	    3)	FP:		The workhorse function (in DEFF2.CRL)
	    4)  FLOATSUM.C	A Sample use of all this stuff

 EXTENDED VERSION ADDING FIVE USEFUL FUNCTIONS, USING THE V 1.44
  STATIC CAPABILITY, AND ADDING THE ZERO PAD FEATURE TO _spr
  (This is a further improvement of previous FLOATXT)