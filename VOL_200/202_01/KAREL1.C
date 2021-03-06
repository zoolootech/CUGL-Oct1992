#include <stdio.h>
#include <ctype.h>

#define CURPOS      '\033'      /* ESC */
#define ERROR       -1          /* error flag */
#define MAXLINE     150         /* longest input line */
#define NSECTS      8           /* number of sectors */
#define SECSIZ      128         /* sector size */
#define CPMEOF      0x1A        /* end of file for op system */
#define BRACKET     '['
#define LETTERH     'H'
#define SEMI        ';'
#define TWO         '2'
#define LETTERJ     'J'

char world[23][75];             /* array for Karel's world */
int bag;                        /* Karel's beeper bag */
int kst;                        /* Karel's street */
int kav;                        /* Karel's avenue */
char kdir;                      /* Karel's direction */
int last;                       /* next loc in sym table */

main(argc,argv)
int argc;                       /* number of arguments for main */
char **argv;                    /* arg vector */
{
          char ans;
          FILE *fileptr;
          int speed;

          if (argc < 3)
             printf("MISSING FILENAME(S)\n");
          else {
             if ((fileptr = fopen(argv[2],"r")) != NULL) {
                readworld(fileptr);
                printworld();
             } else { 
                  initworld(); 
                  printworld();
               }
             fclose(fileptr);
             set_curs(25,0,0,"CHANGE KAREL'S LOCATION? (Y/N)");
             if ((ans = upper(getchr())) != 'N')
                kareloc();
             set_curs(25,0,75,"CHANGE BEEPERS IN WORLD? (Y/N)");
             if ((ans = upper(getchr())) != 'N')
                beeploc();
             set_curs(25,0,75,"CHANGE BEEPERS IN BAG? (Y/N)");
             if ((ans = upper(getchr())) != 'N')
                bagloc();
             set_curs(25,0,75,"CHANGE WALL LOCATIONS? (Y/N)");
             if ((ans = upper(getchr())) != 'N')
                wallsloc();
             alterations();
             set_curs(25,0,75,"EXECUTE PROGRAM? (Y/N)");
             if ((ans = upper(getchr())) != 'Y') {
                set_curs(25,0,75,"EXIT KAREL? (Y/N)");
                if ((ans = upper(getchr())) != 'N')
                   exit();
             }
             saveworld(argv,fileptr);
             do {
                set_curs(25,0,75,"KAREL'S SPEED? (1,2,3)");
                speed = getint();
             } while (speed != 1 && speed != 2 && speed != 3);
             createpgm(argv,speed);
          }
}


char upper(c)
char c;
{
     return islower(c) ? c-32 : c;
}

initworld()   /* initialize "empty world" */
{
          int i,j;
          char c;

          kst = 21;                          /* init Karel's st */
          kav = 4;                          /* init Karel's ave */
          kdir = '0';                       /* init Karel's dir */

          for (i = 0; i < 23; i++)
            for (j = 0; j < 75; j++)         /* blank out array */
              world[i][j] = ' ';

          for (i = 1; i < 21; i+=2)
            for (j = 8; j < 75; j += 4)
              world[i][j] = '.';                     /* corners */

          for (i = 0; i < 20; i++) {
            world[i][6] = '|';            /* left edge of world */
            world[i][74] = '|';          /* right edge of world */
          }
          for (j = 6; j < 75; j++) {
            world[20][j] = '-';          /* lower edge of world */
            world[0][j] = '-';           /* upper edge of world */
          }

          world[1][0] = 'S';            world[22][66] = 'A';
          world[2][0] = 'T';            world[22][67] = 'V';
          world[3][0] = 'R';            world[22][68] = 'E';
          world[4][0] = 'E';            world[22][69] = 'N';
          world[5][0] = 'E';            world[22][70] = 'U';
          world[6][0] = 'T';            world[22][71] = 'E';
          world[7][0] = 'S';            world[22][72] = 'S';

          world[1][3] = '1';  world[1][4] = '0';
          for (i = 3, c = '9'; i < 21; i += 2, c--)
            world[i][4] = c;                  /* number streets */

          for (j = 8, c = '1'; j < 75; j += 4, c++) {
            world[21][j] = c;                 /* number avenues */
            if (c > '9') {
                world[21][--j] = '1';
                world[21][++j] = c - 10;
            }
          }
}

