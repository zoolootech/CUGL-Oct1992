#include <dos.h>
#include <stdio.h>
#include <stdlib.h>

long int get_long_date()
/*
旼컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴커
쿛urpose: Get the current date.                                      �
�                                                                    �
� Inputs: None.                                                      �
�                                                                    �
쿚utputs: None.                                                      �
�                                                                    �
� Return: A long int containing the current date in the format       �
�         YYMMDD.                                                    �
읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴켸
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