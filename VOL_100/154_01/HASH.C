/* hash.c:	create and display an open hash table */

#include <stdio.h>
#define MAXSTR 31
#define PRIME 23

struct element {
	struct element *next;
	char *data;
} *hashtab[PRIME];

main()
{	int i;
	char s[MAXSTR];

	for (i = 0; i < PRIME; ++i)		/* ..initialize bucket.. */
		hashtab[i] = NULL;
	while (gets(s) != NULL)			/* ..create table.. */
		if (search(s) == 0) insert(s);	
	for (i = 0; i < PRIME; ++i) {		/* ..print it out.. */
		printf("%3d: -> ",i);
		print_list(hashtab[i]);
		putchar('\n');
	}
}

print_list(p)
struct element *p;
{	for ( ; p != NULL; p = p->next)
		printf("%s -> ",p->data);
}

int hashval(s)
char *s;
{	int sum;
	
	for (sum = 0; *s; sum += *s++) ;
	return sum % PRIME;
}

insert(s)
char *s;
{	struct element *p;
	int h;

	p = (struct element *) malloc(sizeof(struct element));
	p->next = hashtab[h = hashval(s)];
	hashtab[h] = p;			/* insert at top (why not?) */
	p->data = (char *) malloc(strlen(s)+1);
	strcpy(p->data,s);
}

int search(s)
char *s;
{	struct element *p;

	for(p = hashtab[hashval(s)]; p != NULL; p = p->next)
		if (strcmp(p->data,s) == 0) return 1;
	return 0;
}
