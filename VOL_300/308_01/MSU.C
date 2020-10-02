

/* The following are the various constants defined and
 * used in the simulation software for the MSU1 machine. 
 */

#include <stdio.h>
#include <ctype.h>

#define TRUE  1  /* boolean to represent the true 
                    value */
#define FALSE 0  /* boolean to represent the false
                    value */
#define READ  1  /* this is defined constant for the 
                    read operation */
#define WRITE 0  /* this is defined constant for the
                    write operation */ 
#define maxsize  4096  /* main memory size declared
                          as a constant */ 
#define maxint   2147483674  /* the maximum integer for 
                                32 bits computer */
#define file1 "msu.dat" /* define the input
                                         file name */
#define file2 "msu.out"  /* define the output
                                         file name */




/* The following global variables are used and defined
 * in the MSU1 software program.
 */

int rwsignal,    /* read action set to 1,
                    write action set to 0 */
    L0, base0,   /* the length & base address of 
                    the program */
    L1, base1,   /* the length & base address of the
                    input buffer */
    L2, base2,   /* the length & base address of the
                    output buffer */
    L3, base3,   /* the length & base address of the
                     workspace */
    jobid = 0,   /* for the job identification */
    clock, trace,  /* for variables clock, trace */
    FLAG = 1;      /* flag signal use boolean express */

FILE *in, *out;    /* input, output file pointers */
unsigned CHANNEL,       /* read or write address */
         MEM[maxsize];  /* declares the memory space */    
char c,             /* character read from input file */
     *mne,          /* stands for instruction name */
     *maskmode;     /* mask value from 0000 to 1111 */




/* The GI instruction format:
 *
 *  +--------+------+----------+------+----------+
 *  | opcode |  S1  |    D1    |  S2  |    D2    |
 *  +--------+------+----------+------+----------+
 *   31    26 25  24 23      13 12  11 10       0
 */


typedef struct instruction_tag1
{
  unsigned d2 : 11; /* displacement value of the 
                       operand2 */
  unsigned s2 :  2; /* origin register of the operand2 */
  unsigned d1 : 11; /* displacement value of the
                       operand1 */
  unsigned s1 :  2; /* origin register of the operand1 */
  unsigned op :  6; /* opcode */
};




/* The BI instruction format:
 *
 *  +--------+------+----------+------+----------+
 *  | opcode |  M   | not used |  S1  |    D1    |
 *  +--------+------+----------+------+----------+
 *   31    26 25  22 21      13 12  11 10       0
 */


typedef struct instruction_tag2
{
  unsigned d1   : 11; /* displacement value of the
                         operand1 */
  unsigned s1   :  2; /* origin register of the 
                         operand1 */
  unsigned nu   :  9; /* not used */
  unsigned mask :  4; /* mask bits used in branching
                         instruction */
  unsigned op   :  6; /* opcode */
};




/* The PSW format:
 *
 *  +------+------+------------------------------+
 *  |  CC  |  IC  |     program counter (PC)     |
 *  +------+------+------------------------------+
 *   31  28 27  24 23                           0
 */


typedef struct psw_tag
{
  unsigned pc : 24;     /* program counter */
  unsigned ic :  4;     /* interrupt code */
  unsigned cc :  4;     /* condition code */     
 };




/* The segment origin register format:
 * 
 *  +-------------------+------------------------+
 *  |    base address   |   length of segment    |
 *  +-------------------+------------------------+
 *   31               18 17                     0
 */


typedef struct reg
{
  unsigned length : 18;     /* length of segment */
  unsigned base   : 14;     /* base address */
};




union
{
  struct psw_tag index;
  unsigned value;
} PSW;     /* Processor Status Word */



union
{
  struct instruction_tag1 gi;
  struct instruction_tag2 bi;
  unsigned value;
} IR;      /* Instruction Register */




union
{
  struct reg seg;
  unsigned value;
} ciow[4];    /* code, input, output, workspace */




/*=====================================================*
 *                       main                          *
 *=====================================================*/
main()
{
  /* The main function call the other functions to 
   * open the input/output file. Also, when the _SYSTEM
   * return his job and return to main, the main 
   * functin will call file_close() to close all the
   * files which it has been open.
   */

  read_input_file();       /* open an input file   */
  write_output_file();     /* open an output file  */
  _SYSTEM();
  file_close();
}




