/*#define VAX*/
#define PHASE2
/*#define PHASE3*/
#define P15nov86
#define P17nov86
/*#define P18nov86*/
#define P28nov86
#ifdef VAX
#include <stdio.h>
#include <ctype.h>
#else
#include stdio.h
extern int fputs(), strncmp(), strncpy();
#endif

#define save_line_count 20
#define lin_buf_len 1600
#define line 80
#define opt_deps 100

/*  Peephole optimizer for 6809 code */
/*  written by Dieter H. Flunkert   */
/*  Written:     */
/*  Version A.07 */
/*    02-dec-85  */
/*  Update:      */
/*    01-jul-86  */
/*    17-jul-86  */
/*    07-nov-86  */
/*    15-nov-86  */
/*    17-nov-86  */
/*    18-nov-86  */
/*    28-nov-86  */

/* Global variables    */
char new_table[] = "EQNELEGELTGTHIHSLSLO";
char jump_table[] = "NEEQGTLTGELELSLOHIHS";
char number[] = " LB   ";
#ifdef VAX
FILE *ifp, *ofp;
#else
int ofp, ifp;
#endif
char buffer[line];
char save_buffer[lin_buf_len];
char save_jump[line];
char num_1[line];
char num_2[line];
#ifdef P18nov86
int opt_level, wr_ok;
#endif
int opt_end, opt_id, bp;

/* for stand alone  */

/* #define EOF -1  */
/* #define NULL 0  */
/* #define EOL 13  */

#define jump7 "*+7\n"
#define jump5 " BRA  *+5\n"
#define kommas ",S\n"
#define ldd " LDD  "
#define lddnum " LDD  #"
#define ldd0 " LDD  #0\n"
#define ldd1 " LDD  #1\n"
#define pulsd " PULS D\n"
#define pshsd " PSHS D\n"
#define pshsy " PSHS Y\n"
#define pulsx " PULS X\n"
#define pshsx " PSHS X\n"
#define tfryd " TFR  Y,D\n"
#define addd " ADDD "
#define addd1 " ADDD #1\n"
#define adddsplus " ADDD ,S++\n"
#define coma " COMA\n"
#define comb " COMB\n"
#define subd " SUBD "
#define stb " STB  "
#define ldb " LDB  "
#define stbsplus " STB  [,S++]\n"
#define ldbsplus " LDB  [,S++]\n"
#define lddsplus " LDD  [,S++]\n"
#define stdsplus " STD  [,S++]\n"
#define stdx " STD  ,X\n"
#define std " STD  "
#define leay " LEAY "
#define leax " LEAX "
#define cmpd " CMPD "
#define cmpd0 " CMPD #0\n"
#define cmpdsplus " CMPD ,S++\n"
#define lb " LB"
#define lbeqcc " LBEQ cc"
#define lbnecc " LBNE cc"
#define lbsr " JSR "
#define jmp " JMP "

