/*
TITLE:		Karel The Robot;
VERSION:	1.0;
DATE:		09/27/1984;
DESCRIPTION:
	"Part 1 of source code for syntax-directed 
	 editor KED.  Used for building programs to
	 be executed in KAREL's world.";
KEYWORDS:	Syntax, editor;
SYSTEM:		CP/M 2+;
FILENAME:	KED.C;
AUTHORS:	Linda Rising;
COMPILERS:	BDS C;
REFERENCES:
	AUTHOR: Richard E. Pattis;
	TITLE: "KAREL the Robot: A Gentle Introduction
		to the Art of Programming";
	CITATION: "";
ENDREF
*/


#include <bdscio.h>                    /* standard library header */
#include <ked.h>                            /* globals for editor */


main(argc,argv)
int argc;                         /* number of arguments for main */           char **argv;                                        /* arg vector */
{
           int i;                             /* next instruction */
           int stop;                        /* boolean to end pgm */
           char nextcomm[MAXLINE];       /* buffer for next instr */
           struct tnode *alloc();            /* allocate function */
           
           stop = 0;                        /* not end of session */ 
           insert = 0;                         /* not insert mode */
           first = last = 0;        /* no entries in symbol table */
           tempend = NULL;                  /* last node inserted */
           temp = NULL;                /* last instr in construct */

           if (argc < 3)               /* check for two filenames */
               printf("MISSING FILENAME(S)\n");
           else { 
              initinstr();       /* initialize arrays of commands */
              if (fopen(argv[1],ibuf1) == ERROR) {
                  p = alloc(24);             /* create first node */
                  pr = NULL;
                  tree(6,0,5,1,1,0,1,0);
                  endof = pr = curr = root = p;        
                  p = root->sub = alloc(24); 
                  printf("TYPE MN TO SEE MENU \n");
                  print(root,CONSOLE);
                  prompt("  ",3,"",12,0);
               } else {                            /* if old file */
                    createtree();           /* read in parse tree */
                    curr = root;
                    reply(20);   
                 }
               while (!stop) {
                   i = readin(nextcomm);  /* get the next command */
                   stop = reply(i,argv); /* make correct response */
               }
          }
                  
}



initinstr()    /* initialize arrays of commands */
{
          int i;
          char *alloc();

/* allocate space for array entries */
for (i = 0; i < MAXCOMM; i++)   
     comm[i] = alloc(4);
for (i = 0; i < MAXTEST; i++)
     test[i] = alloc(2);
for (i = 0; i < MAXNAME; i++)
     iname[i] = alloc(10);  

/* initialize editor commands */
comm[0] = "m";            comm[1] = "tl";
comm[2] = "pi";           comm[3] = "pu";
comm[4] = "to";           comm[5] = "b";
comm[6] = "bepg";         comm[7] = "bx";
comm[8] = "nd";           comm[9] = "ndx";
comm[10] = "ndc";         comm[11] = "if";
comm[12] = "th";          comm[13] = "el";
comm[14] = "it";          comm[15] = "wh";
comm[16] = "df";          comm[17] = "n";
comm[18] = "n-";          comm[19] = "d";
comm[20] = "p";           comm[21] = "i";
comm[22] = "t";           comm[23] = "s";
comm[24] = "q";           comm[25] = "mn";
comm[26] = "n--";         comm[27] = "l";
comm[28] = "nc";	  comm[29] = "c";


/* initialize booleans for Karel's programs */
test[0] = "n";            test[1] = "nn";
test[2] = "s";            test[3] = "ns";
test[4] = "e";            test[5] = "ne";
test[6] = "w";            test[7] = "nw";
test[8] = "c";            test[9] = "nc";
test[10] = "b";           test[11] = "nb";
test[12] = "f";           test[13] = "nf";
test[14] = "l";           test[15] = "nl";
test[16] = "r";           test[17] = "nr";
test[18] = "mn";

/* initialize commands for Karel's programs */
iname[0] = "move";        iname[1] = "turnleft";
iname[2] = "pickbeeper";  iname[3] = "putbeeper";
iname[4] = "turnoff";     iname[5] = "begin";
iname[6] = "end";         iname[7] = "if";
iname[8] = "then";        iname[9] = "else";
iname[10] = "iterate";    iname[11] = "while";

}


