#if __STDC__
#define _Cdecl
#else
#define _Cdecl  cdecl
#endif

struct menu_struc   {
	int nsels;
	int attr;
	char *menu_string;
	int rattr;
    int icol[15];
	int ilen[15];
	int selected;
	int p_sel;
};

struct pull_down {
	int npulls;
	int mlen;
	char *pull_items;
};


int _Cdecl  menubar(struct menu_struc *,struct pull_down *);
int _Cdecl  do_pull_down(struct pull_down *,struct menu_struc *);