readworld(fptr)    /* input values from old world file */
FILE *fptr;
{
          int i,j;

          fscanf(fptr,"%d %d %d",&kst,&kav,&bag);
          kdir = getc(fptr);
          for (i = 0; i < 23; i++)
            for (j = 0; j < 75; j++)
              world[i][j] = getc(fptr);
}


printworld()    /* output values stored in world array */
{
          int i,j,kst2,kav2;

          printf("%c%c%c%c",CURPOS,BRACKET,TWO,LETTERJ);
          for (i = 0; i < 23; i++) {
           set_curs(i+1,1,0,"");
            for (j = 0; j < 75; j++) {
              printf("%c",world[i][j]);
            }
          }
          set_curs(25,37,0,"");
          kst2 = (21 - kst)/2;
          kav2 = (kav - 4)/4;
          printf("KAREL'S ST= %d  AVE= %d  DIR= %c BAG= %d",
                        kst2,kav2,kdir,bag);
}


kareloc()/* determine street, avenue and direction for KAREL */
{
          char ans,k;

          k = world[kst][kav];
          do {
             if (kst != 0 && (k == '^' || k == 'V' || 
                              k == '>' || k == '<')) {
                world[kst][kav] = '.';
                set_curs(kst+1,kav+1,1,".");
             }
             do {
                set_curs(25,0,75,"KAREL'S STREET?");
                kst = getint();
             } while (kst == 0 || kst > 10);
             do {
                set_curs(25,0,75,"KAREL'S AVENUE?");
                kav  = getint();
             } while (kav == 0 || kav > 17);
             kst = 21 - 2 * kst;
             kav = 4 + 4 * kav;
             set_curs(25,0,75,"DIRECTION? (N/S/E/W)");
             kdir = upper(getchr());
             storekarel();
             set_curs(kst+1,kav+1,1,"");
             printf("%c",kdir);
             set_curs(25,0,75,"IS KAREL'S LOCATION CORRECT? (Y/N)");
             if ((ans = upper(getchr())) != 'Y') {
                set_curs(kst+1,kav+1,1,"");
                printf("%c", world[kst][kav]);
             }
         } while (ans != 'Y');
         if (world[kst][kav] == '.')
            world[kst][kav] = kdir;
}


storekarel()   /* determine KAREL's direction from console input */
{           
             char k;

             switch(kdir) {
             case 'N':
                kdir = '^';
                break;
             case 'S':
                kdir = 'V';
                break;
             case 'E':
                kdir = '>';
                break;
             case 'W':
             default:
                kdir = '<';
             }  
}
 


beeploc()  /* determine beeper locations */
{
          int bst,bav,valid;
          char ans;

          ans = 'Y';
          while (ans != 'N') {
             do {
                set_curs(25,0,75,"STREET?");
                bst = getint();
             } while (bst == 0 || bst > 10);
             do {
                set_curs(25,0,75,"AVENUE?");
                bav = getint();
             } while (bav == 0 || bav > 17);
             bst = 21 - 2 * bst;
             bav = 4 + 4 * bav;
             set_curs(25,0,75,"TYPE A TO ADD OR D TO DELETE BEEPER");
             if ((ans = upper(getchr())) != 'D') { 
                valid = putbeeper(bst,bav);
                if (valid) {
                   set_curs(bst+1,bav+1,1,"");
                   printf("%c",world[bst][bav]);
                   set_curs(25,0,75,"IS BEEPER LOCATION CORRECT?(Y/N)");
                   if ((ans = upper(getchr())) != 'Y') {
                      pickbeeper(bst,bav);
                      set_curs(bst+1,bav+1,1,"");
                      printf("%c",world[bst][bav]);
                   }
                }
             } else {
                  pickbeeper(bst,bav);
                  set_curs(bst+1,bav+1,1,"");
                  printf("%c",world[bst][bav]);
               }
            set_curs(25,0,75,"OTHER BEEPER LOCATIONS? (Y/N)");
            ans = upper(getchr());
         } 
        
}


bagloc() /* determine number of beepers in beeper bag */
{
          char ans;

          do {
             set_curs(25,0,75,"HOW MANY BEEPERS IN BEEPER BAG?");
             bag = getint();
             set_curs(25,0,75,"");
             printf("%d BEEPERS IN BAG, CORRECT? (Y/N)",bag);
          } while ((ans = upper(getchr())) != 'Y');
}


