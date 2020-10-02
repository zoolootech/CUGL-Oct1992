/*
TITLE:		Karel The Robot;
VERSION:	1.0;
DATE:		09/27/1984;
DESCRIPTION:
	"Part 2 of source code for syntax-directed editor,
	 KED. Other functions.";
KEYWORDS:	Editor, syntax;
SYSTEM:		CP/M 2+;
FILENAME:	KED2.C
AUTHORS:	Linda Rising;
COMPILERS:	BDS C;
REFERENCES:
	AUTHOR: Richard E. Pattis;
	TITLE: "KAREL the Robot: A Gentle Introduction
		to the Art of Programming";
	CITATION: "";
ENDREF
*/


#include <bdscio.h>
#include <ked.h>


readin(w)   /* find location of command in comm array or symbol table */
char w[];
{
          int c,i,l,m;
 
          do {
            inputs(w);
            if ((m = match(w,comm,MAXCOMM)) != NOMATCH)
               return m;              
            if (m == NOMATCH && last != first && p != pr) {
               l = pr->lastin;
               if (l == 15) 
                   l = last;
               for (i = first; i != l; i = new[i].nextin)
                    if ((c = strcomp(w,new[i].defname)) == 0) 
                       return ( -i - 1);
            }          
            printf("INVALID COMMAND. TYPE MN TO SEE MENU.\n?");
          } while (m == NOMATCH);
}



menu()           /* print updated menu */
{
          int i;

          printf("THE FOLLOWING IS A LIST OF VALID ");
          printf("COMMANDS FOR THIS EDITOR.\n\n");
          printf("  INSTRUCTIONS\n");   
          printf("m          move\n");
          printf("tl         turnleft\n");
          printf("pi         pickbeeper\n");
          printf("pu         putbeeper\n");
          printf("to         turnoff\n");
          printf("b          BEGIN\n");
          printf("bx         BEGINNING-OF-EXECUTION\n");
          printf("nd         END\n");
          printf("ndc        end construct\n");
          printf("ndx        END-OF-EXECUTION\n");
          printf("if         IF\n");
          printf("el         ELSE\n");
          printf("it         ITERATE\n");
          printf("wh         WHILE\n");
          printf("df         DEFINE-NEW-INSTRUCTION\n");
	  printf("\nTYPE ANY KEY TO CONTINUE.\n");
	  getchar();
          if (last != first) {   /* any valid sym tab entries? */
           printf("\n  USER-DEFINED COMMANDS\n");
           for (i = first; i != last; i = new[i].nextin)
                printf("%s\n", new[i].defname);  
          }
          printf("\n  EDIT COMMANDS\n");      
          printf("n          move pointer to next instruction\n");
          printf("nc         move pointer to next construct\n");
          printf("n-         move pointer to previous instruction\n");
          printf("n--        move pointer back 10 lines\n"); 
          printf("d          delete current instruction\n");
	  printf("c          print current line\n");
          printf("p          print next 20 lines\n");
          printf("i          insert new instruction before current one\n");
          printf("t          move pointer to the first line\n");
          printf("l          list the rest of the program\n");
          printf("s          save the program\n");
          printf("mn         print an updated menu\n");
          printf("q          quit the edit program\n");
	  printf("\nTYPE ANY KEY TO CONTINUE.\n");
	  getchar();
          printf("\n  TESTS\n");
          printf("n          facing-north\n");
          printf("nn         not-facing-north\n");
          printf("s          facing-south\n");
          printf("ns         not-facing-south\n");
          printf("e          facing-east\n");
          printf("ne         not-facing-east\n");
          printf("w          facing-west\n");
          printf("nw         not-facing-west\n");
          printf("c          next-to-a-beeper (beeper on corner)\n");
          printf("nc         not-next-to-a-beeper\n");
          printf("b          any-beepers-in-beeper-bag\n");
          printf("nb         no-beepers-in-beeper bag\n");
          printf("f          front-is-clear\n");
          printf("nf         front-is-blocked\n");
          printf("l          left-is-clear\n");
          printf("nl         left-is-blocked\n");
          printf("r          right-is-clear\n");
          printf("nr         right-is-blocked\n");
}