reply(i,argv)         /* call proper function to handle input */
int i;
char **argv;
{
	  int d,valid;
          struct tnode *advance();    
    
          if (i < 0) {                  /* user-defined instr */
              i = -i - 1;         /* converted to distinguish */
              defindst(i);             /* from other commands */
              return 0;
          }
          else {
             switch(i) {
             case 0:      /* move */
             case 1:      /* turnleft */
             case 2:      /* pickbeeper */
             case 3:      /* putbeeper */
             case 4:      /* turnoff */
                commandst(i);         
                return 0;
             case 5:      /* BEGIN */
                compoundst(i);
                return 0;
             case 7:      /* BEGINNING-OF-EXECUTION */
                if (insert)
                    printf("COMMAND CAN'T BE INSERTED\n?"); 
                else execst(i);
                return 0;
             case 8:      /* END */
		if (pr->instr == THEN) 
		   endcons();
		if (insert && insertnode->instr == ND &&
		    insertnode->del != NEWINSTR &&
		   (pr->indent - 1) == (insertnode->indent))
		   printf("END NOT REQUIRED\n?");
		else {
		   valid = endst(i);
		   if (valid) {
                      endcons();
		      prompt("",3,"",12,i);
		   }
		}
                return 0;
             case 9:      /* END-OF-EXECUTION */
                if (insert)
                    printf("COMMAND CAN'T BE INSERTED\n?");
                else {
                   if (pr->instr == THEN)  /* automatically end */
                       endcons();                /* THEN and BX */
                   if (pr->indent == 2)
                       endcons();
                   exendst(i);
                } 
                return 0;
             case 10:      /* end construct */
                if (pr->comp == 3)   /* compound statement */
                    printf("END REQUIRED\n");
                else endcons();
                prompt("",3,"",12,i);    
                return 0;
             case 11:      /* IF */
                ifst(i);
                return 0;
             case 13:      /* ELSE */
                elst(i);
                return 0;
             case 14:      /* ITERATE */
                iterst(i);
                return 0;
             case 15:      /* WHILE */
                whilst(i);
                return 0;
             case 16:      /* DEFINE-NEW-INSTRUCTION */
                defst(i);
                return 0;
             case 17:      /* advance pointer */
                endinsert();
                if (!insert) 
                    if (curr == endof)
                        printf("END OF FILE\n");
                    else {
                       curr = advance(curr);
                       print(curr,CONSOLE);
                    } 
                printf("?");
                return 0;
             case 18:      /* reverse pointer */
                endinsert();
                if (!insert) {
                    if (curr->prev != NULL)
                        curr = curr->prev;
                    print(curr,CONSOLE);
                }
                printf("?");
                return 0;
             case 19:      /* delete */
                endinsert();
                if (!insert) 
                    delete();
                printf("?");
                return 0;
             case 20:      /* print 20 lines */
                endinsert();
                if (!insert) {
                    print(curr,CONSOLE);
                    for (i = 0; i < 20; i++) {
                         if (curr == endof) {
                             printf("END OF FILE\n");
                             break;
                        } else {
                             curr = advance(curr);
                             print(curr,CONSOLE);
                          }
                     }  
                }
                printf("?");
                return 0;
             case 21:      /* insert */
                if (insert)
                    printf("ALREADY IN INSERT MODE\n");
                else if (p != pr)       /* NDX not added */
                         printf("INSERT ONLY AFTER END PGM\n");
                     else if ((curr->indent > 2 && curr->instr !=ND) ||
			      (curr->instr == BEG) ||
			      (d = curr->del) == TEST || d == POSINT || 
			      (d == NEWINSTR && curr->prev->instr == DEF))
                              printf("CAN'T INSERT INSIDE CONSTRUCT\n");
                          else insertinstr();  
                printf("?");
                return 0;
             case 22:      /* go to top of pgm  */
                endinsert();
                if (!insert) {
                    curr = root;  
                    print(curr,CONSOLE);
                } 
                printf("?");
                return 0;
             case 23:      /* save file */
                endinsert();
                if (!insert) {
                    if (p != pr)   /* NDX not added */
                        printf("PLEASE END PROGRAM \n");
                    else savetree(root,argv);
                }
                printf("?");
                return 0;
             case 24:      /* quit edit program */
                endinsert();
                if (!insert) {
                    printf("HAS PROGRAM BEEN SAVED? (Y/N)\n");
                    if ((i = toupper(getchar())) == 'Y') 
                         return 1;
                    else {
                       printf("\n EXIT EDITOR ANYWAY? (Y/N)\n");
                       if ((i = toupper(getchar())) == 'Y') 
                          return 1;
                    }
		}
		printf("\n?");
		return 0;
             case 25:      /* output menu */
                menu();
                printf("?");
                return 0;
             case 26:      /* go back 10 lines */
                endinsert();
                if (!insert) {
                    for (i = 0; i < 10; i++) 
                         if (curr->prev != NULL)
                             curr = curr->prev;
                         print(curr,CONSOLE);
                }
                printf("?");
                return 0;
             case 27:      /* list entire pgm */
                endinsert();
                if (!insert) {
                    print(curr,CONSOLE);
                    while (curr != endof) {
                        curr = advance(curr);
                        print(curr,CONSOLE);
                    } 
                    printf("END OF FILE\n");
                }
                printf("?");
                return 0;
             case 28:      /* advance to next construct */
                endinsert();
                if (!insert) {
		   if (curr != endof)
                      while ((curr = advance(curr))->cons != 1)
                          if (curr == endof)
                             break;
                   print(curr,CONSOLE);
                }
                printf("?");
                return 0;
	     case 29:	  /* print current line */
		endinsert();
		if (!insert)
		   print(curr,CONSOLE);
		return 0; 
            }
        }
}


