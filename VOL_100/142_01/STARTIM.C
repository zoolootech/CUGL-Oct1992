#include bdscio.h
/* Modified 8 April 1984 by Alan Coates from Starpri, written by Julian Hyde
 */
char ibuf[BUFSIZ],obuf[BUFSIZ],date1[10],date2[10];
int ok;
main(argc,argv)
int argc;
char **argv;
{
	char *d1,*d2;
	int fct;
	int surv;
	if(argc!=3)
	{
	printf("\nStartim was modified by Alan Coates\n");
	printf("from Starpri,written by Julian Hyde in 1983,\n");
	printf("augmented to calculate interval durations.\n");
	printf("It accepts a file in datastar format\n");
	printf("from which are selected by field number two fields\n");
	printf("which are copied to the output file in the order input\n");
	printf("plus two dates in ddmmyy format.\n");
	printf("  The 2 dates are processed to give \n");
	printf("the interval from the first to the second\n");
	printf("and this is output in days as the fifth field.\n");
	printf("The actual dates are also output to assist debugging data.\n");
	printf("\nEach output field will be padded with leading or\n");
	printf("trailing blanks to a nominated fixed length.\n");
	printf("Cases in which any requested variable is missing \n");
	printf("will be excluded, and the number of such cases reported.\n");
        printf("The program will handle records up to 250 chars.\n\n");
	printf("Type 'startim inputfile outputfile'.\n");
	return;
	}	
   	
	/*OPEN INPUT FILE*/
	if(fopen(*(argv+1),ibuf)==ERROR)
	{	printf("File %s is not available!",*argv);
		return;
	}
	/*CREATE OUTPUT FILE*/
	if(fcreat(*(argv+2),obuf)==ERROR)
	{	printf("Cannot create %s!",*(argv+1));
		return;
	}
	/*RECORD AREAS*/
	char iline[MAXLINE],*i;
	char oline[MAXLINE],*o;
	char store[MAXLINE],*s;
	char diff[MAXLINE],*d3;
	int qno,cno;
	unsigned recs,badrecs;
        badrecs=recs=0;
	/*READ SPECIFICATION LINE*/
	printf("\nEnter the field numbers wanted, each followed by the \n");
	printf("length to which it is to be padded out or truncated\n");
	printf("in the output record, in sequence separated\n");
	printf("by non-numerics.  Then type return.\n");
	printf("\nIf output fields are to be right justified, use a\n");
	printf("specifier of the form lRp where l is the length to which\n");
	printf("the field will be right truncated then right justified,\n");
	printf("and p specifies a fixed number of trailing blanks.\n\n");
	gets(store);
	/*PROCESS REST OF INPUT FILE*/
	while(fgets(iline,ibuf))
	{	/*IGNORE LEADING NON NUMERICS IN SPEC*/
		s=store;
		fct = 0;	/* reset counter for new line	*/
		ok = TRUE;	/* reset test for new case	*/
		while(!isdigit(*s)&&*s)s++;
		for(i=iline,o=oline;*s;)
		{	/*GET FIELD NO */
			int fieldno;fieldno=0;
			while(isdigit(*s))
				fieldno=fieldno*10+*(s++)-48;
			/*IGNORE NON NUMERIC IN SPEC*/
			while(!isdigit(*s)&&*s)s++;
			/*GET LENGTH NO*/
			int ll; ll=0;
			while (isdigit(*s))
				ll=ll*10+*(s++)-48;
			if(fieldno)
			{	fieldno--;
				/*FIND FIELD*/
			 
				qno=cno=0;
				for(i=iline;*i&&cno<fieldno;i++)
					if(*i=='"')qno++;
					else if(*i==','&&!(qno%2))cno++;
				if(!*i)
				{	printf("\nNo field %d exists!",
					fieldno+1); return;
				}
				/*COPY FIELD*/
     				int l;l=0;
				++fct;
				char *oo;oo=o;
				while(*i&&*i!='\n'&&(*i!=','||qno%2))
				{       char c;                 
					c=*(i++);
					if(c=='"')cno++;
					if(((c!='"')||(cno%2&&cno>1))&&l<ll)
					{	*(o++)=c;
						l++;
					}
				}
				/* TEST FOR BLANK FIELD MAY BE ADDED LATER
				if (c = ' ')
					remark("blank in field",fieldno+1);*/
				/* TEST THIRD AND FOURTH FIELDS FOR length=6
				 */
				if(fct == 3 || fct == 4)
				{	
					
					if(l != 6)
		remark("Wrong length date field - case no",recs+1); 

				/* COPY LAST 6 CHARACTERS TO DATE */
				char *ooo; 
				if (fct == 3)
				 {
				 for(d1=date1,ooo=o-6; ooo < o; ooo++)
				   *(d1++) = *ooo;
				 *(d1++) = '\0';
				 }
				if (fct == 4)
				 { 
				 for(d2=date2,ooo=o-6; ooo < o; ooo++)
				   *(d2++) = *ooo;
				 *(d2++) = '\0';
 				  surv = days(date2) - days(date1);
				if (ok == FALSE || surv <= 0)
					{
					badrecs += 1;
					d3 = diff;
					*(d3++) = '1';
					*(d3++) = '-';
					for(d3=diff+2;d3<diff+8;++d3)
						*d3 = ' ';
					*(d3++) = '\0';
		printf("Unable to calculate survival for case %d\n\n",recs+1); 
					}
				else
/*	make string diff backwards from surv	*/
				  for (d3=diff;d3 < diff + 8;++d3)
					{
					*d3 = (surv%10) + 48;
					surv = surv/10;
					}
				  *d3='\0';
				 }
				}
				if(*s=='R'||*s=='r')
				{	/*RIGHT JUSTIFY*/
					char *ora,*orb;
					while(l<ll)
					{
			for(orb=o++,ora=orb-1;orb>oo;ora--,orb--)
						*orb=*ora;
					*orb=' ';
					l++;
					}
					/*GET PADDING SIZE*/
					ll=l=0;
					s++;
					while (isdigit(*s))
						ll=ll*10+*(s++)-48;
				}
				/*PAD FIELD WITH BLANKS*/
				while (l<ll)
				{	*(o++)=' ';
					l++;
				}
			 	
			}
			/*IGNORE NON NUMERIC IN SPEC*/
       			while(!isdigit(*s)&&*s)s++;
			/*TERMINATE FIELD OR LINE*/
			if(*s);
			else
			{	/* WRITE diff to oline, reversed */
				for(d3=diff+5; d3>=diff; --d3)
					*(o++) = *d3;

				*(o++)='\n';
				*o=0;
			}
		}
		/*WRITE OUTPUT FILE*/
			if(fputs(oline,obuf)==ERROR)
			 {	printf("Write error!\n");
			return;
			 }
		if(!(++recs%100))
		 printf("\nReformatted %d records - %d bad\n",recs,badrecs);
	}
	printf("\nFinal cases processed : %d\n",recs);
	printf("Cases set to -1 : %d\n",badrecs);
	/*CLOSE INPUT FILE*/
	fclose(ibuf);
	/*CLOSE OUTPUT FILE*/
	putc(CPMEOF,obuf);
	fflush(obuf);
}
/*
function days(date)
ASSUMES 6 DIGIT ASCII ARGUMENT DDMMYY, RETURNS DAYS FROM 1.1.1900
If date > about mid 1989, this will exceed MAXINT and appear negative.
For the purpose of calculating a difference between two dates, the
overflow doesn't matter so long as difference is not > MAXINT.
 */