/*====================================================*
 *                read_input_file                     *
 *====================================================*/
read_input_file()
{
  /* This function opens an input file. */

  extern FILE *in;
  extern char c;
  void _error();

  if ((in = fopen(file1,"r")) == NULL) {
     _error(1);
     exit(3);
  }
  else {
         c = getc(in);
         /* skip the space and end of line */
         while (isspace(c) || (c == '\n'))  
               c = getc(in);
         if (c == EOF) {
            _error(2);
            exit(4);
         }
  }
}




/*====================================================*
 *                write_output_file                   *
 *====================================================*/
write_output_file()
{
  /* This function opens a output file. */

  extern FILE *out;
  void _error();

  /* if can not open */
  if ((out = fopen(file2,"w")) == NULL) { 
     _error(3);
     exit(5);
  }
}




/*====================================================*
 *                     _SYSTEM                        *
 *====================================================*/
_SYSTEM()
{
  /* The _SYSTEM function act as the main driver in
   * MSU1 machine. The modules LOADER, MEMORY, CPU and
   * INTERRUPT are called from this function. This loops
   * repeats until all the jobs are finished.
   */

  extern int FLAG, job;
  extern char c;
  extern FILE *in, *out;
  int LOADER(), CPU(), INTERRUPT(), 
      nextjob(), io_operation(),
      card_check,  /* check the cardreader is finished
                      or not */
      startadd,    /* the beginning address of the 
                      executing program */
      end;         /* ensure the read/write action */
  short icmode;    /* interrupt condition code */
  void _error(), output_format(), listresult(), reset();

  if (c == 'I') {
     fscanf(in,"%04x %04x %04x %04x",&L0,&L1,&L2,&L3);
     card_check = TRUE;
  }
  else {
         _error(2);
         exit(11);
  }
  while (card_check) { /* until all jobs finish */
        jobid++;
        fprintf(out,"\n******************************");
        fprintf(out,"**************************\n\n");
        fprintf(out,">>>>>>>>>>       JOB ID %4x  ",
                jobid);
        fprintf(out,"COMMENCES       <<<<<<<<<<\n\n");
        LOADER(&startadd,&trace,&FLAG);
        if (FLAG) {   /* load input data has no error */
           if (trace) output_format();
           do {
                CPU(startadd,trace);
                icmode = PSW.index.ic;
                if (icmode == 1)   io_operation();
                end = INTERRUPT(icmode,&startadd);
           } while (end && FLAG);
           listresult();
           reset();
        }
        c = getc(in);
        while ((c != 'I') && (c != EOF))
              c = getc(in);
        card_check = nextjob();
  }  /* end of while */
}




/*====================================================*
 *                      LOADER                        *
 *====================================================*/