defindst(i)                 /* user-defined instruction entered */
int i;
{
         int j,in,d,fn,fs,b,t,l,*valid;
         struct tnode *allocp();

         in = checkin(&valid);             /* correct location? */
         b = checkcomp(&valid);          /* compound statement? */
         if (valid) {
             j = 0;                            
             while (new[i].loc[j] != NULL)      /* find loc for */
                if (++j == 15) {           /* next use of instr */
       	           printf("MAXIMUM NUMBER OF USES OF "); 
		   printf("DEFINITION EXCEEDED\n?");
		   return;
	        }
             new[i].loc[j] = p;
             new[i].loc[++j] = NULL;
             d = 4;                      /* line may be deleted */
             fn = 0;                   /* any instr may be next */
             fs = 4;                      /* no component instr */
             t = 0;            /* not start or end of construct */
             l = pr->lastin;        /* last valid sym tab entry */    
             tree(i,in,d,fn,fs,b,t,l);
             p = p->next = allocp();
             print(curr,CONSOLE);
             prompt("",3,"",12,0); 
          }
}


commandst(i)    /* move, turnleft, pickbeeper, putbeeper, turnoff */
int i;
{
          int in,d,fn,fs,b,t,l,*valid;
          struct tnode *allocp();

          in = checkin(&valid);              /* correct location? */
          b = checkcomp(&valid);           /* compound statement? */
          if (valid) {
              d = 1;                       /* line may be deleted */
              fn = 0;                    /* any instr may be next */
              fs = 4;                       /* no component instr */    
              t = 0;             /* not start or end of construct */
              l = pr->lastin;         /* last valid sym tab entry */
              tree(i,in,d,fn,fs,b,t,l);    
              p = p->next = allocp();
              print(curr,CONSOLE);
              prompt("",3,"",12,0);
          }
}


compoundst(i)                                    /* BEGIN entered */
int i;
{
          int in,d,fn,fs,b,t,l;
          struct tnode *allocp();

          switch (pr->comp) {
          case 1:                          /* valid loc for begin */
             in = pr->indent + 1;           /* for pretty printer */
             d = 0;           /* entire construct must be deleted */
             fn = 4;                   /* no valid next construct */
             fs = 0;                      /* any instr may follow */ 
             b = 3;                         /* compound statement */
             t = 0;              /* not start or end of construct */
             l = pr->lastin;          /* last valid sym tab entry */
             tree(i,in,d,fn,fs,b,t,l);
             p = p->sub = allocp();
             print(curr,CONSOLE);
             prompt("  ",3,"",12,0);        
             break;
         case 0:                       /* incorrect loc for BEGIN */
         case 2:
         case 3:                    
            printf("BAD LOCATION FOR BEGIN. TYPE MN TO SEE MENU.\n?");
         }
}