putbeeper(bst,bav) /* place one beeper in the world */
int bst,bav;       /* at bst street and bav avenue  */
{
          char k;

          k = world[bst][bav];
          if (k == '9') {
             set_curs(25,0,75,"NO MORE THAN 9 BEEPERS ON A CORNER.");
             set_curs(25,36,0,"TYPE ANY CHARACTER TO CONTINUE");
             getchr();
             return (0);
          } else {
               if (k == '.' || k == '^' || k == 'V' ||
                               k == '>' || k == '<')
                  k = '@';
               else if (k == '@')
                       k = '2';
               else ++k;
               world[bst][bav] = k;
               return(1);
            }
}


pickbeeper(bst,bav)  /* remove one beeper from the world   */
int bst,bav;         /* location bst street and bav avenue */
{
          char k;
          
          k = world[bst][bav];
          if (k == '.' || k == '^' || k == 'V' ||
                          k == '<' || k == '>') {
             set_curs(25,0,75,"NO BEEPER ON CORNER.");
             set_curs(25,21,0,"TYPE ANY CHARACTER TO CONTINUE.");
             getchr();
          } else {
               if (bst == kst && bav == kav && k == '@')
                  k = kdir;
               else if (k == '2')
                        k = '@';
               else if (k != '@')
                       --k;
               else k = '.';
               world[bst][bav] = k;
            }
}


wallsloc()   /* determine wall location(s) */
{
          int i,j,s1,s2,st1,st2,a1,a2,av1,av2,horiz;
          char ans;

          do {
             do {
                set_curs(25,0,75,"BETWEEN WHAT STREETS?");
                s1 = getint();
                set_curs(25,0,40,"?");
                s2 = getint();
             } while (s1 < 0 || s2 < 0 || s1 > 11 || s2 > 11);
             st1 = min(s1,s2);
             st2 = max(s1,s2);
             horiz = abs(st2 - st1);
             st1 = 20 - 2 * st1;
             st2 = 22 - 2 * st2;
             do {
                set_curs(25,0,40,"BETWEEN WHAT AVENUES?");
                a1 = getint();
                set_curs(25,0,40,"?");
                a2 = getint();
             } while (a1 < 0 || a2 < 0 || a1 > 18 || a2 > 18);
             av1 = min(a1,a2);
             av2 = max(a1,a2);
             av1 = 6 + 4 * av1;
             av2 = 2 + 4 * av2;
             set_curs(25,0,75,"TYPE A TO ADD OR D TO DELETE THE WALL.");
             if ((ans = upper(getchr())) != 'D') {
                addwall(st1,st2,av1,av2,horiz);
                set_curs(25,0,40,"IS WALL LOCATION CORRECT? (Y/N)");
                if ((ans = upper(getchr())) != 'Y')
                   erasewall(st1,st2,av1,av2,horiz);
                else {
                   set_curs(25,0,40,"IS THIS THE LAST WALL? (Y/N)");
                   ans = upper(getchr());
                }
              } else {
                   erasewall(st1,st2,av1,av2,horiz);
                   set_curs(25,0,40,"IS THIS THE LAST WALL? (Y/N)");
                   ans = upper(getchr());
                }
          } while (ans != 'Y');
}


addwall(st1,st2,av1,av2,horiz)   /* add one wall to the world */
int st1,st2,av1,av2,horiz;
{
           int i,j;

           if (horiz == 1)
              for (j = av1; j <= av2; j++) {
                  if (world[st1][j] == '|' ||
                      world[st1][j] == '+')
                      world[st1][j] = '+';
                  else world[st1][j] = '-';       
                  set_curs(st1+1,j+1,1,"");
                  printf("%c",world[st1][j]);
              }
            else for (i = st2; i <= st1; i++) {
                     if (world[i][av1] == '-' ||
                         world[i][av1] == '+')
                         world[i][av1] = '+';
                     else world[i][av1] = '|'; 
                     set_curs(i+1,av1+1,1,"");
                     printf("%c",world[i][av1]);
                 }
}


erasewall(st1,st2,av1,av2,horiz)  /* remove one wall from the world */
int st1,st2,av1,av2,horiz;
{ 
           int i,j;

           if (horiz == 1)
              for (j = av1; j <= av2; j++) {
                if (j == 6 || j ==74)
                   world[st1][j] = '|';
                else if (world[st1][j] == '+')
                        world[st1][j] = '|';
                else world[st1][j] = ' ';
                set_curs(st1+1,j+1,1,"");
                printf("%c",world[st1][j]);
              }
           else for (i = st2; i <= st1; i++) {
                  if (i == 0 || i == 20)
                     world[i][av1] = '-';
                  else if (world[i][av1] == '+')
                          world[i][av1] = '-';
                  else world[i][av1] = ' ';
                  set_curs(i+1,av1+1,1,"");
                  printf("%c",world[i][av1]);
                }
}