createtree()                      /* reads information from old file1 */
{
           int i,in,d,fn,fs,b,t,l;
           struct tnode *alloc();

           fscanf(ibuf1, "%d %d %d %d %d %d %d %d",
                  &i,&in,&d,&fn,&fs,&b,&t,&l);
           pr = NULL;
           p = alloc(24);                 
           tree(i,in,d,fn,fs,b,t,l);              /* store first node */
           root = p;
           p = root->sub = alloc(24);
           pr = root;
           fscanf(ibuf1, "%d %d %d %d %d %d %d %d",
                  &i,&in,&d,&fn,&fs,&b,&t,&l);
           while (i != NDX && i != CPMEOF && i != EOF) { 
               createnode(ibuf1,i,in,d,fn,fs,b,t,l);    
               fscanf(ibuf1, "%d %d %d %d %d %d %d %d",
                      &i,&in,&d,&fn,&fs,&b,&t,&l);
           }
           endcons();
           tree(i,in,d,fn,fs,b,t,l);
           pr = p;
           fclose(ibuf1);        
}


createnode(ibuf1,i,in,d,fn,fs,b,t,l)    /* restores nodes for parse tree */
FILE *ibuf1;
int i,in,d,fn,fs,b,t,l;
{
          int j;
          char w[MAXLINE];
          struct tnode *allocp();

          if (d == NEWINSTR && pr->instr != DEF) {
              i = reloc[i];                   /* set up array to convert */
              j = 0;                              /* old symb tab to new */
              while (new[i].loc[j] != NULL)
                     j++;
              new[i].loc[j] = p;
              new [i].loc[++j] = NULL;
	      if (l != 15)
		 l = last;
              tree(i,in,d,fn,fs,b,t,l);
              p = p->next = allocp();
          } else switch (i) { 
                 case 0:            /* move */
                 case 1:            /* turnleft */
                 case 2:            /* pickbeeper */
                 case 3:            /* putbeeper */
                 case 4:            /* turnoff */
                    while (in < pr->indent)
                           endcons();
		    if (l != 15)
		       l = last;
                    tree(i,in,d,fn,fs,b,t,l);
                    p = p->next = allocp();
                    break;
                case 5:             /* BEGIN */
                case 7:             /* BEGINNING-OF-EXECUTION */
                case 13:            /* ELSE */
		   if (l != 15)
		      l = last;
                   tree(i,in,d,fn,fs,b,t,l);
                   p = p->sub = allocp();
                   break;
               case 8:              /* END */
		  if (l != 15)
		     l = last;
                  tree(i,in,d,fn,fs,b,t,l);
                  p = p->next = allocp();
                  endcons();
                  break;
               case 11:             /* IF */
                  while (in < pr->indent)
                      endcons();
		  if (l != 15)
		     l = last;
                  tree(i,in,d,fn,fs,b,t,l);
                  p = p->sub = allocp();
                  fscanf(ibuf1,"%d %d %d %d %d %d %d %d",
                         &i,&in,&d,&fn,&fs,&b,&t,&l);         /* get test */
		  if (l != 15)
		     l = last;
                  tree(i,in,d,fn,fs,b,t,l);
                  p = p->sub = allocp();
                  fscanf(ibuf1, "%d %d %d %d %d %d %d %d",
                         &i,&in,&d,&fn,&fs,&b,&t,&l);         /* get THEN */
		  if (l != 15)
		     l = last;
                  tree(i,in,d,fn,fs,b,t,l);
                  p = p->sub = allocp();
                  break;
               case 14:               /* ITERATE */
               case 15:               /* WHILE */
                  while (in < pr->indent)
                      endcons();
		  if (l != 15)
		     l = last;
                  tree(i,in,d,fn,fs,b,t,l);
                  p = p->sub = allocp();
                  fscanf(ibuf1,"%d %d %d %d %d %d %d %d",
                         &i,&in,&d,&fn,&fs,&b,&t,&l);      /* get test or */
		  if (l != 15)    			       /* pos int */
		     l = last;
                  tree(i,in,d,fn,fs,b,t,l);                   
                  p = p->sub = allocp();
                  break;
               case 16:             /* DEFINE-NEW-INSTRUCTION */
                  tree(i,in,d,fn,fs,b,t,last);
                  p = p->sub = allocp();
                  fscanf(ibuf1,"%d %d %d %d %d %d %d %d",
                         &i,&in,&d,&fn,&fs,&b,&t,&l);      /* get new name */
                  i = reloc[i] = last;                        /* translate */
                  j = 0;                              /* sym tab subscript */
                  while ((w[j++] = getc(ibuf1)) != '\n')
                      ;
                  j -= 3;
                  w[j] = '\0';
                  strcpy(new[i].defname,w);
                  new[i].nextin = last + 1; /* store next ref in sym table */
                  tree(i,in,d,fn,fs,b,t,last);
                  p = p->sub = allocp();
                  break;
               }
}   