LOADER(beginadd,dump,OK)
int *beginadd,      /* starting address */
    *dump,          /* trace bit value */
    *OK;            /* FLAG value */
{
  /* The LOADER module is responsible for loading the
   * user program from simulated hardware module--
   * card reader, and stores it into main memory. If
   * any error being detected, then LOADER will return
   * to the calling program and list the error message.
   */

  extern char c;
  extern FILE *in;
  int  counter = 0,   /* code length variable */
       turn,  /* boolean value for digit or 'A/a to F/f' */
       STOP = FALSE,  /* while loop boolean variable */
       next,   /* true: read next char */
       /* variable name */
       remainder, temp = 0, data, value,
       i, j,   /* loop counter variable name */
       follow, /* trace bit value */
       flag = TRUE,   /* boolean variable */
       MEMORY();
  unsigned add_ptr = 0;  /* the next instruction 
                            address */
  void load_seg_reg(), _error();

  load_seg_reg();
  c = getc(in);
  while (isspace(c))  c = getc(in);
  if (isdigit(c))  turn = FALSE;
  else turn = TRUE;
  next = TRUE;
  while (!STOP)
    switch (turn) {
        case 0:
              if (next) {
                 temp = (c - '0') * 16;
                 fscanf(in,"%c",&c);
                 next = FALSE;
                 if (!isdigit(c))  turn = TRUE;
              }
              else {
                temp = temp + (c - '0');
                remainder = temp % 8;
                temp = temp / 8;
                if (remainder > 0) {
                   counter = counter + temp + 1;
                   temp += 1;
                }
                else  counter += temp;
                if (counter <= ciow[0].seg.length) {
                   for (i = 0; i < temp; i++) {
                       fscanf(in,"%08x",&data);
                       MEMORY(WRITE,add_ptr,&data);
                       ++add_ptr;
                   }
                   remainder = 0;
                   temp = 0;
                   c = getc(in);
                   while (isspace(c))  c = getc(in);
                   if (!isxdigit(c))  STOP = TRUE;
                   else if (!isdigit(c))  turn = TRUE;
                   next = TRUE;
                }
                else {
                       _error(10);
                       flag = FALSE;
                       STOP = TRUE;
                }
              }
              break;
        case 1: 
              if (next) {
                 switch (c) {
                   case 'A': 
                   case 'a':
                           temp = 16 * 10;
                           break;
                   case 'B':
                   case 'b':
                           temp = 16 * 11;
                           break;
                   case 'C':
                   case 'c':
                           temp = 16 * 12;
                           break;
                   case 'D':
                   case 'd':
                           temp = 16 * 13;
                           break;
                   case 'E':
                   case 'e':
                           temp = 16 * 14;
                           break;
                   case 'F':
                   case 'f':
                           temp = 16 * 15;
                           break;
                 }
                 c = getc(in);
                 next = FALSE;
                 if (isdigit(c))  turn = FALSE;
              }
              else {
                 switch (c) {
                    case 'A':
                    case 'a': 
                            temp += 10;
                            break;
                    case 'B':
                    case 'b':
                            temp += 11;
                            break;
                    case 'C':
                    case 'c':
                            temp += 12;
                            break;
                    case 'D':
                    case 'd':
                            temp += 13;
                            break;
                    case 'E':
                    case 'e':
                            temp += 14;
                            break;
                    case 'F':
                    case 'f':
                            temp += 15;
                            break;
                 }
                 remainder = temp % 8;
                 temp /= 8;
                 if (remainder > 0) {
                    counter = counter + temp + 1;
                    temp += 1;
                 }
                 else  counter += temp;
                 if (counter <= ciow[0].seg.length) {
                    for (i = 0; i < temp; i++) {
                        fscanf(in,"%08x",&data);
                        MEMORY(WRITE,add_ptr,&data);
                        ++add_ptr;
                    }
                    remainder = 0;
                    temp = 0;
                    fscanf(in,"%c",&c);
                    if (!isxdigit(c))  STOP = TRUE;
                    else if (isdigit(c))  turn = FALSE;
                    next = TRUE;
                 }
                 else {
                        _error(10);
                        flag = FALSE;
                        STOP = TRUE;
                 }
              }
              break;
    } /* end of switch */
  fscanf(in,"%c",&c);
  if (c == 'K')  fscanf(in,"%c",&c);
  if (flag) {
     fscanf(in,"%04x",&counter);
     add_ptr = ciow[3].seg.base;
     for (i = 0; i < counter; i++) {
         fscanf(in,"%04x %08x",&temp,&data);
         MEMORY(WRITE,add_ptr,&data);
         ++add_ptr;
     }
     fscanf(in,"%04x %d",&temp,&follow);
     *dump = follow;
     *beginadd = temp / 4;
     *OK = flag;
  } /* end of if */
  else *OK = FALSE;
}

           


/*====================================================*
 *                  load_seg_reg                      *
 *====================================================*/
void load_seg_reg()
{
  /* This function read the data, both base and length, 
   * and store in the code, input, output and workspace
   * segment origin register.
   */   

  extern FILE *in;
  extern int L0, base0, L1, base1, L2, base2,   
         L3, base3;   

  L0 = L0 >> 2;
  L1 = L1 >> 2;
  L2 = L2 >> 2;
  L3 = L3 >> 2;
  base0 = 0;
  base1 = L0;
  base2 = base1 + L1;
  base3 = base2 + L2;
  ciow[0].seg.base   = base0;
  ciow[0].seg.length = L0;
  ciow[1].seg.base   = base1;
  ciow[1].seg.length = L1;
  ciow[2].seg.base   = base2;
  ciow[2].seg.length = L2;
  ciow[3].seg.base   = base3;
  ciow[3].seg.length = L3;
}




