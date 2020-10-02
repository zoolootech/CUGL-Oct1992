/****************************************************************************/
/*                                                                          */
/*                                                                          */
/*      itest.c  v1.3  (c) 1990  Ken Harris                                 */
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

char     data[2040];
main()
{
        DATA_SET ds;
        int      i,j;

        memset(data,' ',2040);
        printf("itest - Test ISAM file routines\n\n");
        rcreate();
        printf("...test file created...\n\n");

        ds = db_open("", "itest.dat");
        if (db_error)
        {       printf("Open Failure - %s\n", db_error_msg(db_error));
                exit(0);
        }

        printf("...adding 100 records in reverse ...\n");
        for (i=100; i>0; i--)
        {       printf("%d\n",i);
                sprintf(data,"%05d",i);
                db_add(ds, data);
                if (db_error)
                {       printf("Add Failure - %s\n", db_error_msg(db_error));
                        exit(0);
                }
        }
        printf("...add complete...\n\n");

	printf("...read forward...\n\n");
        db_read_first(ds, data);
        while (!db_error)
        {       printf("%5s\n",data);
                db_read_next(ds, data);
        }
        if (db_error != DB_END_OF_FILE)
        {       printf("Read Sequential - %s\n", db_error_msg(db_error));
                exit(0);
        }
	printf("...read forward complete...\n\n");

	printf("...read reverse...\n\n");
	db_read_last(ds, data);
        while (!db_error)
        {       printf("%5s\n",data);
		db_read_prev(ds, data);
        }
        if (db_error != DB_END_OF_FILE)
        {       printf("Read Sequential - %s\n", db_error_msg(db_error));
                exit(0);
        }
	printf("...read reverse complete...\n\n");

        printf("...find and delete...\n");
        for (i=1; i<101; i++)
        {       printf("%d\n",i);
                sprintf(data,"%05d",i);

                db_find(ds, data, data, 0);
                if (db_error)
                {       printf("Find Failure - %s\n", db_error_msg(db_error));
                        exit(0);
                }
                db_delete(ds);
                if (db_error)
                {       printf("Delete Failure - %s\n", db_error_msg(db_error));
                        exit(0);
                }
        }
        printf("...find and delete complete...\n\n");

        printf("...adding 100 records (again) ...\n");
        for (i=1; i<101; i++)
        {       printf("%d\n",i);
                sprintf(data,"%05d",i);
                db_add(ds, data);
                if (db_error)
                {       printf("Add Failure - %s\n", db_error_msg(db_error));
                        exit(0);
                }
        }
        printf("...add complete...\n\n");

        db_close(ds);
        unlink("itest.dat");
}

/*
 *      rcreate - create our test random file
 */

rcreate()
{
        DATA_SET ds;


        ds = db_create("", "itest.dat", "idx, rec=25, blk=512, key=5");
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