#ifdef VAX
main()
#else
main(argc, argv) int argc, argv[];
#endif
{
/* i points to first free place in save buffer */
/* bp points to next line to be transfered to check buffer */

 int i,j;
 int opt;

#ifdef VAX
init();
#else
init(argc,argv);
#endif
 opt_end = opt_id = bp = i = 0;
 opt = 
#ifdef P18nov86
 wr_ok =
#endif
 1;
 while(getline() != EOF) {    /* read lines until end of file */
  if(i+line >= lin_buf_len) { /* buffer full? */
    write_buffer();           /* write line to file */
    i=m1_down();              /* move buffer down */
    bp=0;                     /* reset buffer pointer */
  }
  i = copy_append(i);         /* copy to save buffer (append it) */
  while((bp = copy_buffer(bp)) > 0) { /* copy to check buffer */
    if(buffer[0] != ";") {    /* process only valid assembler code */
      if(check_1(opt++) != 0) { /* check if good for optimize */
#ifdef P18nov86
     if(wr_ok) 
#endif
       write_buffer(); /* write first line of buffer to file */
#ifdef P18nov86
     if(!wr_ok) {
       m1_down();                /* shift buffer one line */
       m1_down();
     }
#endif
     i = m1_down();  /* shift buffer one line */
     opt_end = bp = 0;
     opt =
#ifdef P18nov86
     wr_ok =
#endif
     1;
    }
    else {

 /* if good for optimize and already the last in this sequence */
 /* write the optimized version and forget the rest     */

     if (opt_end < 0) {
      write_opt();
      opt =
#ifdef P18nov86
      wr_ok =
#endif
      1;
      i = move_buffer(bp); /* move buffer down */
      opt_end = opt_id = bp = 0;
     }
    }
   }
  }
 }
 if (save_buffer[bp] != '\0')
  fputs(save_buffer[bp],ofp); /* write the rest of the save buffer */
   fputs(buffer, ofp);   /* write last line */
}
copy_buffer(bp)
/* copy line of save buffer to check buffer */
int bp; /* buffer pointer of save buffer  */
{
 int i;

 if (bp < 0) bp = -bp;  /* use absolute value */
 if (save_buffer[bp] == '\0') 
    return (-bp); /* return negative value if nothing to do */
 i = 0;
 while((buffer[i++] = save_buffer[bp++]) != '\n') ;
 buffer[i] = '\0';
 return(bp);
}
write_buffer()
/* write first line from save buffer to output file */
{
 copy_buffer(0);
 fputs(buffer,ofp);
}
m1_down()
/* move save buffer one line 'down'  */
{
 int i;

 i = 0;
 while (save_buffer[i++] != '\n') ;
 return(move_buffer(i));
}
move_buffer(bp)
int bp;
/* move buffer bp characters down */
{
 int  j;

 j = 0;
 while((save_buffer[j++] = save_buffer[bp++]) != '\0') ;
 return (--j);
}
write_opt()
/* write optimize string to output file  */
{
 int i,len;

 i = 0;
 switch(opt_id) {
#ifdef PHASE2
  case 1: {
   fputs(num_2,ofp);
   fputs(num_1,ofp);
   break;
   }
#endif
  case 2: {
   fputs(leax,ofp);
   fputs(num_1,ofp);
   fputs(ldd,ofp);
   fputs(num_2,ofp);
   fputs(stdx,ofp);
   break;
  }
  case 3: {
   fputs(number,ofp);
   fputs(num_2,ofp);
   break;
  }
  case 10: {
   fputs(ldd,ofp);
   fputs(num_1,ofp);
   break;
  }
  case 11: {
   fputs(leay,ofp);
   fputs(num_2,ofp);
   fputs(pshsy,ofp);
   fputs(ldd,ofp);
   fputs(num_1,ofp);
   break;
  }
#ifndef P18nov86
  case 12: {
   fputs(ldb,ofp);
   fputs(num_2,ofp);
   fputs(stb,ofp);
   fputs(num_1,ofp);
   break;
  }
#endif
  case 13: {
   fputs(ldd,ofp);
   fputs(num_1,ofp);
   fputs(subd,ofp);
   fputs(num_2,ofp);
   fputs(std,ofp);
   fputs(num_1,ofp);
   break;
  }
  case 14: {
   fputs(ldb,ofp);
   fputs(num_1,ofp);
   break;
  }
  case 15: {
   fputs(lbsr,ofp);
   fputs(num_2,ofp);
   fputs(std,ofp);
   fputs(num_1,ofp);
   break;
  }
#ifndef P18nov86
  case 16: {
   fputs(ldd,ofp);
   fputs(num_2,ofp);
   fputs(std,ofp);
   fputs(num_1,ofp);
   break;
  }
#endif
  case 17: {
   fputs(ldd,ofp);
   fputs(num_1,ofp);
   fputs(addd,ofp);
   fputs(num_2,ofp);
   fputs(std,ofp);
   fputs(num_1,ofp);
   break;
  }
 case 18:
  fputs(save_jump,ofp);
  fputs("8\n",ofp);
  fputs(ldd0,ofp);
  fputs(jmp,ofp);
  fputs(num_2,ofp);
/*
  fputs(ldd1,ofp);
*/
  break;
#ifdef P17nov86
 case 21:
  fputs(lddnum,ofp);
  fputs(num_1,ofp);
  putc('\n',ofp);
  break;
#endif
#ifdef P28nov86
 case 22:
  fputs(num_1,ofp);
  fputs(num_2,ofp);
  break;
#endif
 case 23:
  fputs(num_1,ofp);
  break;
 case 24:
  fputs(lddnum,ofp);
  putc('-',ofp);
  fputs(num_1,ofp);
  break;
#ifdef P18nov86
 case 99:
  fputs(num_2,ofp);
  fputs(num_1,ofp);
  break;
#endif
 default: break;
 }
}
#ifdef VAX
init()
/*  get filenames and open files   */
{
 int i;   /* just for counting */
 char infile[80]; /* file names   */
 char outfile[80];

 i = 0;
 fputs("\nFile to optimize? ",stdout);
 while((infile[i]=getchar()) != '\n')
    if(infile[i]!='\b') ++i;
    else {
     if(--i < 0) i = 0;
     putc(' ',stdout);
     putc('\b',stdout);
    }
 infile[i] = '\0';
 if ((ifp = fopen(infile,"r")) == NULL) {
  fputs("\n*** E R R O R *** opening file ",stdout);
    fputs(infile,stdout);
  exit(0);
 }
 i = 0;
 fputs("\nOutput File Name? ",stdout);
 while((outfile[i]=getchar()) != '\n')
    if(outfile[i]!='\b') ++i;
    else {
     if(--i < 0) i=0;
     putc(' ',stdout);
     putc('\b',stdout);
    }
 outfile[i] = '\0';
   if(outfile[0] == '\0') {
    ofp = stdout;
    return;
   }
 if ((ofp = fopen(outfile,"w")) == NULL) {
  fputs("\n*** E R R O R *** opening file ",stdout);
    fputs(outfile,stdout);
  exit(0);
 }
}
#else
init(argc, argv) int argc, argv[]; {
char *argptr;
int i;
  i=0;
  ifp=stdin;
  ofp=stdout;
  if(argc==1) {
    fputs("\nusage peephole <infile >outfile",stderr);
    exit(0);
  }
  while(--argc) {
    argptr=argv[++i];
    if(*argptr=='<') {
      if((ifp=fopen(++argptr,"r")) == 0) {
         fputs("\nerror opening input file",stderr);
         exit(0);
       }
     }
     if(*argptr=='>') {
       if((ofp=fopen(++argptr,"w")) == 0) {
         fputs("\nerror opening output file",stderr);
         exit(0);
       }
     }
   }
 }
