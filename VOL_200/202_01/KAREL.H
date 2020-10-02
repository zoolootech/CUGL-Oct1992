/* global constants */

#define CURPOS  '\033'          /* cursor control ESC  */
#define ERROR     -1            /* error flag */
#define MAXLINE   150           /* longest input line */
#define NSECTS    8             /* number of sectors */
#define SECSIZ    128           /* sector size */
#define CPMEOF    0X1A          /* end of file for op system */
#define BRACKET   '['
#define LETTERH    'H'
#define SEMI       ';'
#define LETTERJ    'J'
#define TWO        '2'
#define BELL       '\007'

/* global variables */

char wworld[23][75];             /* array for Karel's world */
int kst;                         /* Karel's street */
int kav;                         /* Karel's avenue */
char kdir;                       /* Karel's direction */
int nnorth;                     
int ssouth;
int eeast;
int wwest;
int ccorner;
int bbag;
int ffront;
int rright;
int lleft;

set_up(argv)   /* prepare for execution of KAREL's program */
char **argv;
{

          FILE *fileptr;

          fileptr = fopen(argv[1],"r");
          readworld(fileptr);
          printworld();
          setbooleans();
          wwait();  wwait();
}
             
readworld(fptr)   /* read values stored in world file */
FILE *fptr;
{
          int i,j;

          fscanf(fptr,"%d %d %d",&kst,&kav,&bbag);
          kdir = getc(fptr);
          for (i = 0; i < 23; i++)
            for (j = 0; j < 75; j++)
              wworld[i][j] = getc(fptr);
          fclose(fptr);
}


printworld()   /* output values stored in world array */
{
          int i,j;

          printf("%c%c%c%c",CURPOS,BRACKET,TWO,LETTERJ);
          for (i = 0; i < 23; i++) {
           set_curs(i+1,1,0,"");
            for (j = 0; j < 75; j++) {
              printf("%c",wworld[i][j]);
           }
          } 
}


setbooleans()     /* evaluate tests for KAREL's program */
{
          char k;

          k = wworld[kst][kav];
          if (k == '.' || k == '^' || k == 'V' ||
                          k == '>' || k == '<')
             ccorner = 0;
          else ccorner = 1;

          nnorth = (kdir == '^');
          ssouth = (kdir == 'V');
          eeast = (kdir == '>');
          wwest = (kdir == '<');

          switch(kdir) {
          case '^':
             ffront = (wworld[kst - 1][kav] == ' ');
             rright = (wworld[kst][kav + 2] == ' ');
             lleft = (wworld[kst][kav - 2] == ' ');
             break;
          case 'V':
             ffront = (wworld[kst + 1][kav] == ' ');
             rright = (wworld[kst][kav - 2] == ' ');
             lleft = (wworld[kst][kav + 2] == ' ');
             break;
          case '>':
             ffront = (wworld[kst][kav + 2] == ' ');
             rright = (wworld[kst + 1][kav] == ' ');
             lleft = (wworld[kst - 1][kav] == ' ');
             break;
          case'<':
             ffront = (wworld[kst][kav - 2] == ' ');
             rright = (wworld[kst - 1][kav] == ' ');
             lleft = (wworld[kst + 1][kav] == ' ');
          }
}


move()        /* move KAREL one unit in direction faced */
{

          if (ffront) {               /* if front is clear */
             if (!ccorner) {   /* and no beepers on corner */
                set_curs(kst+1,kav+1,1,".");     /* put a "." */
                wwait();
             }
             switch(kdir) {     /* determine new location */
             case '^':
                kst -= 2;
                break;
             case 'V':
                kst += 2;
                break;
             case '<':
                kav -= 4;
                break;
             case '>':
                kav  += 4;
             }
             setbooleans();         /* re-evaluate booleans */
             if (!ccorner) {     /* if no beepers on corner */
                wworld[kst][kav] = kdir;     /* print KAREL */
                set_curs(kst+1,kav+1,1,"");
                printf("%c",kdir);
                wwait();
             } else {
                  set_curs(kst+1,kav+1,1,"");
                  printf("%c",kdir);
                  wwait();
                  set_curs(kst+1,kav+1,1,"");
                  printf("%c",wworld[kst][kav]);
                  wwait();
               }
          } else turnoff(1);  /* error if front is blocked */
}