/*====================================================*
 *                      MEMORY                        *
 *====================================================*/
MEMORY(type,address,data)
int type,
    *data;
unsigned address;
{
  /* The memory function performs the read/write
   * operation, and extracts/stores data in main
   * memory.
   */   

  extern unsigned MEM[maxsize];
  
  switch (type) {
         case WRITE : MEM[address] = *data;
                      break;
         case READ  : *data = MEM[address];
                      break;
  }
}




/*====================================================*
 *                       CPU                          *
 *====================================================*/
CPU(address,trace)
int address, trace;
{
  /* The CPU function will loop indefinitely fetching,
   * decoding, and executing instructions until a HLT
   * instruction or other exceptional condition occurs.
   */   

  int RUN = TRUE,
      contents,
      MEMORY(), determine_mne(), compare(), multiply(),
      move(), readdata(), substraction(), writedata(),
      branch();
  unsigned maskbit, PC;
  short opcode;
  void dump(); 

  PSW.value = (unsigned) address;
  while (RUN) {
        PC = PSW.index.pc;
        /* fetch instruction */
        MEMORY(READ,PC,&contents); 
        PSW.index.pc += 1;     /* increment PC */
        IR.value = contents;          
        opcode = IR.gi.op;
        determine_mne();  /* find the mne string name */
        if (trace)   dump();
        switch (opcode) { /* executes the instruction */
               case 0  : 
                       break;
               case 1  :
                       add();
                       break;
               case 4  :
                       move();
                       break;
               case 11 :
                       branch();
                       break;
               case 24 :
                       multiply();
                       break;
               case 27 :
                       readdata();
                       break;
               case 33 :
                       substraction();
                       break;
               case 42 :
                       compare();
                       break;
               case 51 :
                       writedata();
                       break;
               case 63 :
                       PSW.index.ic = 4;
                       break;
               default :
                       PSW.index.ic = 5;
                       break;
        } /* end of switch */
        if ((PSW.index.ic != 0) && (PSW.index.ic != 2))
           RUN = FALSE;
  } /* end of while */
}




/*====================================================*
 *                 output_format                      *
 *====================================================*/
void output_format()
{
  extern FILE *out;

  fprintf(out,">>>>>>>>>>            MEMORY DUMP     ");
  fprintf(out,"        <<<<<<<<<<\n\n");
  fprintf(out,"COMMENTS : THE VALUE OF OPERAND1'S");
  fprintf(out,"/OPERAND2'S IS IT'S\n");
  fprintf(out,"           CONTENTS OF MEMORY OF THE");
  fprintf(out," EFFECTIVE ADDRESS\n\n");
  fprintf(out,"   PSW       IR    TYPE MNE   ");
  fprintf(out,"OP1       OP2       CLOCK\n\n");
}




/*====================================================*
 *                  determine_mne                     *
 *====================================================*/
determine_mne()
{
  /* use the opcode to distinguish the instruction 
   * name. 
   */

  extern char *mne;
  extern int clock;
  short opcode;

  opcode = IR.gi.op;
  switch (opcode) {    /* increment clock */
         case 0  :
                 clock += 4;
                 mne = "NOP";
                 break;
         case 1  :
                 clock += 4;
                 mne = "ADD";
                 break;
         case 4  :
                 clock += 4;
                 mne = "MOV";
                 break;
         case 11 :
                 clock += 1;
                 determine_maskmode();
                 mne = "BC ";
                 break;
         case 24 :
                 clock += 4;
                 mne = "MLT";
                 break;
         case 27 :
                 clock += 4;
                 mne = "RD ";
                 break;
         case 33 :
                 clock += 4;
                 mne = "SUB";
                 break;
         case 42 :
                 clock += 4;
                 mne = "CMP";
                 break;
         case 51 :
                 clock += 4;
                 mne = "WR ";
                 break;
         case 63 :
                 clock += 4;
                 mne = "HLT";
                 break;
         default :
                 clock += 4;
                 mne = "   ";
                 break;
  } /* end of switch */
}




/*====================================================*
 *                       dump                         *
 *====================================================*/
