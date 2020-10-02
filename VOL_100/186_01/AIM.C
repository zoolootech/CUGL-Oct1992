                      
/***********************************************************

		Automatic Investment Management
			     by
		       David McCourt
************************************************************/

#include bdscio.h
#define CLEARS "\032"		/* clear screen code */
#define MAX_ISS 15 		/* number of stocks or mutual funds */
#define MAX_LET 11		/* number of letters in name        */
char name[MAX_ISS] [MAX_LET];	/* name of stock or fund */
char shares[MAX_ISS] [5];	/* no of shares owned */
char price[MAX_ISS] [5];	/* current price */
char cost[MAX_ISS] [5];		/* total invested in stock/fund
				   when shares are sold as directed
				   by AIM, the total cost is reduced
				   by the amount of the sale
				*/
char value[MAX_ISS] [5];	/* current total value */
char control[MAX_ISS] [5];	/* used for buy/sell decisions */
char aim_rec[MAX_ISS] [5];	/* the buy sell amount */
char bs[MAX_ISS] [5];		/* 'buy' or 'sell' instruction */
char str[10];			/* used for input to floating point */
char m1[5];			/* used to compute control */
char m2[5];
char m4[5];
char tot_inv[5];		/* used to compute various totals */
int x, iss_num, w_o;		/* iss_num = count of issues in file
				   w_o = issue currently being worked on
				   x = transaction code
				*/

main()
{
	iss_num = -1;
	atof(m1, "1.1");
	atof(m2, ".5");
	atof(m4, ".1");
	menu();

}

menu()
{

	x = 0;
	puts(CLEARS);			
	puts("\n\t\tMenu for Automatic Investment Management\n");
	puts("\t\t========================================\n\n");
	puts("\t\t\t 0) End session\n");
	puts("\t\t\t 1) Original investment\n");
	puts("\t\t\t 2) Dividends received as stock\n");
	puts("\t\t\t 3) Additional cash invested\n");
	puts("\t\t\t 4) AIM Market order - SELL \n");
	puts("\t\t\t 5) AIM Market order - BUY\n");
	puts("\t\t\t 6) Review portfolio\n");
	puts("\t\t\t 7) Save file\n");
	puts("\t\t\t 8) Read file\n");
	puts("\t\t\t 9) Delete issue\n");
	puts("\t\t\t10) Update Prices\n");
	puts("\t\t\t11) Money Market original investment\n");
	puts("\t\t\t12) Money Market deposits, withdrawal\n");
        puts("\t\t\t    and interest received\n");
	printf("\n\n\t\tEnter your choice: ");
	
	scanf("%d",&x);

	switch(x){
		case 0:
			exit();
			break;
		case 1:
			org_pur();
			shs_pur();
			break;
		case 2:
			list();
			sel_iss();
			shs_pur();
			break;
		case 3:
			new_add();
			break;
		case 4:
			list();
			sel_iss();
			shs_pur();
			break;
		case 5:
			list();
			sel_iss();
			shs_pur();
			break;
		case 6:
			ind_rep();
			break;
		case 7:
			save();
			break;
		case 8:
			load();
			break;
		case 9:
			delete();
			break;
		case 10:
			list();
			sel_iss();
			up_date();
			break;
		case 11:
			org_pur();
			dep_mon();
			break;
		case 12:
			list();
			sel_iss();
			dep_mon();
			break;
		default:
			menu();
	}
	menu();
}

dep_mon()
{
char add_cash[5];

	puts(CLEARS);
	printf("\n\t\tWorking on: %s",name[w_o]);
	puts("\n\n\t\tEnter the amount of the deposit:  ");
	atof(add_cash, gets(str));
	atof(price[w_o], "1");	/* set price per share to $1 */
	fpadd(cost[w_o],cost[w_o],add_cash);
	fpadd(control[w_o],control[w_o],add_cash);
	fpadd(shares[w_o],shares[w_o],add_cash);
	fpadd(value[w_o],value[w_o],add_cash);

}