insertinstr()     /* prepare to add instructions in insert mode */
{
	
          struct tnode *allocp(), *conbegin();

          insert = 1;
          insertnode = curr;
          if ((curr->instr == NDX ||        /* find previous construct */
               curr->instr == ND) && curr->del !=  NEWINSTR) {
	      p = curr->prev;
	      if (p->cons != 1)
                 while (p->indent > curr->indent + 1)
                     p = conbegin(p);
          } else p = curr->prec;
	  if (p->instr == IFF) {     /* set THEN as start of construct */
	     p = advance(p);                /* so ELSE can be inserted */
	     p = advance(p);
	  }
	  ins = p->indent;	   /* final indent level for construct */
	  if ((p->instr == BX || p->instr == BEG || p->instr == BEPGM)
	       && p->del != NEWINSTR)
	       ++ins;
          if (insertnode == p->sub ||
 	     (p->instr == BX && p->del != NEWINSTR)) 
              p = p->sub = allocp();           /* set up links to next */
          else {                                   /* node to be added */
               p = p->next = allocp();
	       temp = insertnode->prev;  
               temp->next = p;
          }
}


endinsert()        /* exit insert mode */
{
          if (insert) {
              if (pr->instr == THEN) 
		 if (p == pr->sub)
		    printf("PLEASE COMPLETE CONSTRUCT\n");
	         else endcons();
              if (pr->indent > ins)
                 printf("PLEASE COMPLETE CONSTRUCT\n");
              else {
                 insert = 0;
                 insertnode->prec = pr;
                 if (tempend != NULL) {            /* if a node has been */
                     tempend->next = insertnode;   /* inserted link last */
                     insertnode->prev = tempend;          /* instruction */
                     tempend = NULL;
                 } else if (temp != NULL)           /* otherwise restore */
                            temp->next = insertnode;        /* old links */
                        else if (p == pr->next)
                                 pr->next = insertnode;
                             else pr->sub = insertnode;
                 temp = NULL;  
                 free(p);
                 p = pr;
		 curr = insertnode;
              }
         }
} 


delete()                /* delete or replace current instruction */
{
          int c;

          switch(curr->del) {
          case 0:       /* reserved word */
             printf("DELETE ENTIRE CONSTRUCT? (Y/N)\n");
             if ((c = toupper(getchar())) == 'Y')
                  deletecon();
             else printf("\n");
             break;
          case 1:       /* simple instruction */
             dealloc(curr,curr->next);
             break;
          case 2:       /* test */
             prompt("TYPE REPLACEMENT ",1,"",5,0);
             curr->instr = getest();
             print(curr->prev,CONSOLE);
             print(curr,CONSOLE);
             break;
          case 3:       /* positive number */
             prompt("TYPE REPLACEMENT ",2,"",16,0);
             curr->instr = getint();
             print(curr->prev,CONSOLE);
             print(curr,CONSOLE); 
             break;
          case 4:       /* new name */
             if (curr->prev->instr == DEF) {
                 prompt("TYPE REPLACEMENT ",4,"",9,0);
                 getnew(curr->instr);
                 print(curr->prev,CONSOLE);
                 print(curr,CONSOLE); 
             } else dealloc(curr,curr->next);
             break;
          case 5:       /* BEGINNING-OF-PROGRAM, BX, NDX */
             printf("CAN'T BE DELETED\n?");
             break;
      }
}


deletecon()       /* locate start and end of current construct */
{

          struct tnode *ptr1, *ptr2, *conbegin(), *conend();
 
          if (curr->instr == THEN || curr->cons != 1)
              ptr1 = conbegin(curr);       /* locate start of construct */
          else ptr1 = curr; 
          if (ptr1->instr == DEF)          /* delete all references for */
              deletenewin(ptr1->sub->instr);           /* defined instr */
          ptr2 = conend(ptr1);               /* locate end of construct */
          dealloc(ptr1,ptr2);
}


struct tnode *conbegin(ptr)       /* locates start of current construct */
struct tnode *ptr;
{
          int in;

          if (ptr->instr == ND && ptr->del != NEWINSTR) {
              in = ptr->indent;
              while ((ptr = ptr->prev)->indent > in)
                       ;
          }
          while ((ptr = ptr->prev)->cons != 1 || ptr->instr == THEN)
             ;   
          return ptr;
}


struct tnode *conend(ptr)        /* locates end of current construct */
struct tnode *ptr;
{

          int d,in;
          struct tnode *advance();