execst(i)                       /* BEGINNING-OF-EXECUTION entered */
int i;
{
          int in,d,fn,fs,b,t,l;
          struct tnode *allocp();

          if (pr->nfollow != 1) {
             printf("BAD LOCATION FOR BEGINNING-OF-EXECUTION. ");
	     printf("TYPE MN TO SEE MENU.\n?");
          }  else {
                in = 1;                     /* for pretty printer */
                d = 5;              /* construct can't be deleted */
                fn = 3;     /* no valid next construct except NDX */
                fs = 0;                   /* any instr may follow */
                b = 0;                /* not a compound statement */
                t = 1;                      /* start of construct */
                l = 15;               /* use any instr in sym tab */
                tree(i,in,d,fn,fs,b,t,l);
                p = p->sub = allocp();
                print(curr,CONSOLE);
                prompt("  ",3,"",12,0);
             }
}


endst(i)                                           /* END entered */
int i;
{
          int in,d,fn,fs,b,t,l,valid;
          struct tnode *allocp();
 
          if (pr->comp != 3) {
              printf("STRUCTURE NOT A COMPOUND STATEMENT\n?");
	      valid = 0;
          } else {
	        valid = 1;
                in = pr->indent - 1;         /* for pretty printer */
                d = 0;         /* entire construct must be deleted */
                fn = fs = 4;                 /* no valid followers */
                b = 0;          /* not start of compound statement */
                t = 2;                         /* end of construct */
                l = pr->lastin;        /* last valid sym tab entry */
                tree(i,in,d,fn,fs,b,t,l);
                p = p->next = allocp();
                print(curr,CONSOLE);
             }
	  return valid;
}


exendst(i)                             /* END-OF-EXECUTION entered */
int i;
{
          int in,d,fn,fs,b,t,l;
 
          if (pr->nfollow != 3)
             printf("BAD LOCATION FOR END PGM. TYPE MN TO SEE MENU.\n?");
          else {
             in = 1;                         /* for pretty printer */
             d = 5;                  /* construct can't be deleted */
             fn = fs = 4;                    /* no valid followers */
             b = 0;             /* not start of compound statement */
             t = 2;                            /* end of construct */
             l = pr->lastin;           /* last valid sym tab entry */
             tree(i,in,d,fn,fs,b,t,l);
             pr = curr = p;
             print(p,CONSOLE);
             printf("?");
          }
}   


ifst(i)                                              /* IF entered */
int i;
{
          int j,in,d,fn,fs,b,t,l,*valid,c;
          struct tnode *allocp();

          in = checkin(&valid);               /* correct location? */
          b = checkcomp(&valid);            /* compound statement? */
          if (valid) {
              d = 0;           /* entire construct must be deleted */
              fn = 0;           /* any instr may be next construct */
              fs = 2;                      /* only test may follow */
              t = 1;                         /* start of construct */
              l = pr->lastin;          /* last valid sym tab entry */
              tree(i,in,d,fn,fs,b,t,l);
              p = p->sub = allocp();
              do {
	       for (j = 0; j < in; j++)
		   printf("  ");
	       prompt("IF ",1," ",6,0);
               c = testin();                           /* get test */
               if (c == MENU) 
                   menu();
              } while (c == 18); 
              print(pr->prev,CONSOLE);
              print(pr,CONSOLE);
              thenin();                                /* add THEN */
              print(pr,CONSOLE); 
              prompt("  ",3,"",12,0);
          }
}