#endif
getline()
/* read a line from input file */
{
 int i;
 char c;
 
 i = 0;
 if(CCPOLL()) if(getc(stdin)==3) exit(0);
 while((c = getc(ifp)) != EOF) {
  buffer[i++] = c;
    if(c == '\n') break;
 }
 buffer[i] = '\0';
   if(c == EOF) return EOF;
 return(0);
}
copy_append(i)
/* append a string to the string save area  */
int i;
{
 int j;

 j = 0;
 while((save_buffer[i] = buffer[j++]) != '\n')
    if(save_buffer[i++] == '\0') return(--i);
 save_buffer[++i] = '\0';
 return(i);
}
check_1(opt)
/* check if the line is optimizeable */
int opt;
{
 int i;
 switch(opt) {
  case 1: {
#ifdef P18nov86
   if(strcmp(buffer,stdsplus)==0 ||
      strcmp(buffer,stbsplus)==0) {
    strncpy(num_2,buffer,strlen(std));
    if(opt_level) {
      itoa(stackoffset[--opt_level],work);
      i=0;
      while(num_1[i++] != ',');
      strcat(work,kommas);
      strcpy(num_1,work);
      opt_end = -1;
      opt_id = 99;
      return 0;
    }
   }
#endif
#ifdef PHASE2
   if (strcmp(buffer,pshsd) == 0) {
    opt_id = 1;
    return(0);
   }
#endif
   if (strncmp(buffer,leay,(strlen(leay))) == 0) {
    opt_id = 2;
    strcpy(num_1,&buffer[6]);
#ifdef P18nov86
    i=j=0;
    while((work[j++]=num_1[i++]) != ',');
    work[--j]='\0';
    itoa(atoi(work)-(opt_level<<1),work);
    strcat(work,&num_1[--i]);
    strcpy(num_1,work);
#endif
    return(0);
   }
   if (strncmp(&buffer[6],jump7,(strlen(jump7))) == 0) {
    opt_id = 3;
    strncpy(save_jump,buffer,8);
    change_jump();
    return(0);
   }
#ifdef P15nov86
   if(strcmp(pulsd,buffer)==0) {
     opt_id=20;
     return 0;
   }
#endif
   if(strcmp(pshsx,buffer)==0) {
     opt_id=25;
     return 0;
   }
#ifdef P17nov86
   if (strncmp(buffer,lddnum,(strlen(lddnum))) == 0) {
     if(isdigit(buffer[7]) == 0) return 1; 
     opt_id=21;
     strcpy(num_1,&buffer[7]);
     return 0;  
   }
#endif
   if(strncmp(buffer,cmpd0,strlen(cmpd0)) == 0) {
     opt_id=23;
     return 0;
   }
   return(1);
  }
  case 2: {
#ifdef PHASE2
   if (opt_id == 1) {
    if (strncmp(buffer,ldd,(strlen(ldd))) == 0) {
     strcpy(num_1,&buffer[6]);
     return((buffer[6]=='#') || (isalpha(buffer[6])) ? 0 : 1);
    }
    else return(1);
   }
#endif
   if (opt_id == 2) return(strcmp(tfryd,buffer));
   if (opt_id == 3) return(strcmp(ldd0,buffer));
   if (opt_id == 25)
     if(strcmp(buffer,pulsx) == 0) {
       opt_end = -1;
       opt_id = -opt_id;
       return 0;
     }
     else return 1;
#ifdef P15nov86
   if (opt_id == 20) {
     if(strcmp(pshsd,buffer)==0) {
       opt_end = -1;
       return 0;
     }
     else return 1;
   }
#endif
#ifdef P17nov86
   if(opt_id==21) {
     if(strcmp(pshsd,buffer)==0) {
       i=0;
       while(num_1[i++] != '\n');
       num_1[--i] = 0;
       i=atoi(num_1)<<1;
       itoa(i,num_1);
       return 0;
     }
#endif
#ifdef P28nov86
     else if(strncmp(buffer,cmpd,strlen(cmpd))==0) {
       if(buffer[6]=='#' && isdigit(buffer[7])) {
         strcpy(num_2,&buffer[7]);
         opt_id=22;
         return 0;
       }
     }
     else if(strcmp(buffer,coma)==0) {
       opt_id=24;
       return 0;
     }
#endif
#ifdef P17nov86
   }
#endif
   if(opt_id==23) {
     opt_end = -1;
     if(strncmp(buffer,lb,strlen(lb))==0) {
       strcpy(num_1,buffer);
       return 0;
     }
     else return 1;
   }
   return(1);
  }
  case 3: {
#ifdef PHASE2
   if (opt_id == 1) {
    opt_end = -1;
    if (strcmp(buffer,adddsplus) == 0) {
     if(num_1[1]=='0') opt_id = -opt_id;
     strcpy(num_2,addd);
     return(0);
    }
#ifdef PHASE3
    if (strcmp(buffer,cmpdsplus) == 0) {
     strcpy(num_2,cmpd);
     return(0);
    }
    else
      return(1);
#endif
   }
#endif
   if(opt_id == 2) if(strcmp(buffer,pshsd)==0) {
#ifdef P18nov86
       i=j=0;
       while((num_3[j++]=num_1[i++]) != ',');
       num_3[--j] = '\0';
       wr_ok = 0;
       stackoffset[opt_level++] = atoi(num_3);
#endif
       return 0;
   }
   else return 1;
   if(opt_id == 3) return(strcmp(buffer,jump5));
   if(opt_id == 24) return(strcmp(buffer,comb));
#ifdef P17nov86
   if(opt_id == 21)
     if(strcmp(adddsplus,buffer) == 0) {
       opt_end=-1;
       return 0;
     }
#endif
#ifdef P28nov86
   if(opt_id == 22)
     if(strncmp(buffer,lbeqcc,strlen(lbeqcc))==0) {
       opt_end = -1;
       if(strcmp(num_1,num_2)) opt_id = -1;
       else {
         strcpy(num_1,jmp);
         strcpy(num_2,&buffer[6]);
       }
       return 0;
     }
     else if(strncmp(buffer,lbnecc,strlen(lbnecc))==0) {
       opt_end = -1;
       if(strcmp(num_1,num_2)==0) opt_id = -1;
       else {
         strcpy(num_1,jmp);
         strcpy(num_2,&buffer[6]);
       }
       return 0;
     }
#endif
   return(1);
  }
  case 4: {
   if (opt_id == 3) return(strcmp(buffer,ldd1));
   if(opt_id == 24) {
     opt_end = -1;
     return(strcmp(buffer,addd1));
   }
   if (opt_id == 2) {
    if (strcmp(buffer,lddsplus) == 0) {
     opt_id = 10;
#ifdef P18nov86
     --opt_level;
#endif
     opt_end = -1;
     return(0);
    }
#ifndef P18nov86
    if (strncmp(buffer,ldd,(strlen(ldd))) == 0) {
      if ((isdigit(buffer[7])) != 0) {
      strcpy(num_2,&buffer[6]);
      opt_id = 16;
      return(0);
     }
     else return(1);
    }
#endif
    if (strncmp(buffer,leay,(strlen(leay))) == 0) {
     strcpy(num_2,&buffer[6]);
     opt_id = 11;
     return(0);
    }
    if (strcmp(buffer,ldbsplus) == 0) {
     opt_id = 14;
#ifdef P18nov86
     --opt_level;
#endif
     opt_end = -1;
     return (0);
    }
#ifndef P18nov86
    if (strncmp(buffer,ldb,(strlen(ldb))) == 0) {
     if (isdigit(buffer[7]) != 0) {
        strcpy(num_2, &buffer[6]);
      opt_id = 12;
      return(0);
     }
     else return(1);
    }
#endif
    if (strcmp(buffer,pshsd) == 0) {
     opt_id = 13;
     return(0);
    }
/*
    if (strncmp(buffer,lbsr,(strlen(lbsr))) == 0) {
     opt_id = 15;
     strcpy(num_2,&buffer[6]);
     return(0);
    }
*/
   }
   return(1);
  }
  case 5: {
   if(opt_id == 3) return(strcmp(buffer,cmpd0));
   if(opt_id == 2) return(strcmp(buffer,pulsx));
   if(opt_id == 11) return(strcmp(buffer,tfryd));
#ifndef P18nov86
   if(/*opt_id == 15 ||*/ opt_id == 16) {
    if(strcmp(buffer,stdsplus) == 0) {
     opt_end = -1;
     return(0);
    }
    else return(1);
   }
   if(opt_id == 12)
    if(strcmp(buffer,stbsplus) == 0) {
     opt_end = -1;
     return(0);
    }
    else return(1);
#endif
   return(strcmp(buffer,lddsplus));
  }
  case 6: {
   if(opt_id == 11) return(strcmp(buffer,pshsd));
   if(opt_id == 13) {
    strcpy(num_2,&buffer[6]);
    if(strncmp(buffer,addd,(strlen(addd))) == 0) {
     opt_id = 17;
     return(0);
    }
    else return(strncmp(buffer,subd,(strlen(subd))));
   }
   opt_end = -1;
   if (opt_id == 2)
     if(strcmp(buffer,stdx)==0) {
#ifdef P18nov86
        --opt_level;
#endif
        return 0;
      }
      else return 1;
   if (opt_id == 3)
    if (strncmp(buffer,lbeqcc,(strlen(lbeqcc))) == 0) {
     i=6;
     while(buffer[i] != '\n')
       if(buffer[++i]=='_') {
         opt_id=18;
         break;
       }
     strcpy(num_2,&buffer[6]);
     return(0);
    }
   return(1);
  }
  case 7: {
   opt_end = -1;
   if (opt_id == 13 || opt_id == 17) {
#ifdef P18nov86
     --opt_level;
#endif
     return(strcmp(buffer,stdsplus));
   }
   return(1);
  }
  default: {
   fputs("\nCase error\n",stdout);
   exit(0);
  }
 }
}
change_jump()
/* change jump according to table */
{
 int counter, i;

 counter = 0;
   i = 2;
 while (jump_table[counter] != '\0') {
  if(jump_table[counter] == buffer[i] &
       jump_table[counter+1] == buffer[i+1]) {
   number[++i] = new_table[counter++];
     number[++i] = new_table[counter++];
    }
  else counter = counter + 2;
 }
}