org_pur()
{
int i;

	++iss_num;
	w_o = iss_num;
	puts(CLEARS);
	puts("\n\n\tEqual amounts of money should be deposited in\n");
	puts("\tstock or mutual funds and money market funds.\n");
	puts("\n\tEnter name of stock or fund:\n");
	puts("\tFill all blanks with spaces.\n");
	puts("\tThen press space bar to continue..\n");
	puts("\n\n\t[..........]\b\b\b\b\b\b\b\b\b\b\b");
	for(i=0;(name[iss_num][i] = getchar())!='\n' && i<MAX_LET;++i);
	name[iss_num] [10] ='\0';
}
shs_pur()
{
char add_shs[5];
char new_con[5];
char add_cash[5];
	puts(CLEARS);
	printf("\nWorking on: %s", name[w_o]);
	puts("\n======================\n\n");
	puts("(enter all negative figures with a minus)\n\n");
	puts("\nEnter number of shares bought/sold....\t");
	atof(add_shs, gets(str));
	puts("Enter current price per share..........\t");
	atof(price[w_o], gets(str));
	fpmult(tot_inv,add_shs,price[w_o]);
	printf("\nThis transaction = %10.3f", tot_inv);
	fpadd(shares[w_o],shares[w_o],add_shs);
	fpmult(value[w_o],price[w_o],shares[w_o]);
	if(x==1){ 
		fpmult(control[w_o],price[w_o],shares[w_o]);
		fpmult(cost[w_o],price[w_o],shares[w_o]);
	}
	if(x==3){
		fpmult(new_con,tot_inv,m1);
		fpadd(control[w_o],control[w_o],new_con);
		fpadd(cost[w_o],cost[w_o],tot_inv);
	}
	if(x==4){
		fpadd(cost[w_o],cost[w_o],tot_inv);
	}
	if(x==5){
		fpmult(new_con,tot_inv,m2);
		fpadd(control[w_o],control[w_o],new_con);			fpadd(cost[w_o],cost[w_o],tot_inv);
	}
	hold();
}

hold()
{
	puts("\n\nPress <RETURN> to continue...");
	getchar();
}

list()
{
int i;
	puts(CLEARS);
	i = 0;
	printf("\n\n\t\tNumber\t\tName\n");
	while(i != iss_num+1){
		printf("\n\t\t%6d\t\t%s",i,name[i]);
	++i;
	}
}
ind_rep()
{
char p_l[5];
char ave_cost[5];
char port_val[5];

	atof(port_val,"0");
	atof(p_l, "0");
	puts(CLEARS);
puts("Name of    Shares    Cur       Cur       Ave       Control    Buy/Sell\n");
puts("Issue      Owned     Price     Value     Cost      Amount     Advice\n");
puts("========== ========= ========  ========= ========  ========== =============");
puts("\n\n");
	for(w_o=0; w_o < iss_num +1; ++w_o){
		aim();
		fpsub(p_l, value[w_o], cost[w_o]);
		fpdiv(ave_cost,cost[w_o],shares[w_o]);
		printf("%s",name[w_o]);
		printf("%10.3f",shares[w_o]);
		printf("%9.3f",price[w_o]);
		printf("%11.3f",value[w_o]);
		printf("%9.3f",ave_cost);
		printf("%11.3f",control[w_o]);
		printf("%10.2f %s\n",aim_rec[w_o],bs[w_o]);
		fpadd(port_val,port_val,value[w_o]);
	}
puts("\n\n");
puts("===========================================================================");
	printf("\nPorfolio  Value: %11.3f",port_val);
	hold();
}
sel_iss()
{
	puts("\n\nSelect issue number:  ");
	scanf("%d", &w_o);
}