void dump()
{
  /* print out the contents */
  
  extern int clock;
  extern FILE *out;
  extern char *mne;
  short opcode;
  unsigned operand1, operand2, temp1, temp2;
  int MEMORY();
  char *type1 = "GI",
       *type2 = "BI";
  static char *blank = "         ";

  opcode = IR.gi.op;
  operand1 = ciow[IR.gi.s1].seg.base + (IR.gi.d1 / 4);
  operand2 = ciow[IR.gi.s2].seg.base + (IR.gi.d2 / 4);
  MEMORY(READ,operand1,&temp1);
  MEMORY(READ,operand2,&temp2);
  PSW.index.pc = PSW.index.pc * 4;
  if (opcode != 11) {
     switch (opcode) {
         case  0  :
         case  63 :
                  fprintf(out,"%08x  %08x  %s  %s ",
                          PSW.value,IR.value,type1,mne);
                  fprintf(out,"%s  %s%08x\n",blank,
                          blank,clock);
                  break;
         case  27 :
         case  51 :
                  fprintf(out,"%08x  %08x  %s  %s ",
                          PSW.value,IR.value,type1,mne);
                  fprintf(out,"%s %08x  %08x\n",
                          blank,temp2,clock);
                  break;
         case  1  :  
         case  4  :
         case  24 :
         case  33 :
         case  42 :
                  fprintf(out,"%08x  %08x  %s  %s ",
                          PSW.value,IR.value,type1,mne);
                  fprintf(out,"%08x  %08x  %08x\n",
                          temp1,temp2,clock);
                  break;
     } /* end of switch */
  }
  else { 
         operand1 = ciow[IR.bi.s1].seg.base +
                    (IR.bi.d1 / 4);
         MEMORY(READ,operand1,&temp1);
         fprintf(out,"%08x  %08x  %s  %s ",PSW.value,
                 IR.value,type2,mne);
         fprintf(out,"%08x     %s %08x\n",
                 temp1,maskmode,clock);
  }
  PSW.index.pc = PSW.index.pc / 4;
}




/*====================================================*
 *               determine_maskmode                   *
 *====================================================*/
determine_maskmode()
{
  extern char *maskmode;
  unsigned maskbit;

  maskbit = IR.bi.mask;
  switch (maskbit) {
         case 0  : 
                 maskmode = " 0000 ";
                 break;
         case 1  :
                 maskmode = " 0001 ";
                 break;
         case 2  :
                 maskmode = " 0010 ";
                 break;
         case 3  :
                 maskmode = " 0011 ";
                 break;
         case 4  :
                 maskmode = " 0100 ";
                 break;
         case 5  :
                 maskmode = " 0101 ";
                 break;
         case 6  :
                 maskmode = " 0110 ";
                 break;
         case 7  :
                 maskmode = " 0111 ";
                 break;
         case 8  :
                 maskmode = " 1000 ";
                 break;
         case 9  :
                 maskmode = " 1001 ";
                 break;
         case 10 :
                 maskmode = " 1010 ";
                 break;
         case 11 :
                 maskmode = " 1011 ";
                 break;
         case 12 :
                 maskmode = " 1100 ";
                 break;
         case 13 :
                 maskmode = " 1101 ";
                 break;
         case 14 :
                 maskmode = " 1110 ";
                 break;
         case 15 :
                 maskmode = " 1111 ";
                 break;
  } /* end of switch */
}




/*====================================================*
 *                       add                          *
 *====================================================*/
add()
{
  /* The add function simulates the ADD instruction
   * operation.
   */

  int value1, value2, flag = TRUE, temp, MEMORY(),
      check(), condition_code();
  unsigned operand1, operand2;

  operand1 = ciow[IR.gi.s1].seg.base + (IR.gi.d1 / 4);
  operand2 = ciow[IR.gi.s2].seg.base + (IR.gi.d2 / 4);
  check(&flag);
  if (flag) {
     MEMORY(READ,operand1,&value1);
     MEMORY(READ,operand2,&value2);
     temp = value1 + value2;
     MEMORY(WRITE,operand1,&temp);
     condition_code(temp);
  }
}




/*====================================================*
 *                     multiply                       *
 *====================================================*/
