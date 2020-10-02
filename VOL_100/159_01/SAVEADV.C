
 /* SAVEADV.C  no mods for V 1.43 
  *
  * Modified calls of fcreat() to call fopen() with a mode setting of write.
  * Modified close() call to use fclose() and removed call to fflush().
  * Cahnged "savefd" from and int to pointer to FILE. -- Eco-C88 V2.72 -- BW
  */

#include "advent.h"

VOID PASCAL saveadv(VOID)
{
    auto     char	*sptr;
    auto     FILE	*savefd;
    auto     char	 username[14];

    printf("What do you want to call the saved game? ");
    scanf("%8s", username);
    for (sptr = username; *sptr; sptr++)
    {
	*sptr = (char) toupper(*sptr);
	if (!isprint(*sptr))
	{
	    *sptr = '\0';
	    break;
	}
    }

    if (strlen(username) == 0)
	strcpy(username, "SAVE");
    strcat(username, ".ADV");
    printf("\nOpening save file \"%s\".\n", username);
    if ((savefd = fopen(username, WRITE_BIN)) == 0)
    {
	printf("Sorry, I can't open the file...\n");
	return;
    }

    for (sptr = (char *) &turns; sptr < (char *) &lastglob; sptr++)
    {
	if (putc(*sptr, savefd) == -1)
	{
	    printf("write error on save file...\n");
	    return;
	}
    }

    if (fclose(savefd) == -1)
    {
	printf("can't close save file...\n");
	return;
    }
    printf("Game saved.\n\n");
    return;
}
