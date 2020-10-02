
 /* simple program to test sine, cosine and tangent functions */

main()
{
char *sine(),*cosine(),*degtorad(),*radtodeg();
char s[80],angd[5],angr[5],angd2[5],result[5];
char result2[5],*itof(),*gets();
char *atof(), *fpadd(), initang[5], incrang[5], endang[5];
char result3[5],result4[5],*arctan();
int scanf(), fpcomp(), index;

	while (1)
	{printf("\nEnter initial angle in degrees ");
	 gets(s);
	 atof(initang,s);
	 printf("\nEnter increment angle in degrees ");
	 gets(s);atof(incrang,s);
	 printf("\nEnter end angle in degrees ");
	 gets(s);atof(endang,s);
	  for(index=0;index<=4;index++) angd[index] = initang[index] ;
	 while(fpcomp(angd,endang) <= 0)
	   {
	   degtorad(angr,angd);
	   radtodeg(angd2,angr);
	   printf("\n Input %e deg, %e rad, %e reconv deg ",angd,angr,
		angd2);
	   sine(result,angr);
	   printf("\n sine = %e",result);
	   cosine(result2,angr);
	   printf(" cosine = %E",result2);
	   tangent(result3,angr);
	   printf(" tangent = %e",result3);
	   arctan(result4,result3);
	   printf("\n arctangent = %e angle was %e ",result4,angr);
	   fpadd(angd,angd,incrang);
	  }
	}
}