turnleft()     /* rotate KAREL 90 degrees counterclockwise */
{
          switch(kdir) {
          case '^':
             kdir = '<';
             break;
          case 'V':
             kdir = '>';
             break;
          case '<':
             kdir = 'V';
             break;
          case '>':
             kdir = '^';
          }
          if (!ccorner) {    /* if no beepers on corner */
              wworld[kst][kav] = kdir;   /* print KAREL */
              set_curs(kst+1,kav+1,1,"");
              printf("%c",kdir);
              wwait();
          } else {
               set_curs(kst+1,kav+1,1,"");
               printf("%c",kdir);
               wwait();
               set_curs(kst+1,kav+1,1,"");
               printf("%c",wworld[kst][kav]);
               wwait();
            } 
          setbooleans();
}


pickbeeper()    /* pick up a beeper from KAREL's corner */
{
          char k;
          
          k = wworld[kst][kav];
          if (!ccorner)
             turnoff(3);
          else { printf("%c",BELL);
                 if (k == '2')
                     k = '@';
             else if (k != '@')
                     --k;
             else {
                k = kdir;
                ccorner = 0;
             }
             wworld[kst][kav] = k; 
             set_curs(kst+1,kav+1,1,"");
             printf("%c",k);
             wwait();
             ++bbag;
          }
}

putbeeper()     /* place a beeper on KAREL's corner */
{
          char k;
         
          k = wworld[kst][kav];
          if (!bbag)
             turnoff(2);
          else { 
             if (!ccorner) { 
                k = '@';
                ccorner = 1;
             } else if (k == '@')
                       k = '2';
               else if (k == '9')
                       set_curs(23,0,75,"ONLY 9 BEEPERS ALLOWED.");
               else ++k;
             wworld[kst][kav] = k;
             set_curs(kst+1,kav+1,1,"");
             printf("%c",k);
             wwait();
             --bbag; 
          }
}


turnoff(code)    /* stop execution of KAREL's program */
int code;
{
          if (code == 0) 
             set_curs(24,0,45,"  END OF EXECUTION.");
          else { 
               set_curs(24,0,45,"  ERROR SHUTOFF.");
               switch (code) {
               case 1:
                  set_curs(24,17,0,"FRONT IS BLOCKED.");
                  break;
               case 2:
                  set_curs(24,17,0,"NO BEEPERS IN BAG.");
                  break;
               case 3:
                  set_curs(24,17,0,"NO BEEPERS ON CORNER.");
               case 4:
                  set_curs(24,17,0,"NO TURNOFF EXECUTED.");
               }
            }
          set_curs(25,37,0,"");
          kst = (21 - kst)/2;
          kav = (kav - 4)/4;
          printf("KAREL'S ST= %d  AVE= %d  DIR= %c BAG= %d",
                                            kst,kav,kdir,bbag);
          set_curs(22,0,0,"");
          exit();
}


set_curs(row,col,blanks,mess)    /* set cursor */
int row,col,blanks;
char *mess;
{
          int j;

          printf("%c%c%d%c%d%c",CURPOS,BRACKET,row,SEMI,col,LETTERH);
          for (j = 1; j <= blanks; j++)
            printf(" ");
          printf("%c%c%d%c%d%c",CURPOS,BRACKET,row,SEMI,col,LETTERH);
          printf("%s", mess);
}


wwait()      /* slow KAREL so execution can be seen */
{
          int j;
          int k;

          for (k = 0; k < SPEED; k++)
             for (j = 0; j < 6500; j++)
            ;
}
