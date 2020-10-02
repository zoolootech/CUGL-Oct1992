/****************************************************************************/
/*                                                                          */
/*                                                                          */
/*      vtest.c  v1.3  (c) 1990  Ken Harris                                 */
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
        char     data[50];
        int      i, size;
	long     psn;

	printf("vtest - Test variable file routines\n\n");
	vcreate();
        printf("...test file created...\n\n");

	ds = db_open("", "vtest.dat");
        if (db_error)
        {       printf("Open Failure - %s\n", db_error_msg(db_error));
                exit(0);
        }

        printf("...adding 100 records...\n");
        for (i=1; i<=100; i++)
        {       sprintf(data,"%06d",i);
                psn = db_add(ds, data, 6);
                printf("%d (psn=%ld)\n",i,psn);

                if (db_error)
                {       printf("Add Failure - %s\n", db_error_msg(db_error));
                        exit(0);
                }

        }
        printf("...add complete...\n\n");

	printf("...reorg...\n");
	db_reorg(ds);
	if (db_error) printf("Reorg Failure - %s\n", db_error_msg(db_error));
	printf("...reorg complete...\n");

	printf("...read forward...\n");
	db_read_first(ds, data, &size);
	while (!db_error)
	{	psn = db_get_rec_no(ds);
		printf("%6.6s (size=%d  psn=%ld)\n", data, size, psn);
		data[0] = '_';
		db_update(ds, data);
		db_read_next(ds, data, &size);
	}
	printf("...read forward complete...\n\n");

	printf("...reorg...\n");
	db_reorg(ds);
	if (db_error) printf("Reorg Failure - %s\n", db_error_msg(db_error));
	printf("...reorg complete...\n");

        printf("...read reverse...\n");
	db_read_last(ds, data, &size);
	while (!db_error)
	{	psn = db_get_rec_no(ds);
		db_read_direct(ds, psn, data, &size);
		if (db_error)
		{	printf("%s\n",db_error_msg(db_error));
			break;
		}
		printf("%6.6s (size=%d  psn=%ld)\n", data, size, psn);
		db_read_prev(ds, data, &size);
	}
	printf("...read reverse complete...\n\n");

	printf("...reorg...\n");
	db_reorg(ds);
	if (db_error) printf("Reorg Failure - %s\n", db_error_msg(db_error));
	printf("...reorg complete...\n");

	printf("...delete forward...\n");
	db_read_first(ds, data, &size);
	while (!db_error)
	{	printf("%6.6s\n", data);
		db_delete(ds);
		db_read_next(ds, data, &size);
	}
	printf("...delete forward complete...\n\n");

	printf("...reorg...\n");
	db_reorg(ds);
	if (db_error) printf("Reorg Failure - %s\n", db_error_msg(db_error));
	printf("...reorg complete...\n");

	printf("...read forward...\n");
	db_read_first(ds, data, &size);
	while (!db_error)
	{	printf("%6.6s (%d)\n", data, size);
		db_read_next(ds, data, &size);
	}
	printf("...read forward complete...\n\n");

        printf("...read reverse...\n");
	db_read_last(ds, data, &size);
	while (!db_error)
	{	printf("%6.6s (%d)\n", data, size);
		db_read_prev(ds, data, &size);
	}
	printf("...read reverse complete...\n\n");

        printf("...adding 100 records (again) ...\n");
        for (i=1; i<=100; i++)
        {       printf("%d\n",i);
                sprintf(data,"%06d",i);
                db_add(ds, data, 6);
                if (db_error)
                {       printf("Add Failure - %s\n", db_error_msg(db_error));
                        exit(0);
                }
        }
        printf("...add complete...\n\n");

	printf("...read forward...\n");
	db_read_first(ds, data, &size);
	while (!db_error)
	{	printf("%6.6s (%d)\n", data, size);
		db_read_next(ds, data, &size);
	}
	printf("...read forward complete...\n\n");

        printf("...delete reverse...\n");
	db_read_last(ds, data, &size);
	while (!db_error)
	{	printf("%6.6s (%d)\n", data, size);
		db_delete(ds);
		db_read_prev(ds, data, &size);
	}
	printf("...delete reverse complete...\n\n");

	printf("...read forward...\n");
	db_read_first(ds, data, &size);
	while (!db_error)
	{	printf("%6.6s (%d)\n", data, size);
		db_read_next(ds, data, &size);
	}
	printf("...read forward complete...\n\n");

	printf("...reorg...\n");
	db_reorg(ds);
	if (db_error) printf("Reorg Failure - %s\n", db_error_msg(db_error));
	printf("...reorg complete...\n");

        db_close(ds);
	unlink("vtest.dat");
}

/*
 *	vcreate - create our test variable file
 */

vcreate()
{
        DATA_SET ds;


	ds = db_create("", "vtest.dat", "var,rec=50");
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