thenin()                                     /* store THEN in tree */
{
          int i,in,d,fn,fs,b,t,l;
          struct tnode *allocp();
 
          i = THEN;
          in = pr->prev->indent + 1;         /* for pretty printer */
          d = 0;               /* entire construct must be deleted */
          fn = 2;               /* only ELSE may be next construct */
          fs = 0;                   /* any valid instr may follow  */
          b = 1;                 /* may precede compound statement */
          t = 1;                             /* start of construct */
          l = pr->lastin;              /* last valid sym tab entry */
          tree(i,in,d,fn,fs,b,t,l);
          p = p->sub = allocp();
}


elst(i)                                            /* ELSE entered */
int i;
{
         int in,d,fn,fs,b,t,l,*valid;
         struct tnode *allocp();

         if (pr->nfollow != 2)
             printf("ELSE NOT PAIRED WITH THEN\n?");
         else {
            i = ELS;
            in = pr->indent;                 /* for pretty printer */
            d = 0;             /* entire construct must be deleted */
            fn = 4;                     /* no valid next construct */
            fs = 0;                  /* any valid instr may follow */
            b = 1;               /* may precede compound statement */
            t = 1;                           /* start of construct */
            l = pr->lastin;            /* last valid sym tab entry */
            tree(i,in,d,fn,fs,b,t,l);
            p = p->sub = allocp();
            print(curr,CONSOLE);
            prompt("  ",3,"",12,0);
        }
}


iterst(i)                                       /* ITERATE entered */
int i;
{
          int j,in,d,fn,fs,b,t,l,*valid;
          struct tnode *allocp();
 
          in = checkin(&valid);               /* correct location? */
          b = checkcomp(&valid);            /* compound statement? */
          if (valid) {
              d = 0;           /* entire construct must be deleted */
              fn = 0;     /* any valid instr may be next construct */ 
              fs = 2;                   /* only pos int may follow */
              t = 1;                         /* start of construct */
              l = pr->lastin;          /* last valid sym tab entry */
              tree(i,in,d,fn,fs,b,t,l);
              p = p->sub = allocp();
	      for (j = 0; j < in; j++)
		 printf("  "); 
              prompt("ITERATE ",2," TIMES",22,0);
              intin();                              /* get pos int */
              printf("\n");
              print(pr->prev,CONSOLE);
              print(pr,CONSOLE);
              prompt("  ",3,"",12,0);
         }
}


whilst(i)                                         /* WHILE entered */
int i;
{
          int j,in,d,fn,fs,b,t,l,*valid,c;
          struct tnode *allocp();

          in = checkin(&valid);               /* correct location? */
          b = checkcomp(&valid);            /* compound statement? */
          if (valid) {
              d = 0;           /* entire construct must be deleted */
              fn = 0;     /* any valid instr may be next construct */
              fs = 2;                      /* only test may follow */
              t = 1;                         /* start of construct */
              l = pr->lastin;          /* last valid sym tab entry */
              tree(i,in,d,fn,fs,b,t,l);
              p = p->sub = allocp();
              do {
	       for (j = 0; j < in; j++)
		  printf("  ");
               prompt ("WHILE ",1," DO",8,0);
               c = testin();                           /* get test */
               if (c == MENU) 
                   menu();
              } while (c == MENU);
              print(pr->prev,CONSOLE);
              print(pr,CONSOLE);  
              prompt("  ",3,"",12,0);
          }
}    
         
                           
                      
                                                                                                 
defst(i)                          /* DEFINE-NEW-INSTRUCTION entered */
int i;
{
          int in,d,fn,fs,b,t,l;
          struct tnode *allocp();

          if (pr->nfollow != 1)
             printf("BAD LOCATION FOR DEFINITION. TYPE MN TO SEE MENU.\n?");
          else if (last == 14)
		  printf("MAXIMUM NUMBER OF DEFINITIONS EXCEEDED.\n?");
	  else {
             in = 1;                          /* for pretty printer */
             d = 0;             /* entire construct must be deleted */
             fn = 1;        /* only DEF or BX may be next construct */
             fs = 2;                    /* only new name may follow */ 
             b = 0;             /* won't precede compound statement */
             t = 1;                           /* start of construct */
             if (pr->instr == DEF) 
                 l = new[pr->lastin].nextin;
             else l = pr->lastin;       /* last valid sym tab entry */
             tree(i,in,d,fn,fs,b,t,l);
             p = p->sub = allocp(); 
             newin();                               /* get new name */
             print(pr->prev,CONSOLE);
             print(pr,CONSOLE);
             compoundst(5);                            /* add BEGIN */
          }
}


