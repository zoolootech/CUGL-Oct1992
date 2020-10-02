           
/********************************************************************


			     LTRI.C
                               By
			  David McCourt

*********************************************************************/

#include bdscio.h
#define CLEARS "\032"			/* clear screen 		*/
#define MON 480				/* max months in file           */

int date[MON];				/* date of entry:
						182 = January 1982
					       1282 = December 1982	*/

char djia [MON] [5];			/* closing Dow Jones Industrial
					   average for the last day the
					   market was open in the given
					   month			*/
	
char ind [MON] [5];			/* Long Term risk index		*/
char fac[5];				/* Used to multiply index 	*/
char round[5];				/* Used to round index		*/
char next[5];				/* Used to add to weight factor	*/
int rec_num,w_o;

main()
{
	atof(round,"0.5");
	atof(fac, "10");
	atof(next, "1");
	rec_num = 0;
	w_o = 0;
	menu();
}

menu()
{
int x;
	puts(CLEARS);
	puts("\n\tMenu for Long Term Risk Index\n");
	puts("\t==================================================\n\n");
	puts("\t\t0) End session\n");
	puts("\t\t1) Update file\n");
	puts("\t\t2) Load file\n");
	puts("\t\t3) Save file\n");
	puts("\t\t4) Edit \n");
	puts("\t\t5) Print file\n");
	puts("\n\n\n\tEnter your selection:");
	scanf("%d",&x);
	switch(x){
		case 0:
			exit();
		case 1:
			up_date();
			break;
		case 2:
			load();
			break;
		case 3:
			save();
			break;
		case 4:
			edit();
			break;
		case 5:
			list();
			break;
		default:
			menu();
	}
	menu();
}


/*
	After 35 entries have been made, the program will branch
	to this function to develope the Index
*/
 
index(a)
int a;
{
char bld_1 [5];
char bld_2 [5];
char bld_3 [5];
char count [5];

	atof(count, "0");
	atof(ind[a],"0");

	w_o = a - 10;
	while(w_o < a + 1){
		/* add one to count for weighted average */
		fpadd(count,count,next);
		
		/* subtract 14 month old djia from current djia */
		fpsub(bld_1,djia [w_o], djia [w_o - 14]);

		/* divide by 14 month old djia */
		fpdiv(bld_1,bld_1,djia [w_o - 14]);

		/* subtract 11 month old djia from current djia */
		fpsub(bld_2,djia [w_o],djia [w_o -11]);

		/* divide by 11 month old djia */
		fpdiv(bld_2,bld_2,djia [w_o -11]);

		/* add together */
		fpadd(bld_3,bld_1,bld_2);

		/* weight the average */
		fpmult(bld_3,bld_3,count);

		/* add weighted average to the index */
		fpadd(ind [a],ind [a],bld_3);

		++w_o;
	}

	/* multiply index by 10 */	
	fpmult(ind [a],ind[a],fac);
	fpadd(ind[a],ind[a],round);
}

hold()
{
	puts("\nPress <RETURN> to continue ...");
	getchar();
}
	
													
up_date()
{
char str[5];
int a;

	++rec_num; 
	puts(CLEARS);
	if(rec_num > 1)
		printf("\t\tLast date entered: %d",date[rec_num -1]);
	printf("\n\n\t\tWorking on record number: %3d",rec_num);
	puts("\n\n\t\tEnter date: ");
	scanf("%d",&date[rec_num]);
	puts("\n\t\tEnter closing DJIA: ");
	atof(djia[rec_num], gets(str));
	if(rec_num > 35)
		a = rec_num;
		index(a);

}

load()
{
char iobuf[BUFSIZ];
int a,b;
	puts(CLEARS);
	puts("Loading file ...");
	if(fopen("LTRI.FIL",iobuf) ==ERROR){
		puts("\nCan't open LTRI.FIL");
		return(ERROR);
	}
	rec_num = getw(iobuf);
	for(a=0; a < rec_num +1; ++a){
		date[a] = getw(iobuf);
		for(b=0; b < 5; ++b){
			djia[a] [b] = getw(iobuf);
			ind [a] [b] = getw(iobuf);
		}
	}
	fclose(iobuf);
}

save()
{
char iobuf[BUFSIZ];
int a,b;
	
	puts(CLEARS);
	puts("Saving file to disk ...");
	if(rec_num == 0){
		puts("\nCAUTION FILE IS EMPTY");
		hold();
	}
	if(fcreat("LTRI.FIL",iobuf) == ERROR){
		puts("\nCan't create LTRI.FIL");
		return(ERROR);
	}
	putw(rec_num,iobuf);
	for(a=0; a < rec_num +1; ++a){
		putw(date[a],iobuf);
		for(b=0; b < 5; ++b){
			putw(djia[a] [b],iobuf);
			putw(ind[a] [b], iobuf);
		}
	}
	fflush(iobuf);
	fclose(iobuf); 
}

edit()
{
int a;
char str[5];

	puts(CLEARS);
	puts("\n\t\tEnter record number to change: ");
	scanf("%d", &a);
	puts("\n\t\tEnter date: ");
	scanf("%d",date[a]);
	puts("\n\t\tEnter closing DJIA: ");
	atof(djia[a],gets(str));
	if(a > 35){
		puts("\n\n\t\tEditing record number:\n");
		while(a < rec_num +1){
			printf("%4d",a);
			index(a);
			++ a;
		}
	}
}

list()
{
int a,b;
			
	puts(CLEARS);
	puts("\n\n\n\n\n\n\n\n");
	puts("\t\tThe screen will display five years of history.\n");
	puts("\t\tTo see the most current data,  enter a number\n");
	puts("\t\t1 less than the number of records in the file.\n\n"); 
	printf("\t\tThere are  %d  records in the file.\n",rec_num);
	printf("\t\tStart from record number: [   ]\b\b\b\b");
	scanf("%d", &a);
	if(a + 60 > rec_num){
		b = rec_num;
 		a = rec_num - 59;
		}
	else 
		b = a + 60;
	puts(CLEARS);
	puts(" Date   DJIA    Index       Date   DJIA    Index");  	
	puts("       Date   DJIA    Index\n\n");
	while(a < b){
		printf("%5d  %7.2f   %5.0f     ",date[a],djia[a],ind[a]);
		printf("%5d  %7.2f   %5.0f     ",date[a+1],djia[a+1],ind[a+1]);
		printf("%5d  %7.2f   %5.0f\n",date[a+2],djia[a+2],ind[a+2]);
		a = a + 3;
	}
	hold();
}

/* eof ltri.c							*/
