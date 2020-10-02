/*
	Header for time and date routines

	source:   timedate.h
	version:  February 22, 1983


	All software tools should have the following form:


	#include bdscio.h
	#include dio.h
	#include timedate.h		(optional)
	#include tools.con		(optional)

	main(argc, argv)
	char **argv;
	{
		_allocp = NULL;
		dioinit(&argc, argv);
		wildexp(&argc, &argv);	(optional)
		main1(argc, argv);	(do the real work)
		dioflush();
	}

*/


/* seed for random number generators */
int sys_seed;

/* time and date */

#define MAXLINE 200
char sys_date [MAXLINE];
* seed for random number generators */
int sys_seed;

/* 