int atoi(n)
char *n;
{
          int val;
          char c;
          int sign;
          val=0;
          sign=1;
          while ((c = *n) == '/t' || c== ' ') ++n;
          if (c== '-') {sign = -1; n++;}
          while ( isdigit(c = *n++)) val = val * 10 + c - '0';
          return sign*val;
}

getint()  /* determine integer input from console */
{
          char str[5];
          int in;

          inputs(str);
          in = atoi(str);
          return in;
}


inputs(str)   /* read string input from console */
char str[];
{
          int j;

          j = 0;
          while ((str[j++] = getchr()) != '\r')
              ;
          str[--j] = '\0';
}


alterations()   /* additional changes to the world */
{ 
          char ans;

          set_curs(25,0,40,"ANY FURTHER CHANGES TO THE WORLD? (Y/N)");
          while ((ans = upper(getchr())) != 'N') {
             set_curs(25,0,40,"CHANGE KAREL'S POSITION? (Y/N)");
             if ((ans = upper(getchr())) == 'Y')
                kareloc();
             set_curs(25,0,40,"CHANGE BEEPERS IN WORLD? (Y/N)");
             if ((ans = upper(getchr())) == 'Y')
                beeploc();
             set_curs(25,0,40,"CHANGE BEEPERS IN BAG? (Y/N)");
             if ((ans = upper(getchr())) == 'Y')
                bagloc();
             set_curs(25,0,40,"CHANGE WALL LOCATIONS? (Y/N)");
             if ((ans = upper(getchr())) == 'Y')
                wallsloc();
             set_curs(25,0,40,"ANY FURTHER CHANGES TO THE WORLD? (Y/N)");
          }
}


set_curs(row,col,blanks,mess) /* set cursor and print message */
int row,col,blanks;
char *mess;
{
          int j;

          printf("%c%c%d%c%d%c",CURPOS,BRACKET,row,SEMI,col,LETTERH);
          for (j = 1; j <= blanks; j++)
            printf(" ");        /* blank out previous message */
          printf("%c%c%d%c%d%c",CURPOS,BRACKET,row,SEMI,col,LETTERH);
          printf("%s", mess);   /* output current message */
}



saveworld(argv,fptr)   /* save world array in second file */
char **argv;
FILE *fptr;
{
          int i,j;

          fptr = fopen(argv[2],"w");
          fprintf(fptr,"%d %d %d",kst,kav,bag);
          putc(kdir,fptr);
          for (i = 0; i < 23; i++)
            for (j = 0; j < 75; j++)
              putc(world[i][j],fptr);
          putc(CPMEOF,fptr);
          fclose(fptr);
}


getchr()  /* low-level character input */
{
          char c;

          c = getch();
          putch(c);
          return c;
}




createpgm(argv,speed)   /* create C pgm from parse tree */
char **argv;
int speed;
{
          int i,in,d,fn,fs,b,t,l,reloc[10];
          char newname[10][MAXLINE],fname[12];
          char *test[19],*j[9];
          FILE *fileptro,*fileptri;

          last = 0;
          init(test); 
          if ((fileptri = fopen(argv[1],"r")) == NULL)
             printf("CAN'T FIND TREE FILE.\n");
          else {
             strcpy(fname,argv[1]); 
             strcat(fname,".C");
             if (( fileptro = fopen(fname,"w")) == NULL)
                printf("CAN'T CREATE NEW FILE.\n");
             else {
                fprintf(fileptro,"#include <stdio.h>\n");
                fprintf(fileptro,"%s%d%s","#define SPEED   ",speed,"\n");
                fprintf(fileptro,"#include <karel.h>\n");
                setupspace(j);
                fprintf(fileptro,"int j1,j2,j3,j4,j5,j6,j7,j8;\n");
                fscanf(fileptri,"%d %d %d %d %d %d %d %d",&i,&in,&d,&fn,&fs,&b,&t,&l);
                fscanf(fileptri,"%d %d %d %d %d %d %d %d",&i,&in,&d,&fn,&fs,&b,&t,&l);
                while (i != 9) { /* translate to end of program */
                    createline(j,i,in,d,newname,reloc,test,fileptri,fileptro);
                    fscanf(fileptri,"%d %d %d %d %d %d %d %d",&i,&in,&d,&fn,&fs,&b,&t,&l);
                 }
                 fprintf(fileptro,"  turnoff(4);\n");
                 fprintf(fileptro,"  } \n");
                 putc(CPMEOF,fileptro);
                 fclose(fileptri);
                 fclose(fileptro);
              } 
          } 
}