multiply()
{
  /* The multiply function simulates the MLT 
   * instruction operation.
   */

  int value1, value2,
      flag = TRUE,
      MEMORY(), check(), condition_code(),
      temp;
  unsigned operand1, operand2;
  
  operand1 = ciow[IR.gi.s1].seg.base + (IR.gi.d1 / 4);
  operand2 = ciow[IR.gi.s2].seg.base + (IR.gi.d2 / 4);
  check(&flag);
  if (flag) {
     MEMORY(READ,operand1,&value1);
     MEMORY(READ,operand2,&value2);
     temp = value1 * value2;
     MEMORY(WRITE,operand1,&temp);
     condition_code(temp);
  }
}




/*====================================================*
 *                      compare                       *
 *====================================================*/
compare()
{
  /* The compare function simulates the CMP instruction
   * operation. 
   */

  int MEMORY(), check(), condition_code(), value1,
      value2, flag = TRUE;
  unsigned operand1, operand2;
 
  operand1 = ciow[IR.gi.s1].seg.base + (IR.gi.d1 / 4);
  operand2 = ciow[IR.gi.s2].seg.base + (IR.gi.d2 / 4);
  check(&flag);
  if (flag) {
     MEMORY(READ,operand1,&value1);
     MEMORY(READ,operand2,&value2);
     if (value1 == value2)  PSW.index.cc = 8;
     else if (value1 < value2)  PSW.index.cc = 4;
          else  PSW.index.cc = 2;
  }
}




/*====================================================*
 *                       move                         *
 *====================================================*/
move()
{
  /* The move function simulates the MOV instruction
   * operation.
   */

  int check(), MEMORY(), condition_code(), value2,
      flag = TRUE;
  unsigned operand1, operand2;

  operand1 = ciow[IR.gi.s1].seg.base + (IR.gi.d1 / 4);
  operand2 = ciow[IR.gi.s2].seg.base + (IR.gi.d2 / 4);
  check(&flag);
  if (flag) {
     MEMORY(READ,operand2,&value2);
     MEMORY(WRITE,operand1,&value2);
     condition_code(value2);
  }
}




/*====================================================*
 *                    readdata                        *
 *====================================================*/
readdata()
{
  /* The readdata function simulates the RD 
   * instruction operation. 
   */

  extern int FLAG, rwsignal;
  extern unsigned CHANNEL;
  int flag = TRUE;
  unsigned operand2, bound2;

  operand2 = ciow[IR.gi.s2].seg.base * 4 + IR.gi.d2;
  bound2 = ciow[IR.gi.s2].seg.length * 4;
  if (IR.gi.d2 > bound2-1) {
     FLAG = flag = FALSE;
     PSW.index.ic = 7;
  }
  if ((operand2 % 4) != 0) {
     PSW.index.ic = 3;
     FLAG = flag = FALSE;
  }
  if (flag) {
     CHANNEL = operand2 / 4;
     rwsignal = 1;
     PSW.index.ic = 1;
  }
}




/*====================================================*
 *                   substraction                     *
 *====================================================*/
substraction()
{
  /* The substraction function simulated the SUB
   * instruction operation. 
   */

  int check(), MEMORY(), condition_code(), value1, 
      value2, flag = TRUE, temp;
  unsigned operand1, operand2;
   
  operand1 = ciow[IR.gi.s1].seg.base + (IR.gi.d1 / 4);
  operand2 = ciow[IR.gi.s2].seg.base + (IR.gi.d2 / 4);
  check(&flag);
  if (flag) {
     MEMORY(READ,operand1,&value1);
     MEMORY(READ,operand2,&value2);
     temp = value1 - value2;
     MEMORY(WRITE,operand1,&temp);
     condition_code(temp);
  }
}




/*====================================================*
 *                    writedata                       *
 *====================================================*/
writedata()
{
  /* The writedata function simulates the WR 
   * instruction operation.
   */

  extern int FLAG, rwsignal;   
  extern unsigned CHANNEL;
  int flag = TRUE;
  unsigned operand2, bound2;

  operand2 = ciow[IR.gi.s2].seg.base * 4 + IR.gi.d2;
  bound2 = ciow[IR.gi.s2].seg.length * 4;
  if (IR.gi.d2 > bound2-1) {
     FLAG = flag = FALSE;
     PSW.index.ic = 7;
  }
  if ((operand2 % 4) != 0) {
     PSW.index.ic = 3;
     FLAG = flag = FALSE;
  }
  if (flag) {
     CHANNEL = operand2 / 4;
     rwsignal = 0;
     PSW.index.ic = 1;
  }  
}




