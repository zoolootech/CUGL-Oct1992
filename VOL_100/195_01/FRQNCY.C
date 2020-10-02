/* [FRQNCY.C of JUGPDS Vol.18]
*****************************************************************
*								*
*	Written by  Hakuo Katayose (JUG-CP/M No.179)		*
*		    49-114 Kawauchi-Sanjuunin-machi		*
*		    Sendai, Miyagi 980                          *
*		    Phone: 0222-61-3219				*
*								*
*	Edited & tested by Y. Monma (JUG-C/M Disk Editor)       * 
*								*
*****************************************************************
*/

/* term - produce word frequency list for a text
            K&P Exercise 4-24 in p.126 */

#include "stdio.h"
#include "def.h"
#include <dio.h>

#define MAXWORD 64

struct tnode {
	char  *word;
	int   count;
	struct tnode *left;
	struct tnode *right;
};

char	opt_f;		/* fold order	    */


main(argc, argv)
int	argc;
char *argv[];

{
	struct tnode *root, *tree();
	char word[MAXWORD], *ap;
	int  t;

	dioinit(&argc, argv);
	if (argc < 2) {
		error("Usage: frqncy <infile >outfile ^Z");
		exit();
		}
	else
	opt_f = OFF;
	while (--argc > 0) {
		ap = *++argv;
		if (*ap++ == '-')
			if (tolower(*ap) == 'f')
				opt_f = ON;
		}
	root = NULL;
	while ((t = getword(word, MAXWORD)) != EOF) {
		if (t == LETTER) {
			if ((root = tree(root, word)) == NULL)
				exit(puts("alloc overflow."));
			}
		}
	treeprint(root);
	dioflush();
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


treeprint(p)
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
	char *alloc();
	struct tnode *p;

	return(p = alloc(sizeof(*p)));
}