testin()                                      /* store test in tree */
{
          int i,in,d,fn,fs,b,t,l;
          struct tnode *allocp();

          i = getest();
          if (i != MENU) {
              in = pr->indent;                /* for pretty printer */
              d = 2;               /* no deletion, replacement only */
              fn = 4;                          /* no next construct */
              fs = 0;                       /* any instr may follow */
              b = 1;              /* may precede compound statement */
              t = 0;               /* not start or end of construct */
              l = pr->lastin;           /* last valid sym tab entry */
              tree(i,in,d,fn,fs,b,t,l);
              p = p->sub = allocp();
          }
          return i;
}


getest()            /* get valid test */
{
          char w[MAXLINE];
          int m;

          do {
            inputs(w);   
            if ((m = match(w,test,MAXTEST)) == NOMATCH)
               printf("INVALID TEST. TYPE MN TO SEE MENU.\n?");
          } while (m == NOMATCH);
          return m;
}


intin()                              /* store positive number in tree */
{
          int i,in,d,fn,fs,b,t,l;
          struct tnode *allocp();

          i = getint();
          in = pr->indent;                      /* for pretty printer */
          d = 3;                  /* may not be deleted, replace only */
          fn = 4;                          /* no valid next construct */
          fs = 0;                             /* any instr may follow */
          b = 1;                    /* may precede compound statement */
          t = 0;                     /* not start or end of construct */
          l = pr->lastin;                 /* last valid sym tab entry */
          tree(i,in,d,fn,fs,b,t,l);
          p = p->sub = allocp();
}


getint()                                       /* get positive number */
{ 
          char s[3];
          int i,num;
   
          for (i = 0; (s[i] = getchar()) != ' '
				 && s[i] != '\n'; ++i);
          s[i] = '\0';                              /* read in string */
          num = atoi(s);                    /* and convert to integer */
          return num;
}


newin()                                     /* store new name in tree */
{
          int i,in,d,fn,fs,b,t,l;
          struct tnode *allocp();

          i = last;                  /* next location in symbol table */
	  prompt("  DEFINE-NEW-INSTRUCTION ",4," AS",12,0);
          getnew(i);
          putintable(i);
          in = pr->indent;                      /* for pretty printer */
          d = 4;                  /* may not be deleted, replace only */
          fn = 4;                          /* no valid next construct */
          fs = 0;                     /* any defined instr may follow */
          b = 1;                    /* may precede compound statement */
          t = 0;                     /* not start or end of construct */
          l = last;                       /* last valid sym tab entry */
          tree(i,in,d,fn,fs,b,t,l);
          p = p->sub = allocp();
}


getnew(i)                /* get unique new name */
int i;
{
          int c,j,m;

          do {
            if (inputs(new[i].defname) != ERROR &&
	        new[i].defname[0] >= 'a' &&
		new[i].defname[0] <= 'z') {
              m = match(new[i].defname,comm,MAXCOMM);
              if (m == NOMATCH)
                for (j = 0; j < MAXNAME; j++)
                  if ((c = strcomp(new[i].defname,iname[j])) == 0)
                    m = 1;     
              if (m == NOMATCH && last != first) 
                for (j = first; j != i; j = new[j].nextin)
                  if ((c = strcomp(new[i].defname,new[j].defname)) == 0)
                    m = 1;
              if (m != NOMATCH)
                printf("\nNEW NAME MUST BE UNIQUE\n?");
	    } else {
		 printf("\nINVALID CHARACTERS IN NEW NAME\n?");
		 m = 1;
	      }                
          } while (m != NOMATCH);
          new[i].loc[0] = NULL;
}



