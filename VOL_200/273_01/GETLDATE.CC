#include <dos.h>
#include <stdio.h>
#include <stdlib.h>

long int get_long_date()
/*
ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
³Purpose: Get the current date.                                      ³
³                                                                    ³
³ Inputs: None.                                                      ³
³                                                                    ³
³Outputs: None.                                                      ³
³                                                                    ³
³ Return: A long int containing the current date in the format       ³
³         YYMMDD.                                                    ³
ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
*/
{

struct date today;
char mystr[40];
long l_date;

	getdate(&today);
	sprintf(mystr,"%2.2d%2.2d%2.2d",
		today.da_year - 1900,today.da_mon,today.da_day);

    l_date = atol(mystr);
	return(l_date);
}