/*====================================================*
 *                      branch                        *
 *====================================================*/
branch()
{
  /* The branch function simulates the BC 
   * instruction operation. 
   */

  extern int FLAG;
  unsigned bound1, operand1, maskbit;
  int flag = TRUE, temp;

  operand1 = ciow[IR.bi.s1].seg.base * 4 + IR.bi.d1;
  maskbit  = IR.bi.mask;
  bound1 = ciow[IR.bi.s1].seg.length * 4;
  if (IR.bi.d1 > bound1-1) {
     FLAG = flag = FALSE;
     PSW.index.ic = 7;
  }
  if ((operand1 % 4) != 0) {
     PSW.index.ic = 3;
     FLAG = flag = FALSE;
  }
  if (flag)
     if ((PSW.index.cc & maskbit) != 0)
        PSW.index.pc = operand1 / 4;
}




/*====================================================*
 *                      check                         * 
 *====================================================*/
check(flag)
int *flag;
{ 
  /* This function is used to check the memory address
   * reference whether is Boundary fault or Segment 
   * address fault. 
   */

  extern int FLAG;
  unsigned operand1, operand2,
           bound1,       /* buffer edge */
           bound2;       /* buffer edge */

  operand1 = ciow[IR.gi.s1].seg.base * 4 + IR.gi.d1;
  operand2 = ciow[IR.gi.s2].seg.base * 4 + IR.gi.d2;
  bound1 = ciow[IR.gi.s1].seg.length * 4;
  bound2 = ciow[IR.gi.s2].seg.length * 4;
  if ((IR.gi.d1 > bound1-1) && 
      (IR.gi.d2 > bound2-1)) {
     *flag = FALSE;
     PSW.index.ic = 7;
  }
  if (((operand1 % 4) != 0) || 
      ((operand2 % 4) != 0)) {
     PSW.index.ic = 3;
     FLAG = *flag = FALSE;
  }
}




/*====================================================*
 *                 condition_code                     *
 *====================================================*/
condition_code(value)
unsigned value;
{
  /* This function is used to check the condition
   * code and set the bit of PSW.index.cc if the 
   * condition is true.  
   */

  if (value == 0)  
     PSW.index.cc = 8;
  else if (value < 0)
          PSW.index.cc = 4;
       else if (value > maxint)
               PSW.index.cc = 1;
            else  PSW.index.cc = 2;
}




/*====================================================*
 *                    io_operation                    *
 *====================================================*/
io_operation()
{
  /* This function performs i/o operation: read data
   * from card reader or write data to the printer.
   */

  extern FILE *in, *out;
  extern int rwsignal;
  extern unsigned CHANNEL;
  int temp;

  if (rwsignal) {   /* read action */
     /* read from the card reader */
     fscanf(in,"%08x",&temp);
     /* put into the input buffer */
     MEMORY(WRITE,CHANNEL,&temp);
  }
  else {   /* write action */
    /* read data from the output buffer */
    MEMORY(READ,CHANNEL,&temp);
    /* write data into the line printer */
    fprintf(out,"\nLINE PRINTER :");
    fprintf(out," %08x\n\n",temp);
  }
}




/*====================================================*
 *                    INTERRUPT                       *
 *====================================================*/
int INTERRUPT(icmode,startadd)
short icmode;
int   *startadd;
{
  /* The INTERRUPT function serrves several 
   * conditions. When an interrupt fault is recognize
   * by the CPU, the IC field of the PSW is loaded with
   * the appropriate interrupt code and control returns
   * to the _SYSTEM module.
   */

  extern int clock, jobid;
  extern FILE *out;
  int finish = 0;
  void _error();

  switch (icmode) {
     case 1 :
            clock += 16;
            finish = 1;
            break;
     case 4 :
            fprintf(out,"\nJOB ID %d FINISHED ===>",jobid);
            fprintf(out,"  NORMAL TERMINATE\n\n");
            break;
     case 3 :
     case 5 :
     case 6 :
     case 7 :
            _error(5);
            break;
     default :
             _error(6);
             break;
  } /* end of switch */
  *startadd = PSW.index.pc;
  return finish;
}




