#include <stdio.h>
#include <c_wndw.h>
#include <c_ndx.h>
struct MAP {int field, col;};
void setup (int, struct DBF_DEF*, struct MAP*);
void record_display (int, struct MAP*, int);
int  record_accept  (int, struct MAP*, int);

/*
*	Copyright 1989, Marietta Systems, Inc.
*	All rights reserved
*/

/*
* * * * * * * * * * * * *
*   This function builds the entry control map array for the entry
*   of the fields.
*/
void setup (int fh, struct DBF_DEF *dict, struct MAP *map)
{
	int col = 1, i;
/* get the field numbers from data dictionary */
	map[0].field = dbffield (fh, "FIRSTNAME");
	map[1].field = dbffield (fh, "INITIAL");
	map[2].field = dbffield (fh, "LASTNAME");
	map[3].field = dbffield (fh, "NICKNAME");
	map[4].field = dbffield (fh, "INCOME");
	map[5].field = -1;
/* setup starting field locations on the screen */
	for (i = 0 ; i < 5 ; i++)
	{
		map[i].col = col;
		col += dict[map[i].field].dbf_len + 1;
	}
	display ("Firstname          Init Lastname            \
Nickname             Income", 1, 1, alt_low);
}

/*
* * * * * * * * * * * * *
*	This function displays the current record from the dBASE file
*/
void record_display (int fh, struct MAP *map, int row)
{
	int i;
	for (i = 0 ; i < 5 ; i++)
		dispdbf (fh, map[i].field, row, map[i].col, reverse);
}

/*
* * * * * * * * * * * * *
* 	This function accepts a record for the dBASE file
*	It returns 1 if the record is completely entered, o/w zero
*/
int record_accept (int fh, struct MAP *map, int row)
{
	int i, ret;
	for (i = 0; i < 5 && i >= 0 ; )
	{
		set_crsr (row, map[i].col); /* set cursor */
		ret = acptdbf (fh, map[i].field, alt_high);
		if (ret < 0) return 0; /* error */

		switch (ret)
		{
			case QUIT: case ESC: return 0;

			case 0: case ENTER: case CRS_RT: i++; break;

			case CRS_LT: i--; break;

			default: shortbeep(ret); break;
		}
	}
	return (i < 0 ? 0 : 1);
}

/*
* * * * * * * * * * * * * * * *
* 	This program reads opens a dBASE file and its associated index file.
*	Then reads sequentially through the file to allow the operator to
*	modify the INCOME field.
*	This continues until the end of the file or the F10 key is pressed.
*/

void main ()
{
	 int fh, ndx, ret, x, action = 1;
	 struct DBF_DEF *dict;
	 struct MAP map[6];
/* initialize */
	 clr_scrn ("Test dBASE screen I/O");
	 set_keys (3, ESC, CRS_RT, CRS_LT);
	 fh = fileopen ("MARIETTA\\FRIENDS.DBF", dbase3, update);
	 if (!fh) goodbye (err_wndw (" Cannot find FRIENDS.DBF file ", 0, 0));
	 dict = FN[fh].fnext;     /* dBASE data dictionary */
	 ndx = ndxopen (fh, "MARIETTA\\FRIENDS.NDX"); /* index file */
	 if (ndx <= 0) goodbye (10);
	 setup (fh, dict, map);

/* read and display the first record */
	 ret = ndxread (ndx, firstrec);
	 record_display (fh, map, (x = 2));

 /* read and update file */
	for (;;)
	{
		flt_wndw (5, 20, "Select action");
		action = menu ("Next record&Previous record&Modify record&\
Add a record&Quit (or press ESC)&", 20, action, 1);
		rm_wndw(); /* remove menu window */

		switch (action)
		{
			case 1: /* next record */
				ret = ndxread (ndx, nextrec);
				if (ret <= 0) err_wndw (" End of file reached ", 0, 9);
				else record_display (fh, map, ++x);
				break;

			case 2: /* previous record */
				ret = ndxread (ndx, previous);
 				if (ret <= 0) err_wndw (" End of file reached ", 0, 9);
				else record_display (fh, map, ++x);
				break;

			case 3: /* modify record */
				ret = record_accept (fh, map, x);
				if (ret) ndxwrite (ndx); /* rewrite modified record */
				break;

			case 4: /* add new record */
				memset (FN[fh].record, 32, FN[fh].rec_len); /* clear record */
				*FN[fh].record = IN_USE;
				ret = record_accept (fh, map, ++x);
				if (ret) ndxappend (ndx); /* write new record */
				break;

			default: /* end with F10 or Esc */
				fileclos (fh);
				goodbye (0);
		}
     }
}
