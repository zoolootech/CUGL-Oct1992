/* whelp
 *		Context-sensitive help system.
 *		The help system is installed with a file reference.
 *		The program looks for help files with the extension .HLP and .HX
 *			in the 'home directory' specified by whome_dir
 *		
 *		HELP is displayed until ESCAPE is pressed.
 *		F1 again displays a list of HELP topics.
 *
 */
#include "wsys.h"

#ifndef __TURBOC__
	/* this is MSC */
	#include "direct.h"
	#define MAXPATH  80
#else  
	/* __TURBOC__ */
	#include <dir.h>
#endif


#include "whelp.h"



static void help(void);


static char hname[MAXPATH] = {0};
static char xname[MAXPATH] = {0};


static char NOHELP[] = "Not available";





void whelp_install ( char *fn )
	{
	if ( strlen ( fn ) > 8 )
		{
		werror ('H', "Helpfile name > 8");
		}


	if ( ! *hname )
		{
		/* not yet installed
		 */
		whotkey_install ( FKEY(1), help );
		}
			
	strcpy ( hname, whome_dir );
	strcat ( hname, fn );
	strcat ( hname, ".HLP" );

	strcpy ( xname, whome_dir );
	strcat ( xname, fn );
	strcat ( xname, ".HX" );



	return;		/* whelp_install */
	}


static void help(void)
	{

	/* a list of ptrs into the file topics
	 */
	char **hxlist;
	int  key, n, ntopics, nbytes, complen, l_marg, t_marg;

	char *topic, *text;

	HX  *hx;

	FILE *hfile;

	topic = whelp_ptr;

	/* read the index file
	 */
	if ( NULL == (hfile =fopen (xname, "rb") ))
		{
		werror ('H', "HELP- open index");
		}
	if ( 1 != fread ( &ntopics, sizeof(ntopics), 1, hfile ) )
		{
		werror ('H', "HELP- read index");
		}
	hx = wmalloc ( sizeof (HX) * (1+ntopics), " HELP ");

	if ( ntopics != fread (hx, sizeof(HX), ntopics, hfile) )
		{
		werror ('H', "HELP- read index");
		}
	fclose (hfile);



	/* run through the topics and create list of ptrs to the strings
	 */
	hxlist = wmalloc ( sizeof (char *) * (ntopics+1), " HELP " );
	for ( n=0; n<ntopics; ++n )
		{
		hxlist[n] = hx[n].hxtopic;
		}
	hxlist[n] = NULL;




	wopen (10, 5, 60, 15, whelpattr, DOUBLE_BORDER, whelpattr,
			WSAVE2RAM);
	wtitle ( " HELP " );

	wbutton_add ("ESCAPE",     6, 13, 7, ESCAPE, WBTN_BOX );
	wbutton_add ("F1 TOPICS", 46, 13,10, FKEY(1),WBTN_BOX );

	do
		{
		/* find the topic in the index
		 */
		for ( n=0; n<ntopics; ++n )
			{
			/* shorten comparison lenght to exclude terminal 
			 * spaces, punctuation chars, newline chars etc...
			 * required because of method of generating help 
			 * for WFORM and WMENU
			 */
			complen = min ( strlen(topic), TOPIC_SIZE );
			while ( (complen > 0) 
				  && (strchr ( " :-.=?+\n\r", topic[complen-1] ) != NULL ) )
				{
				--complen;
				}

			if ( memicmp (hxlist[n], topic, complen )==0 )
				{
				/* found the topic - read in from file.
				 */
				nbytes = hx[n].hxsize;
				text = wmalloc ( nbytes+1, " HELP " );

				if ( NULL == (hfile = fopen ( hname, "rb" )))
					{
					werror ('H', "HELP -helpfileO");
					}
				if (0!=fseek (hfile, hx[n].hxpos, SEEK_SET) )
					{
					werror ('H', "HELP -helpfileS");
					}

				if (1!= fread (text, nbytes, 1, hfile))
					{
					werror ('H', "HELP -helpfileR");
					}
				fclose (hfile);

				text[nbytes] =0;

				break;
				}

			}
		if ( n == ntopics )
			{
			/* topic not found
			 * NOTE that 'text' is freed later, so must alloc
			 * 	even though we know exactly what to put.
			 */
			text = wmalloc (sizeof (NOHELP), " HELP ");
			strcpy (text,NOHELP);
			}



		/* print the help text
		 *		NOTE recalc position each time in case window is wmsdrag()'d
		 */
		l_marg = w0-> winleft + 5;
		t_marg = w0-> wintop  + 3; 
		 
		/* define a small window to create left and top margins for text.
		 * also, this clears previous text.
		 */
		wopen ( l_marg, t_marg, 50, 8, whelpattr, NO_BORDER, 0, WSAVE2NULL );
		wputs (text);
		free  (text);
		wabandon();

		wgoto ( 5, 1 );
		wclearline();
		wputs ("TOPIC: "); 
		wputs (topic);

		key = wgetc ();

		if ( key == FKEY(1) )
			{
			/* set location for list of topics above the "F1" button.
			 */
			wsetlocation ( WLOC_ATWIN, 46, 1 );

			topic = hxlist [ wpicklist ("help topics",
							hxlist ) ];

			if ( topic == NULL )  /* ESCAPE pressed */
				{
				topic = whelp_ptr;
				}
			}

		}
	while ( key != ESCAPE );

	wclose ();

	free (hx);

	return;		/* help */
	}
/*------------------ end of whelp.c ----------------------*/