int days(date)
char date[];
{
int dd,mm,yy;
unsigned daytot;
char *s;
	dd = mm = yy = 0;
	for(s = date;s < date+2 && isdigit(*s); )
		dd = dd*10 + *(s++) -48;
	for(s = date + 2;s < date+4 && isdigit(*s); )
		mm = mm*10 + *(s++) -48;
	for(s = date + 4 ;s < date+ 6 && isdigit(*s); )
		yy = yy*10 + *(s++) -48;
	if(dd > 31)
		{
		remark("Bad day",dd);
		}
	daytot = dd;
	/*	add months, correct for time of year	*/
	daytot += 30*(mm - 1);
	switch (mm)
			{
		case 1:
		case 4:
		case 5:
			daytot += 0; /*	no correction	*/
			break;
		case 3:
			daytot -= 1;
			break;
		case 2:
		case 6:
		case 7:
			daytot += 1;
			break;
		case 8:
			daytot += 2;
			break;
		case 9:
		case 10:
			daytot += 3;
			break;
		case 11:
		case 12:
			daytot += 4;
			break;
		default:
			remark("Bad month",mm);
			break;
			}
	/*	add for completed years	*/
		daytot += 365*yy;
	/*	correct within a leap year iff after Feb 29	*/
		if(mm > 2 && yy%4 == 0 && yy > 0)
			daytot += 1;
	/*	correct for elapsed leap years	*/
		daytot += (yy-1)/4;
	return(daytot);
}
remark(s1,x)
	char s1[] ;
	int x;
	{
	printf("%s   %d\n",s1,x);
	ok = FALSE;
	return;
	}
	