          in = ptr->indent;
	  if (ptr != endof)
             ptr = advance(ptr);
          while (ptr != endof && (ptr->indent > in ||
                 (d = ptr->del) == TEST || d == POSINT || d == NEWINSTR)) 
              ptr = advance(ptr);
          if (ptr == endof && (ptr->indent > in ||
              (d = ptr->del) == TEST || d == POSINT || d == NEWINSTR))
              ptr = p;
          return ptr; 
 
}


deletenewin(i)                /* removes user-defined instruction */
int i;
{

          int j;
          struct tnode *ptr;

          j = 0;
          while ((ptr = new[i].loc[j]) != NULL) { 
              dealloc(ptr,ptr->next);        /* locate and delete */
              j++;                                 /* uses in pgm */
          }
          j = first;
          if (j != i) {
             while (new[j].nextin != i)             /* remove sym */
                 j = new[j].nextin ;          /* table references */
             new[j].nextin = new[i].nextin;
          } else if (new[first].nextin == last)
                     --last;
                 else first = new[i].nextin;
}
 


dealloc(ptr1,ptr2)                 /* free space from ptr1 up to ptr2 */
struct tnode *ptr1,*ptr2;
{

          struct tnode *ptr;
      
          printf("\n");
          if (ptr2->instr != NDX || ptr2->del == NEWINSTR)
              ptr2->prec = ptr1->prec;        /* set up links between */
          curr = ptr2;                        /* ptr2 & ptr1's parent */
          ptr = ptr2->prev = ptr1->prev;
          if (ptr1 == ptr->sub &&
	     (ptr2->instr != NDX || ptr2->del == NEWINSTR))
              ptr->sub = ptr2;
          else { 
             ptr->next = ptr2;
             if (ptr2->instr == NDX && ptr2->del != NEWINSTR)
                 ptr->sub = NULL;
          }
          while (ptr1 != ptr2) {               /* deallocate nodes in */
              ptr = ptr1;                     /* succession and print */
              if (ptr1->sub != NULL)
                  ptr1 = ptr1->sub;
              else if (ptr1->next != NULL)
                       ptr1 = ptr1->next;
              print(ptr,CONSOLE);
              free(ptr);  
          }
          if (ptr2 == p && p != pr) {              /* if last line of */
              endof = temp = curr = ptr1->prev;   /* pgm, set new eof */ 
              pr = ptr1->prec;
              temp->next = p;
          }       
}


putintable(i)     /* add new name to symbol table */
int i;
{
          int j,k;

          if (insert && insertnode->instr == DEF) {
              j = insertnode->sub->instr;         /* if inserting before */
              new[i].nextin = j;                    /* DEF locate proper */
              for (k = first; new[k].nextin != last;)     /* position in */
                   k = new[k].nextin;                     /* linked list */
              new[k].nextin = last + 1;
              if (j == first)
                  first = i;
              else {
                 for (k = first; new[k].nextin != j;)
                      k = new[k].nextin;
                 new[k].nextin = i;
             }                            /* otherwise add as last entry */
          } else new[i].nextin = last + 1;
}   


savetree(p,argv)         /* write parse tree to file1, text to file2 */
struct tnode *p;
char **argv;
{
          struct tnode *advance(); 

          fcreat(argv[1],ibuf1);         /* open both files */  
          fcreat(argv[2],ibuf2);         /* for writing */
          while (p != endof) {
              fprintf(ibuf1, "%d %d %d %d %d %d %d %d \n",   /*codes */
                    p->instr,p->indent,p->del,p->nfollow,/* in file1 */
                    p->sfollow,p->comp,p->cons,p->lastin);
              print(p,ibuf2);                       /* text in file2 */
              if (p->instr == DEF && p->del == 0) {
                  p = advance(p);
                  fprintf(ibuf1,"%d %d %d %d %d %d %d %d \n",
                          p->instr,p->indent,p->del,p->nfollow,
                          p->sfollow,p->comp,p->cons,p->lastin);
                  print(p,ibuf2);
                  fprintf(ibuf1,"%s \n",new[p->instr].defname);
              }             
              p = advance(p);      
           } 
           fprintf(ibuf1, "%d %d %d %d %d %d %d %d \n",
                   9,1,5,4,4,0,2,10);
           print(p,ibuf2);
           putc(CPMEOF,ibuf1);    putc(CPMEOF,ibuf2);
           fclose(ibuf1);         fclose(ibuf2);
         
}



tree(i,in,d,fn,fs,b,t,l)    /* store instruction codes in parse tree */
int i,in,d,fn,fs,b,t,l;
{
         p->sub = NULL;
         p->next = NULL;
         p->prec = pr;
         if (pr->sub != NULL && p == pr->next) {
             p->prev = temp;                       /* set up link to */
             temp = NULL;                    /* previous instruction */
         } else p->prev = pr;
         p->instr = i;
         p->indent = in;
         p->del = d;
         p->nfollow = fn;
         p->sfollow = fs;
         p->comp = b;
         p->cons = t;
         p->lastin = l;
         if (insert)
             tempend = p;     /* last node inserted */
         else endof = p;      /* last node added (eof) */
}


