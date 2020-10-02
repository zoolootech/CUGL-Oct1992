
/*  programs in DATABASE.C    no changes for V 1.43			*/

#include "advent.h"

static VOID	NEAR PASCAL DisplayText(char *pTbl[], SHORT sMsg);


/*  Routine to fill travel array for a given location.	*/

VOID PASCAL gettrav(SHORT loc)
{
    extern  TRAVTAB	TravTab[];

    --loc;
    pTravel  = TravTab[loc].pTrav;
    sTravCnt = TravTab[loc].sTrav;
    return;
}

/*  Routine to request a yes or no answer to a question.		    */

SHORT PASCAL yes(SHORT msg1, SHORT msg2, SHORT msg3)
{
    auto char       answer[80];

    if (msg1)
	rspeak(msg1);
    putchar('>');
    gets(answer);

    if (tolower(answer[0]) == 'n')
    {
	if (msg3)
	    rspeak(msg3);
	return(0);
    }

    if (msg2)
	rspeak(msg2);

    return(1);
}


/*  Print a random message from database 6				    */

VOID PASCAL rspeak(SHORT msg)
{
    extern  char      *pTextMsg[];

    if (dbgflg)
	printf("** rspeak(%d) ** ", msg);

    DisplayText(pTextMsg, msg);

    return;
}


/*  Routine to print the message for an item in a given state.		    */

VOID PASCAL pspeak(SHORT item, SHORT state)
{
    register char     *p;
    extern   char     *pObjDesc[];

    if (dbgflg)
	printf("** pspeak(%d,%d) ** ", item, state);

    p = pObjDesc[item - 1];
    while (state > -1)
    {
	while (*p && '/' != *p)
	    ++p;

	if (NUL == *p)
	    return;

	++p;
	--state;
    }

    while (TRUE)
    {
	if (NUL == *p || '/' == *p)
	    break;

	putchar(*p);
	++p;
    }

    putchar('\n');

    return;
}


/*  Print the long description of a location				    */

VOID PASCAL desclg(SHORT loc)
{
    extern   char     *pLongRmDesc[];

    if (dbgflg)
	printf("** desclg(%d) ** ", loc);

    DisplayText(pLongRmDesc, loc);
    putchar('\n');
    return;
}


/*  Print the short description of a location				    */

VOID PASCAL descsh(SHORT loc)
{
    extern   char      *pShortRmDesc[];

    if (dbgflg)
	printf("** descsh(%d) ** ", loc);

    DisplayText(pShortRmDesc, loc);
    putchar('\n');
    return;
}


static VOID NEAR PASCAL DisplayText(char *pTbl[], SHORT sMsg)
{
    auto     char      *p;

    --sMsg;

    if ('@' == *pTbl[sMsg])
    {
	p = pTbl[sMsg] + 1;
	while (*p)
	{
	    DisplayText(pTbl, atoi(p));
	    while (*p && ',' != *p)
		++p;

	    if (',' == *p)
		++p;
	}
    }
    else
	puts(pTbl[sMsg]);

    return;
}


/*  routine to look up a vocabulary word.
        word is the word to look up.
        val  is the minimum acceptable value,
                if != 0 return %1000
*/
SHORT PASCAL vocab(char *word, SHORT val)
{
    register SHORT	i;
    register SHORT	left, right;
    auto     SHORT	sCmp;
    extern   SHORT	sVocabCount;
    extern   VOCABTAB	VocabTab[];

    left  = 0;
    right = sVocabCount - 1;

    while (right >= left)
    {
	i = (left + right) / 2;

	sCmp = *word - *VocabTab[i].pWord;
	if (0 == sCmp)
	    sCmp = strcmp(word, VocabTab[i].pWord);

	if (0 == sCmp)
	    return(val ? VocabTab[i].sWord % 1000 : VocabTab[i].sWord);

	if (sCmp < 0)
	    right = i - 1;
	else
	    left  = i + 1;
    }

    return(-1);
}

/*
        Utility Routines
*/

/*
        Routine to test for darkness
*/
BOOL PASCAL dark(VOID)
{
    return(!(cond[loc] & LIGHT) && (!prop[LAMP] || !here(LAMP)));
}

/*
        Routine to tell if an item is present.
*/
BOOL PASCAL here(SHORT item)
{
     return (place[item] == loc || toting(item));
}

/*
        Routine to tell if an item is being carried.
*/
BOOL PASCAL toting(SHORT item)
{
    return(place[item] == -1);
}

/*
        Routine to tell if a location causes
        a forced move.
*/
BOOL PASCAL forced(SHORT atloc)
{
    return(cond[atloc] == 2);
}

/*
        Routine true x% of the time.
*/
BOOL PASCAL pct(SHORT x)
{
    return(rand() % 100 < x);
}

/*
        Routine to tell if player is on
        either side of a two sided object.
*/
BOOL PASCAL at(SHORT item)
{
    return(place[item] == loc || fixed[item] == loc);
}

/*
        Routine to destroy an object
*/
VOID PASCAL dstroy(SHORT obj)
{
                    move(obj, 0);
    return;
}

/*
        Routine to move an object
*/
VOID PASCAL move(SHORT obj, SHORT where)
{
    auto SHORT      from;

    from = (obj < MAXOBJ) ? place[obj] : fixed[obj];
    if (from > 0 && from <= 300)
	carry(obj, from);
    drop(obj, where);
    return;
}

/*
        Juggle an object
        currently a no-op
*/
VOID PASCAL juggle(SHORT loc)
{
    loc = loc;				    /* eliminate compiler warning */
    return;
}

/*
        Routine to carry an object
*/
VOID PASCAL carry(SHORT obj, SHORT where)
{
    where = where;			   /* eliminate compiler warning */
    if (obj < MAXOBJ)
    {
	if (place[obj] == -1)
	    return;
	place[obj] = -1;
	++holding;
    }
    return;
}

/*
        Routine to drop an object
*/
VOID PASCAL drop(SHORT obj, SHORT where)
{
    if (obj < MAXOBJ)
    {
	if (place[obj] == -1)
	--holding;
	place[obj] = where;
    }
    else
	fixed[obj - MAXOBJ] = where;

    return;
}

/*
        routine to move an object and return a
        value used to set the negated prop values
        for the repository.
*/
SHORT PASCAL put(SHORT obj, SHORT where, SHORT pval)
{
    move(obj, where);
    return((-1) - pval);
}

/*
        Routine to check for presence
        of dwarves..
*/
SHORT PASCAL dcheck(VOID)
{
    register SHORT  i;

    for (i = 1; i < (DWARFMAX - 1); ++i)
    {
	if (dloc[i] == loc)
	    return(i);
    }
    return(0);
}

/*
        Determine liquid in the bottle
*/
SHORT PASCAL liq(VOID)
{
    auto     SHORT	i, j;

    i = prop[BOTTLE];
    j = (-1) - i;
    return(liq2(i > j ? i : j));
}

/*
        Determine liquid at a location
*/
SHORT PASCAL liqloc(SHORT loc)
{
    if (cond[loc] & LIQUID)
	return(liq2(cond[loc] & WATOIL));

    return(liq2(1));
}

/*
        Convert  0 to WATER
                 1 to nothing
                 2 to OIL
*/
SHORT PASCAL liq2(SHORT pbottle)
{
    return((1 - pbottle) * WATER + (pbottle >> 1) * (WATER + OIL));
}

/*
        Fatal error routine
*/
VOID PASCAL bug(SHORT n)
{
    printf("Fatal error number %d\n", n);
    exit(1);
}