print(ptr,buf)                     /* pretty printer */
struct tnode *ptr;
FILE *buf;
{
          int d,i;
 
          d = ptr->del;
          if (d == TEST || d == POSINT || d == NEWINSTR)
              switch(ptr->prev->instr) {
              case 11:             /* instr is an if test */
                 printest(ptr,buf);
                 fprintf(buf,"\n");
                 return;
              case 14:             /* instr is a pos num */
                 fprintf(buf,"%2d TIMES\n", ptr->instr);
                 return;
              case 15:             /* instr is a while test */
                 printest(ptr,buf);
                 fprintf(buf," DO\n");
                 return;
              case 16:             /* instr is a new name */
                 fprintf(buf,"%s AS\n",new[ptr->instr].defname);
                 return;
              }    
          
          for (i = 0; i < ptr->indent; i++)
               fprintf(buf,"  ");  /* indent line */

          if (d == NEWINSTR) {
             fprintf(buf,"%s",new[ptr->instr].defname);
             endline(ptr,buf);
             return;
          }
         
          switch(ptr->instr) {
          case 0:
             fprintf(buf,"move");
             endline(ptr,buf);
             break;
          case 1:
             fprintf(buf,"turnleft");
             endline(ptr,buf);
             break;
          case 2:
             fprintf(buf,"pickbeeper");
             endline(ptr,buf);
             break;
          case 3:
             fprintf(buf,"putbeeper");
             endline(ptr,buf);
             break;
          case 4:
             fprintf(buf,"turnoff");
             endline(ptr,buf);
             break;
          case 8:
             fprintf(buf,"END");
             endline(ptr,buf);
             break;
          case 5:
             fprintf(buf,"BEGIN\n"); 
             break;
          case 6:
             fprintf(buf,"BEGINNING-OF-PROGRAM\n");
             break;
          case 7:
             fprintf(buf,"\n  BEGINNING-OF-EXECUTION\n");
             break;
          case 9:
             fprintf(buf,"END-OF-EXECUTION\n");
             fprintf(buf,"END-OF-PROGRAM\n");
             break;
          case 11:
             fprintf(buf,"IF ");
             break;
          case 12:
             fprintf(buf,"THEN\n");
             break;
          case 13:
             fprintf(buf,"ELSE\n");
             break;
          case 14:
             fprintf(buf,"ITERATE ");
             break;
          case 15:
             fprintf(buf,"WHILE ");
             break;
          case 16:
             fprintf(buf,"\n  DEFINE-NEW-INSTRUCTION ");
             break;
          }
}



endline(ptr,buf)             /* add ; if required and start newline */
struct tnode *ptr;
FILE *buf;
{
          int i;
         
          i = ptr->next->instr;
          if ((i == ND || i == NDX || i == ELS) &&
 	       ptr->next->del != NEWINSTR)
             fprintf(buf,"\n");
          else fprintf(buf,";\n");  
}


printest(ptr,buf)     /* print test after IF or WHILE */
struct tnode *ptr;
FILE *buf; 
{

          switch(ptr->instr) {
          case 0:
             fprintf(buf,"facing-north");
             break;
          case 1:
             fprintf(buf,"not-facing-north");
             break;
          case 2:
             fprintf(buf,"facing-south");
             break;
          case 3:
             fprintf(buf,"not-facing-south");
             break;
          case 4:
             fprintf(buf,"facing-east");
             break;
          case 5:
             fprintf(buf,"not-facing-east");
             break;
          case 6:
             fprintf(buf,"facing-west");
             break; 
          case 7:
             fprintf(buf,"not-facing-west");
             break;        
          case 8:
             fprintf(buf,"next-to-a-beeper");
             break;
          case 9:
             fprintf(buf,"not-next-to-a-beeper");
             break;
          case 10:
             fprintf(buf,"any-beepers-in-beeper-bag");
             break;
          case 11:
             fprintf(buf,"no-beepers-in-beeper-bag");
             break;
          case 12:
             fprintf(buf,"front-is-clear");
             break;
          case 13:
             fprintf(buf,"front-is-blocked");
             break;
          case 14:
             fprintf(buf,"left-is-clear");
             break;
          case 15:
             fprintf(buf,"left-is-blocked");
             break;
          case 16:
             fprintf(buf,"right-is-clear");
             break;
          case 17:
             fprintf(buf,"right-is-blocked");
             break;
          case 18:
             menu();
             break;
          }
}

