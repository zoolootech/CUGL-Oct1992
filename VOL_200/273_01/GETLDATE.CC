#include <dos.h>
#include <stdio.h>
#include <stdlib.h>

long int get_long_date()
/*
��������������������������������������������������������������������Ŀ
�Purpose: Get the current date.                                      �
�                                                                    �
� Inputs: None.                                                      �
�                                                                    �
�Outputs: None.                                                      �
�                                                                    �
� Return: A long int containing the current date in the format       �
�         YYMMDD.                                                    �
����������������������������������������������������������������������
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