new_add()
{
int z;
	puts(CLEARS);
	puts("Select 1) New issue or 2) Additional stock\n");
	scanf("%d", &z);
	switch(z){
		case 1:
			org_pur();
			shs_pur();
			break;
		case 2:
			list();
			sel_iss();
			shs_pur();
			break;
		default:
			new_add();
	}
}
up_date()
{
	puts(CLEARS);
	printf("\nEnter price for:  %s: ", name[w_o]);
	atof(price[w_o], gets(str));
	fpmult(value[w_o],price[w_o],shares[w_o]);
}
save()
{
char iobuf[BUFSIZ];
int a,b,c;
	puts(CLEARS);
	puts("Saving file to disk ...");
	if(iss_num < 0){
		puts("CAUTION !!! file is empty.");
		hold();
		menu();
	}
	if(fcreat("AIM.FIL",iobuf) == ERROR){
		puts("\nCan't create AIM.FIL\n");
		return(ERROR);
	}
	putw(iss_num,iobuf);
	for(a=0; a < iss_num +1; ++a){
		for(b=0; b < MAX_LET; ++b){
			c = name[a] [b];
			putw(c,iobuf);
		}
	}
	for(a=0; a< iss_num + 1; ++a){
		for(b=0; b < 5; ++b){
			c = shares[a] [b];
			putw(c,iobuf);
			c = price[a] [b];
			putw(c,iobuf);
			c = cost[a] [b];
			putw(c,iobuf);
			c = value[a] [b];
			putw(c,iobuf);
			c = control[a] [b];
			putw(c,iobuf);
			c = aim_rec[a] [b];
			putw(c,iobuf);
		}
	}
	fflush(iobuf);
	fclose(iobuf);
}
load()
{
char iobuf[BUFSIZ];
int a,b,c;
	puts(CLEARS);
	puts("Loading file ...");
	if(fopen("AIM.FIL",iobuf) == ERROR){
		puts("\nCan't open AIM.FIL\n");
		return(ERROR);
	}
	iss_num = getw(iobuf);
	for(a=0; a < iss_num +1; ++a){
		for(b=0; b < MAX_LET; ++b)
			name[a] [b] = getw(iobuf);
	}
	for(a=0; a < iss_num +1; ++a){
		for(b=0; b < 5; ++b){
			shares[a] [b] = getw(iobuf);
			price[a] [b] = getw(iobuf);
			cost[a] [b] = getw(iobuf);
			value[a] [b] = getw(iobuf);
			control[a] [b] = getw(iobuf);
			aim_rec[a] [b] = getw(iobuf);
		}
	}
	fclose(iobuf);
}
aim()
{
char safe[5];
char test[5];
int y;
	fpmult(safe,value[w_o],m4);
	atof(test, "100");
	y = fpcomp(value[w_o],control[w_o]);
	if(y==1){
		fpsub(aim_rec[w_o],value[w_o],control[w_o]);
		fpsub(aim_rec[w_o],aim_rec[w_o],safe);
		strcpy(bs[w_o], "Sell");
	}
	if(y==-1){
		fpsub(aim_rec[w_o],control[w_o],value[w_o]);
		fpsub(aim_rec[w_o],aim_rec[w_o],safe);
		strcpy(bs[w_o], "Buy ");
	}
	if(y==0)
		atof(aim_rec[w_o], "0");
	y = fpcomp(aim_rec[w_o], test);
	if(y==-1){
		atof(aim_rec[w_o], "0");
		strcpy(bs[w_o], "None");
	}
}
delete()
{
int i,y;
		puts(CLEARS);
		list();
		sel_iss();
		while(w_o < iss_num+1){
			for(i=0; i < 6; ++i){
				name[w_o] [i] = name[w_o +1] [i];
				shares[w_o] [i] = shares[w_o + 1] [i];
				price[w_o] [i] = price[w_o +1] [i];
				cost[w_o] [i] = cost [w_o +1] [i];
				value[w_o] [i] = value[w_o +1] [i];
				control[w_o] [i] = control[w_o +1] [i];
				aim_rec[w_o] [i] = aim_rec[w_o +1] [i];
			}
		++w_o;
		}
		iss_num = iss_num -1;
}

/* remark end of file aim.c */
 test[