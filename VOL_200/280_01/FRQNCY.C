/* [FRQNCY.C of JUGPDS Vol.46] */
/*
*****************************************************************
*								*
*	Written by  Hakuo Katayose (JUG-CP/M No.179)		*
*		    49-114 Kawauchi-Sanjuunin-machi		*
*		    Sendai, Miyagi 980                          *
*		    Phone: 0222-61-3219				*
*								*
*       Modifird by Toshiya Oota   (JUG-CPM No.10)              *
*                   Sakae ko-po 205 				*
*		    5-19-6 Hosoda				*
*		    Katusikaku Tokyo 124			*
*								*
*		for MS-DOS Lattice C V3.1J & 80186/V20/V30	*
*								*
*	Compiler Option: -ccu -k0(1) -ms -n -v -w		*
*								*
*	Edited & tested by Y. Monma (JUG-CP/M Disk Editor)	*
*			&  T. Ota   (JUG-CP/M Sub Disk Editor)	*
*								*
*****************************************************************
*/

/* term - produce word frequency list for a text
            K&P Exercise 4-24 in p.126 */

#include "stdio.h"
#include "dos.h"
#include "stdlib.h"
#include "tools.h"
#include "toolfunc.h"

#define MAXWORD 64

void treeprint();

struct tnode {
	char  *word;
	int   count;
	struct tnode *left;
	struct tnode *right;
};

char	opt_f;		/* fold order	    */

void main(argc, argv)
int	argc;
char *argv[];
{
struct tnode *root, *tree();
char word[MAXWORD], *ap;
int  t;
    if (argc > 2)
	error("FRQ999 Usage: frqncy [-f] <infile >outfile");
    else
	opt_f = OFF;
	while (--argc > 0) {
	    ap = *++argv;
	    if (*ap++ == '-')
		if (tolower(*ap) == 'f')
		    opt_f = ON;
		else
		    error("FRQ999 Usage: frqncy [-f] <infile >outfile");
	}
    root = NULL;
    while ((t = getword(word, MAXWORD)) != EOF) {
	if (t == LETTER) {
	    if ((root = tree(root, word)) == NULL)
		exit(puts("FRQ901 alloc overflow."));
	    }
    }
    treeprint(root);
}

struct tnode *tree(p, w)
struct tnode *p;
char *w;
{
struct tnode *talloc();
char *strsave();
int  cond;
    if (p == NULL)
	if (((p = talloc()) == NULL) || (p->word = strsave(w)) == NULL)
	    return NULL;
	else {
	    p->count = 1;
	    p->left = p->right = NULL;
	}
	else if ((cond = (opt_f == ON) ? strfcmp(w, p->word)
		 : strcmp(w, p->word)) == 0)
	    p->count++;
	else if (cond < 0)
	    p->left = tree(p->left, w);
	else
	    p->right = tree(p->right, w);
	return(p);
}

void treeprint(p)
struct tnode *p;
{
    if (p != NULL) {
	treeprint(p->left);
	printf("%5d: %s\n", p->count, p->word);
	treeprint(p->right);
    }
}


struct tnode *talloc()
{
    return( (struct tnode *) malloc(sizeof(struct  tnode)) );
}
