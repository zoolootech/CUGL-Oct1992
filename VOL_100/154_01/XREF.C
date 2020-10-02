/*
 *   xref.c:    word-to-line cross-reference -
 *     usage:      xref [file]  (defaults to standard input)
 *     note:       A "word" is defined by the macro "okchar(c)" 
 *
 *   (c) Chuck Allison, 1985
 *
 */

#include <stdio.h>
#include <ctype.h>

#ifdef MICROSOFT

#include <malloc.h>

#define		getmem		malloc
#define		chk_ferror	fprintf

#endif

#define okchar(c)  (isalnum(c) || c == '-' || c == '\'')
#define MAXLINE 512

/* ..Linked-list structure for each list of line numbers.. */
struct list
{
    struct list *next;
    int lnum;
} *addline();

/* ..Node structure for tree of distinct words.. */
struct tree
{
    char *word;
    struct tree *left,*right;
    struct list *lptr;       /* -> to list of line #'s for this word */
} *words = NULL, *node, *addword(), *find_node();

char *lineptr, *getword(), *getmem();
int  distinct = 0;


main(argc,argv)
int argc;
char *argv[];
{
    register lineno = 0;
    static char linebuf[MAXLINE], wordbuf[MAXLINE];
    
    /* ..Get optional filename.. */
    if (argc > 1)
        if (freopen(argv[1],"r",stdin) == NULL)
        {
            printf("---> ERROR: can't open %s\n",argv[1]);
            exit(1);
        }
        else
            fputs("reading file ...\n",stderr);
    else
            fputs("Enter data:\n\n",stderr);

    /* ..Process each line of text file.. */
    while (fgets(linebuf,MAXLINE,stdin))
    {
        ++lineno;                  /* ..we just read another line.. */
        lineptr = linebuf;         /* ..point to start of line.. */
        while (getword(wordbuf))
        {
            /* ..add to tree.. */
            words = addword(words,wordbuf);

            /* ..get list header for this word's line numbers.. */
            node = find_node(words,wordbuf);

            /* ..add this line to list of line numbers.. */
            node->lptr = addline(node->lptr,lineno);
        }
    }

    fprintf(stderr,"No. of distinct words: %d\n\n",distinct);
    print_tree(words);
}


char *getword(s)          /* ..get next word from line buffer.. */
char *s;
{
    register char *wordptr = s;

    /* ..Ignore unwanted characters.. */
    while (*lineptr && !okchar(*lineptr))
        ++lineptr;

    /* ..Build word.. */
    while (okchar(*lineptr))
	{
        *wordptr++ = tolower(*lineptr);
	lineptr++;
	};
    *wordptr = '\0';
    return strlen(s) ? s : NULL;
}

struct tree *addword(t,word)     /* ..add word to list (if new).. */
struct tree *t;
char *word;
{
    if (t == NULL)
    {
        /* ..new entry.. */
        ++distinct;
        t = (struct tree *) getmem(sizeof(struct tree));
        t->word = (char *) getmem((strlen(word)+1)*sizeof(char));
        strcpy(t->word,word);
        t->left = t->right = (struct tree *) NULL;
        t->lptr = (struct list *) NULL;
    }
    else if (strcmp(t->word,word) < 0)
        t->right = addword(t->right,word);
    else if (strcmp(t->word,word) > 0)
        t->left = addword(t->left,word);

    return t;
}

struct list *addline(p,n)       /* ..add line # for current word.. */
struct list *p;
int n;
{
    struct list *q;
    
    if (p == NULL)
    {
        /* ..insert at tail.. */
        q = (struct list *) getmem(sizeof(struct list));
        q->lnum = n;
        q->next = (struct list *) NULL;
        return q;
    }
    else if (p->lnum != n)
        /* ..keep looking.. */
        p->next = addline(p->next,n);

    return p;
}
 
print_tree(t)
struct tree *t;
{
    if (t != NULL)
    {
        print_tree(t->left);
        printf("%-20.20s: ",t->word);
        chk_ferror(stdout,"standard output");
        print_list(t->lptr); putchar('\n');
        print_tree(t->right);
    }
}

print_list(p)
struct list *p;
{
    register count;
    
    for (count = 0; p != NULL; p = p->next, ++count)
    {
        printf("%5d",p->lnum);
        if ((count+1)%10 == 0 && p->next != NULL)
            printf("\n%22c",' ');
    }
}

struct tree *find_node(p,s)
struct tree *p;
char *s;
{
    if (strcmp(p->word,s) > 0)
        return find_node(p->left,s);
    else if (strcmp(p->word,s) < 0)
        return find_node(p->right,s);
    else
        return p;       /* .."s" is guaranteed to be found.. */
}