/*====================================================*
 *                    listresult                      *
 *====================================================*/
void listresult()
{
  /* This routine will print the result of program's
   * execution or print error message by a normal
   * or an abnormal way.
   */

  extern int clock;
  void _error();

  switch (PSW.index.ic) {
         case 3  :
                 _error(4);
                 break;
         case 4  :
                 break;
         case 5  :
                 _error(8);
                 break;
         case 6  :
                 _error(7);
                 break;
         case 7  :
                 _error(9);
                 break;
  } /* end of switch */
  fprintf(out,"THE SPENDING OF CLOCK TIME UNITS ===> ");
  fprintf(out,"%08x \n",clock);
}




/*====================================================*
 *                      reset                         *
 *====================================================*/
void reset()
{
  /* This routine will clear the clock, trace, PSW,
   * IR, and origin instruction registers value.
   */

  extern int clock, trace, FLAG;
  int i;

  clock = 0;
  trace = 0;
  PSW.value = 0;
  IR.value  = 0;
  for (i = 0; i < 4; i++)
      ciow[i].value = 0;
  FLAG = 1;
}




/*====================================================*
 *                     _error                         *
 *====================================================*/
void _error(n)
int n;
{
  /* print out the error message */

  switch (n) {
     case 1 : 
            fprintf(out,"\nERROR_1: CAN NOT OPEN");
            fprintf(out," THE INPUT FILE.\n\n");
            break;
     case 2 :
            fprintf(out,"\nERROR_2: THE INPUT FILE");
            fprintf(out," IS EMPTY.\n\n");
            break;
     case 3 :
            fprintf(out,"\nERROR_3: CAN NOT OPEN THE");
            fprintf(out," OUTPUT FILE.\n\n");
            break;
     case 4 :
            fprintf(out,"\nERROR_4: BOUNDARY");
            fprintf(out," FAULT.\n\n");
            break;
     case 5 :
            fprintf(out,"\nERROR_5: JOB ID %d ",jobid);
            fprintf(out,"FINISHED ===>  ");
            fprintf(out,"  ABNORMAL TERMAINATE.\n\n");
             break;
     case 6 :
            fprintf(out,"\nERROR_6: NO SUCH CASE OF");
            fprintf(out," EXCEPTION.\n\n");
            break;
     case 7 :
            fprintf(out,"\nERROR_7: ATTEMPT TO DIVIDE");
            fprintf(out," BY ZERO.\n\n");
            break;
     case 8 :
            fprintf(out,"\nERROR_8: INVALID OPCODE.");
            fprintf(out,"\n\n");
            break;
     case 9 :
            fprintf(out,"\nERROR_9: SEGMENT ADDRESS");
            fprintf(out," FAULT.\n\n");
            break;
     case 10 : 
             fprintf(out,"\nERROR_10: INPUT CODE");
             fprintf(out," LENGTH IS TOO BIG.\n\n");
  } /* end of switch */
}




/*====================================================*
 *                    nextjob                         *
 *====================================================*/
int nextjob()
{
  /* This routine will read the code, input, output
   * and workspace information or EOF, and return value
   * 1 to the calling program if there is another job
   * available.
   */

  extern char c;
  extern FILE *in;
  int finish1 = FALSE,
      finish = FALSE,
      temp,
      data;

  while (!finish) {
        if (c != EOF) {
           fscanf(in," %04x %04x %04x %04x",&L0,&L1,&L2,&L3);
           finish = TRUE;
           finish1 = FALSE;
        }
        else {
               finish = TRUE;
               finish1 = TRUE;
        }
  } /* end of while */
  if (finish && finish1)
     return FALSE;
  else  return TRUE;
}




/*====================================================*
 *                    file_close                      *
 *====================================================*/
file_close()
{
  extern FILE *in, *out;

  fprintf(out,"\n\n\n<<<<<<<<<<<<<           ");
  fprintf(out,"   END             >>>>>>>>>>>>>");
  fprintf(out,"\n*****************************");
  fprintf(out,"***************************\n\n");
  fclose(in);
  fclose(out);
}



