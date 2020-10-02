
 /* SAVEADV.C  no mods for V 1.43 
 */
#include "advent.h"

main()
{
	/*
		save adventure game
	*/
	char *sptr;
	int savefd;
	char username[13];

	printf("What do you want to call the saved game? ");
	scanf("%s",username);
	strcat(username,".adv");
	if ((savefd=fcreat(username,dbuff)) == -1) {
		printf("sorry, I can't create the file...\n");
		exit();
	}
	for (sptr=&turns; sptr<&lastglob; sptr++) {
		if (putc(*sptr,dbuff) == -1) {
			printf("write error on save file...\n");
			exit();
		}
	}
	if (fflush(dbuff) == -1) {
		printf("flush error on save file...\n");
		exit();
	}
	if (close(savefd) == -1) {
		printf("can't close save file...\n");
		exit();
	}
	printf("OK -- \"C\" you later...\n");
}