struct tnode *allocp()      /* get space for next node */
{
          struct tnode *alloc(), *ptr;

          ptr = alloc(24);
          curr = p;
          pr = p;
          return ptr;
}


inputs(w)         /* read in next command */
char w[];
{
          int i,m;
	  char c;

	  m = 0;
          i = 0;
          while ((c = w[i++] = tolower(getchar())) != '\n')
              if ((c > 'z' || c < 'a') && (c > '9' || c < '0') && c != '-') 
		m = ERROR;
          w[--i] = '\0';
	  return m;
}


match(w,s,max)          /* compares string to array element */
char w[], *s[];
int max;
{
          int c,i;

          for (i = 0; i < max; i++) {
            c = strcomp(w,s[i]);
            if (c == 0)                 /* if match, return */
                return i;                /* array subscript */
          }
          return NOMATCH;            /* else return NOMATCH */
}


prompt(mess1,num1,mess2,num2,i)       /* prompt for next instruction */
char *mess1,*mess2;
int num1, num2,i;
{
          int j;

          printf("%s",mess1);                  /* print first string */
          switch(num1) {           /* print desired instruction type */  
          case 1:
             printf("<test>");
             break;
          case 2:
             printf("<positive-number>");
             break;
          case 3:
             for (j = 0; j < curr->indent;
                         printf("  "), j++);
	     if (i == ND || i == NDC) {
		printf("\b");
		printf("\b");
	     }
             printf("<instruction>");
             break;
          case 4:
             printf("<new-name>");
             break;
          }
          printf("%s",mess2);            /* print second string */
          for (; num2-- > 0; printf("\b"))
            ;
}


struct tnode *advance(ptr)         /* point to next line or eof */
struct tnode *ptr;
{
          if (ptr->sub != NULL)
              ptr = ptr->sub;
          else if (ptr->next != NULL)
                   ptr = ptr->next;
          return ptr;
}


checkin(valid)   /* correct location for an instruction? */
int *valid;
{
          int in;

          *valid = 1;
          if (p == pr && !insert) {  /* if NDX added, must insert */
              printf("INSERT ONLY\n?");
              *valid = 0;
          } else { 
               if (pr->instr == THEN && p!= pr->sub) {
                   endcons();               /* end THEN construct */
		   if (insert)            /* since ELSE not added */
		      ins = pr->indent;
	       }
               if (pr->nfollow == 0 &&           
                          p == pr->next)
                   in = pr->indent;         
               else if (pr->sfollow == 0 && 
                               p == pr->sub)
                        in = pr->indent + 1;
                     else { 
                        *valid = 0;
                        printf("BAD LOCATION FOR COMMAND. ");
                        printf("TYPE MN TO SEE MENU.\n?");
                     }
            }           
          return in;
}


checkcomp(valid)         /* compound statement? */
int *valid;
{
          int b;

          if (pr->comp == 2) {       /* if 2nd instr added without BEGIN */
              *valid = 0;
              printf("STRUCTURE NOT A COMPOUND STATEMENT\n?");
          } else if (pr->comp == 1)  /* if 1st instr added without BEGIN */
                     b = 2;            /* set warning for future entries */
                 else b = pr->comp;
          return b;
}



endcons()         /* end construct */
{  
          int in;
          struct tnode *alloc();
  
          if (pr->indent == 1)     /* only BX, NDX indented 1 level */
              printf("LAST POSSIBLE LEVEL\n");   
          else {
             if (temp->next != p     /* keep last instr in previous */
		 || temp == NULL)               
  		 temp = pr;            /* construct if NDC repeated */
             do {
              in = pr->indent;
              while ((pr = pr->prev)->cons != 1 /* go back to start */
                      || pr->indent >= in);         /* of construct */
             } while (pr->instr == ELS);               /* skip ELSE */
             if (pr->instr == DEF)          /* if DEF ended add new */
                 last++;                         /* name to sym tab */
             pr->next = alloc(24);
             free(p);
             p = temp->next = pr->next;
          } 
}    


strcomp(s1,s2)
char s1[],s2[];
{
	  int i;

	  i = 0;
	  while (s1[i] == s2[i])
	     if (s1[i++] == '\0' || i == 8)
		return 0;
	  return s1[i] - s2[i];
}