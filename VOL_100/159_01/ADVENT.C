
/*  program ADVENT.C    modified by LCC for V 1.43 by:
        altering buffer sizes
  
        Changed call of exec() to static call for Eco-C88 (BW)
        Added function initw() for Eco-C88                (BW)
	Made function init() static for Eco-C88 	  (BW)

May 1990 - Bob Withers
	- Ported code to Microsoft C V 5.10 and 6.00
	- Placed all global variables in header ADVENT.H
	- Removed most runtime variable initialization and replaced
	  by compile time initializers.
	- Added file ADVENTDB.C to completely replace all the adventure
	  data base files.  This keeps all data in memory and eliminates
	  all disk accesses during game play.  All functions in DATABASE.C
	  were modified to access in memory data.
	- Added code to support the BRIEF and VERBOSE verbs.
	- Added code to ignore CTRL-C signal.
	- Modified vocab search to use a binary search.
*/

#define DRIVER

#include "advent.h"
#include <conio.h>
#include <signal.h>

static VOID	NEAR PASCAL init(VOID);
static VOID	NEAR PASCAL eadvent(VOID);


int main(int argc, char **argv)
{
    auto     SHORT	rflag;	    /* user restore request option */
    auto     SHORT	dflag;	    /* user restore request option */

    brief_sw = dbgflg = rflag = 0;
    signal(SIGINT, SIG_IGN);

    while (--argc > 0)
    {
	if ((argv[argc])[0] == '-' || (argv[argc])[0] == '/')
	{
	    switch (tolower((argv[argc])[1]))
	    {
		case 'r':
		    ++rflag;
		    continue;
		case 'd':
		    ++dbgflg;
		    printf("Debug enabled.\n");
		    continue;
		case 'b':
		    ++brief_sw;
		    printf("Brief mode enabled.\n");
		    continue;
		default:
		    printf("unknown flag: %c\n", (argv[argc])[1]);
		    continue;
	    }
	}
    }

    dflag = dbgflg;
    init();

    if (!rflag)
    {
	printf("\nDo you want to restore a saved game? (Y/N) ");
	do
	{
	    rflag = getch();
	    rflag = toupper(rflag);
	} while (!(rflag == 'Y' || rflag == 'N'));
	putchar(rflag);
	putchar('\n');
	if (rflag == 'N')
	    rflag = 0;
    }

    if (rflag)
	restore();

    dbgflg = dflag;
    eadvent();
    return(0);
}

/*  Initialization							*/

static VOID NEAR PASCAL init(VOID)
{
    dloc[DWARFMAX - 1] = chloc;
    return;
}

/*  restore saved game handler	*/

VOID PASCAL restore(VOID)
{
    auto     char	username[14];
    auto     FILE      *restfd;
    auto     SHORT	c, cnt;
    auto     char      *sptr;

    printf("What is your saved game name? ");
    scanf("%8s", username);
    for (sptr = username; *sptr; sptr++)
    {
        *sptr = (char) toupper(*sptr);
	if ((!isprint(*sptr)) || *sptr == ' ')
	{
	    *sptr = '\0';
	    break;
	}
    }

    if (strlen(username) == 0)
    {
	printf("\nNo name entered, restore request ignored.\n");
	return;
    }

    strcat(username, ".ADV");
    printf("\nOpening save file \"%s\".\n", username);
    if ((restfd = fopen(username, READ_BIN)) == 0)
    {
	printf("sorry, can't open save file...\n");
	exit(1);
    }
    cnt = 0;
    for (sptr = (char *) &turns; sptr < (char *) &lastglob; sptr++)
    {
	if ((c = getc(restfd)) == -1)
	{
	    printf("Can't read save file...\n");
	    if (dbgflg)
		printf("%d characters read\n", cnt);
	    fclose(restfd);
	    exit(1);
	}
	*sptr = (char) c;
	cnt++;
    }

    if (fclose(restfd) == -1)
	printf("warning -- can't close save file...\n");

    printf("\n");
    saveflg = 0;

    return;
}


static VOID NEAR PASCAL eadvent(VOID)
{
    srand(511);
    if (yes(65, 1, 0))
	limit = 1000;
    else
	limit = 330;

    while (!saveflg)
	turn();

    if (saveflg)
	saveadv();
    return;
}
