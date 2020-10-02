/****************************************************************************/
/*                                                                          */
/*                                                                          */
/*      stest.c  v1.3  (c) 1990  Ken Harris                                 */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
/*                                                                          */
/*      This software is made available on an AS-IS basis. Unrestricted     */
/*      use is granted provided that the copyright notice remains intact.   */
/*      The author makes no warranties expressed or implied.                */
/*                                                                          */
/****************************************************************************/

#include <stdio.h>
#include "db.h"

main()
{
        DATA_SET ds;
        static char     data[50];
        int      i;

	printf("stest - Test sequential file routines\n\n");
	screate();
        printf("...test file created...\n\n");

	ds = db_open("", "stest.dat");
        if (db_error)
        {       printf("Open Failure - %s\n", db_error_msg(db_error));
                exit(0);
        }

        printf("...adding 100 records...\n");
        for (i=1; i<100; i++)
        {       printf("%d\n",i);
                sprintf(data,"%04d",i);
                db_add(ds, data);
                if (db_error)
                {       printf("Add Failure - %s\n", db_error_msg(db_error));
                        exit(0);
                }
        }
        printf("...add complete...\n\n");

	printf("...read forward...\n");
	db_read_first(ds, data);
	while (!db_error)
	{	printf("%5.5s\n", data);
		db_read_next(ds, data);
	}
	printf("...read forward complete...\n\n");

        printf("...read reverse...\n");
	db_read_last(ds, data);
	while (!db_error)
	{	printf("%5.5s\n", data);
		db_read_prev(ds, data);
	}
	printf("...read reverse complete...\n\n");

	printf("...delete forward...\n");
	db_read_first(ds, data);
	while (!db_error)
	{	printf("%5.5s\n", data);
		db_delete(ds);
		db_read_next(ds, data);
	}
	printf("...delete forward complete...\n\n");

        printf("...adding 100 records (again) ...\n");
        for (i=1; i<100; i++)
        {       printf("%d\n",i);
                sprintf(data,"%05d",i);
                db_add(ds, data);
                if (db_error)
                {       printf("Add Failure - %s\n", db_error_msg(db_error));
                        exit(0);
                }
        }
        printf("...add complete...\n\n");

	printf("...delete reverse...\n");
	db_read_last(ds, data);
	while (!db_error)
	{	printf("%5.5s\n", data);
		db_delete(ds);
		db_read_prev(ds, data);
	}
	printf("...delete forward complete...\n\n");

        db_close(ds);
	unlink("stest.dat");
}

/*
 *	screate - create our test sequential file
 */

screate()
{
        DATA_SET ds;


	ds = db_create("", "stest.dat", "seq,rec=50");
        if (db_error)
        {       printf("Create Failure - %s\n", db_error_msg(db_error));
                exit(0);
        }

        db_close(ds);
        if (db_error)
        {       printf("Close Failure - %s\n", db_error_msg(db_error));
                exit(0);
        }
}
