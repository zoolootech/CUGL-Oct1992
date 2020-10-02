/****************************************************************************/
/*                                                                          */
/*                                                                          */
/*      dtest.c  v1.3  (c) 1990  Ken Harris                                 */
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

char *name_list[] = {"Tom", "Dick", "Fred", "Allen", "Mike", "Sam", "Harry",
		     "William", "Bob", "Ted", ""};

main()
{	DICT d;
	char **names, **data, buf[5];
	int cnt,i;

	printf("Creating dict...\n");
	d = db_dict_init();
	if (db_error)
	{	printf("%s\n",db_error_msg(db_error));
		exit(0);
	}

	printf("Adding names...\n");
	for (i=0; *name_list[i]; i++)
	{	sprintf(buf,"%04d", i);
		printf("%s  %s\n",name_list[i], buf);
		db_dict_add(d, name_list[i],  1, strlen(buf)+1, buf);
		if (db_error)
		{	printf("%s\n",db_error_msg(db_error));
			exit(0);
		}
	}

	printf("Find all and print...\n");
	cnt = db_dict_find_all(d, 0, &names, &data);
	if (db_error)
	{	printf("%s\n",db_error_msg(db_error));
		exit(0);
	}

	printf("cnt = %d\n",cnt);
	for (i=0; i<cnt; i++)
	{	printf("  %s  %s\n",names[i],data[i]);
	}
	free(names);
	free(data);

	printf("Dump...\n");
	db_dict_dump(d,"","dtest.dat");
	if (db_error)
	{	printf("%s\n",db_error_msg(db_error));
		exit(0);
	}

	printf("Close...\n");
	db_dict_close(d);
	if (db_error)
	{	printf("%s\n",db_error_msg(db_error));
		exit(0);
	}

	printf("Init...\n");
	d = db_dict_init();
	if (db_error)
	{	printf("%s\n",db_error_msg(db_error));
		exit(0);
	}

	printf("Load...\n");
	db_dict_load(d,"","dtest.dat");
	if (db_error)
	{	printf("%s\n",db_error_msg(db_error));
		exit(0);
	}

	printf("Find all and print...\n");
	cnt = db_dict_find_all(d, 0, &names, &data);
	if (db_error)
	{	printf("%s\n",db_error_msg(db_error));
		exit(0);
	}

	printf("cnt = %d\n",cnt);
	for (i=0; i<cnt; i++)
	{	printf("  %s  %s\n",names[i],data[i]);
	}
	free(names);
	free(data);

	printf("Delete a few...\n");
	for (i=0; *name_list[i];)
	{	printf("%s\n",name_list[i]);
		db_dict_delete(d, name_list[i],  1);
		if (db_error)
		{	printf("%s\n",db_error_msg(db_error));
			exit(0);
		}
		i++;
		if (*name_list[i]) i++;
	}

	printf("Find all and print...\n");
	cnt = db_dict_find_all(d, 0, &names, &data);
	if (db_error)
	{	printf("%s\n",db_error_msg(db_error));
		exit(0);
	}

	printf("cnt = %d\n",cnt);
	for (i=0; i<cnt; i++)
	{	printf("  %s  %s\n",names[i],data[i]);
	}
	free(names);
	free(data);

	printf("Close...\n");
	db_dict_close(d);
	if (db_error)
	{	printf("%s\n",db_error_msg(db_error));
		exit(0);
	}
	unlink("dtest.dat");
}
