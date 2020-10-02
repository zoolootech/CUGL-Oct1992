/************************************************
 *	Definitions for Menu Manager		*
 ************************************************/

struct menu {
	char *question;
	struct mline {
		char *answer;
		int (*routine)();
		int *parameter;
	}
	selection[1];
};

#define MENU	struct menu
#define MLINE	struct mline

#define MAXMDEPTH 8
extern MENU m;
extern hist[MAXMDEPTH], *hptr;


menu {
	char *question;
	struct mline {
		char *answer;
		int (*routine)();
		int *parameter;
	}
	sel