setupspace(j)      /*  initialize array of ITERATE loop counters */
char *j[9];
{

         char *malloc();
         int i;

         for (i = 0; i <= 8; i++)
              j[i] = malloc(2);
         j[1] = "j1";           j[2] = "j2";
         j[3] = "j3";           j[4] = "j4";
         j[5] = "j5";           j[6] = "j6";
         j[7] = "j7";           j[8] = "j8";
}


init(test)         /* initialize array with C version of tests */
char *test[19];
{

          int i;
          char *malloc();

for (i = 0; i < 19; i++)        /* allocate space for tests */
  test[i] = malloc(10);

test[0] = "nnorth";
test[1] = "!nnorth";
test[2] = "ssouth";
test[3] = "!ssouth";
test[4] = "eeast";
test[5] = "!eeast";
test[6] = "wwest";
test[7] = "!wwest";
test[8] = "ccorner";
test[9] = "!ccorner";
test[10] = "bbag";
test[11] = "!bbag";
test[12] = "ffront";
test[13] = "!ffront";
test[14] = "lleft";
test[15] = "!lleft";
test[16] = "rright";
test[17] = "!rright";
}


createline(j,i,in,d,newname,reloc,test,fptri,fptro)
int i,in,d,reloc[10];
char newname[10][MAXLINE],*test[19],*j[9];
FILE *fptri,*fptro;
{
          int k,fn,fs,b,t,l;
          char w[MAXLINE];

          for (k = 0; k < (in - 1); k++)
            fprintf(fptro,"  ");
          if (d != 4)
             switch(i) {
             case 0:
                fprintf(fptro,"move();\n");
                break;
             case 1:
                fprintf(fptro,"turnleft();\n");
                break;
             case 2:
                fprintf(fptro,"pickbeeper();\n");
                break;
             case 3:
                fprintf(fptro,"putbeeper();\n");
                break;
             case 4:
                fprintf(fptro,"turnoff(0);\n");
                break;
             case 5:
                fprintf(fptro,"{ \n");
                break;
             case 7:
                fprintf(fptro,"\nmain(argc,argv) \n");
                fprintf(fptro,"int argc;\n");
                fprintf(fptro,"char **argv;\n");
                fprintf(fptro,"  { \n");
                fprintf(fptro,"  set_up(argv);\n");
                break;
             case 8:
                fprintf(fptro,"} \n");
                break;
             case 11:
                fscanf(fptri,"%d %d %d %d %d %d %d %d",&i,&in,&d,&fn,&fs,&b,&t,&l);   /* read test */
                fprintf(fptro,"if (%s)\n",test[i]);
                fscanf(fptri,"%d %d %d %d %d %d %d %d",&i,&in,&d,&fn,&fs,&b,&t,&l);   /* skip THEN */
                break;
             case 13:
                fprintf(fptro,"else\n");
                break;
             case 14:
                fscanf(fptri,"%d %d %d %d %d %d %d %d",&i,&in,&d,&fn,&fs,&b,&t,&l);
                fprintf(fptro,"for (%s = 0; %s < %d; %s++)\n",
                        j[in],j[in],i,j[in]);
                break;
             case 15:
                fscanf(fptri,"%d %d %d %d %d %d %d %d",&i,&in,&d,&fn,&fs,&b,&t,&l);
                fprintf(fptro,"while (%s)\n",test[i]);
                break;
             case 16:
                fscanf(fptri,"%d %d %d %d %d %d %d %d",&i,&in,&d,&fn,&fs,&b,&t,&l);
                while ((getc(fptri)) != '\n');
                i = reloc[i] = last++;
                k = 0;
                while ((w[k++] = getc(fptri)) != '\n')
                    if (w[k - 1] == '-')
                       w[k - 1] = '_';
                k -= 2;
                w[k] = '\0';
                strcpy(newname[i],w); 
                fprintf(fptro,"\n%s()\n",newname[i]);
                break;
             default:
                ;
             }
          else { 
             i = reloc[i];
             fprintf(fptro,"%s();\n",newname[i]);
          }
}
