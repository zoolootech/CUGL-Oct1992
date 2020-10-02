/* CHRONOR.C    VERS:- 01.00  DATE:- 09/26/86  TIME:- 09:36:16 PM */
/*
%CC1 $1.C -X 
%CLINK $1 -S
%DELETE $1.CRL 
*/
/* 
Description:  

Read Hayes Chronograph, as device PUN:/RDR:; return strings
for time, date, day of week, month, and displayable summary of these data.
Also, a logon header message.  

See arrays defined as externals below.

This program tests functions that might be incorporated into other programs.

By J. A. Rupley, Tucson, Arizona
Coded for the BDS C Compiler, version 1.50a
*/

#include "jario.h"
#include "bdscio.h"

#define PUNCH 6
#define READER 7
#define CR 0x0d

char time[12];
char date[9];
char day[10];
char month[20];
char date_mes[80];
char log_mes[80];

char weekday[8][10];
char year_month[13][10];


/* Fill time and date strings		*/
main(argc, argv)
int argc;
char **argv;
{
	strcpy(time, "hh:mm:ss XM");
	strcpy(date, "mm/dd/yy");
	strcpy(day, "Wednesday");
	strcpy(month, "September");

	strcpy(weekday[0], "Monday");
	strcpy(weekday[1], "Tuesday");
	strcpy(weekday[2], "Wednesday");
	strcpy(weekday[3], "Thursday");
	strcpy(weekday[4], "Friday");
	strcpy(weekday[5], "Saturday");
	strcpy(weekday[6], "Sunday");
	strcpy(weekday[7], "Illegal");

	strcpy(year_month[0], "Illegal");
	strcpy(year_month[1], "January");
	strcpy(year_month[2], "February");
	strcpy(year_month[3], "March");
	strcpy(year_month[4], "April");
	strcpy(year_month[5], "May");
	strcpy(year_month[6], "June");
	strcpy(year_month[7], "July");
	strcpy(year_month[8], "August");
	strcpy(year_month[9], "September");
	strcpy(year_month[10], "October");
	strcpy(year_month[11], "November");
	strcpy(year_month[12], "December");

	set_time();
	printf("\ntime = %s\n", time);

	date_set();
	printf("\ndate = %s\n", date);

	day_set();
	printf("\nday = %s\n", day);

	set_month();
	printf("\nmonth = %s\n", month);

	mes_set();
	printf("\nmessage = %s\n", date_mes);

	set_log();
	printf("\nlog message = %s\n", log_mes);

}
/* END OF MAIN				*/


/* Fill time string			*/
int set_time()
{
	char clock_in[12];
	int i, j;

	hayes_chrono("RT\r", clock_in);
	/* Clock_in gives hhmmss x	*/
	/* Convert to     hh:mm:ss: xm	*/
	for (j = 0, i = 0; clock_in[i] != NULL; j++, i++)
	{
		if (i == 2 || i == 4 || i == 6)
			j++;
		time[j] = clock_in[i];
	}
	return;
}
/*END OF SET_TIME			*/


/* Fill date string			*/
int date_set()
{
	char clock_in[12];
	int i, j;

	hayes_chrono("RD\r", clock_in);
	/* Clock_in gives yymmdd	*/
	/* Convert to     mm/dd/yy	*/
	for (j = 6, i = 0; clock_in[i] != NULL; j++, i++)
	{
		if (j == 8)
			j = 0;
		if (i == 4)
			j++;
		date[j] = clock_in[i];
	}

	return;
}
/*END OF DATE_SET			*/


/* Fill day string			*/
int day_set()
{
	/* Clock_in gives  d = acsii #	*/
	/* Convert to int ptr to string	*/
	/* Monday is  d = '0'		*/
	char clock_in[12];
	int i;

	hayes_chrono("RW\r", clock_in);
	i = atoi(clock_in);
	if (i < 0 || i > 6)
		i = 7;
	strcpy(day, weekday[i]);
	return;
}
/*END OF DAY_SET			*/


/* Fill month string			*/
int set_month()
{
	char c[20];
	int i;

	strcpy(c, date);
	c[3] = '\0';
	i = atoi(c);
	if (i < 1 || i > 12)
		i = 0;
	strcpy(month, year_month[i]);
	return;
}
/*END OF SET_MONTH			*/


/* Fill message string			*/
int mes_set()
{
	char c[20];

	strcpy(date_mes, time);
	strcat(date_mes, "  on ");
	strcat(date_mes, day);
	strcat(date_mes, " ");
	strcat(date_mes, month);
	strcat(date_mes, " ");
	strcpy(c, &date[3]);
	c[2] = '\0';
	strcat(date_mes, c);
	strcat(date_mes, ", 19");
	strcpy(c, &date[6]);
	c[2] = '\0';
	strcat(date_mes, c);
	return;
}
/*END OF SET_MES			*/


/* Fill logon string			*/
int set_log()
{
	strcpy(log_mes, "LOGON     DATE:- ");
	strcat(log_mes, date);
	strcat(log_mes, "  TIME:- ");
	strcat(log_mes, time);
	return;
}
/*END OF SET_LOG			*/


/* Send command to clock 
					and return with string		*/
int hayes_chrono(command, buffer)
char *command, *buffer;
{
	char temp[12];
	int i;

	strcpy(temp, "AT");
	strcat(temp, command);        /* Form command			*/

	for (i = 0; temp[i] != NULL; i++)
		bios(PUNCH, temp[i]);        /* Use direct bios call		*/

	for (i = 0; i <= 11; i++)
	{
		temp[i] = bios(READER, 0);
		if (temp[i] == CR)
			break;
	}
	temp[i] = '\0';

	strcpy(buffer, temp);
	printf("\nbuffer = %s", buffer);
	return;
}
/* END OF HAYES_CHRONO			*/
