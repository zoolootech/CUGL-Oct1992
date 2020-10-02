/****************************************************************
 *                     Execution Time Profiler                  *
 *                         By Robert Ramey                      *
 *                        21 December 1986                      *
 ****************************************************************/

/* This program is used to determine where another program is   */
/* spending its time.  To use it link in profile as the first   */
/* module in the program.  Then execute with PROFILE instead of */
/* the program name.  When program terminates there will be a   */
/* file on the current disk named PROFILE which will contain a  */
/* table of the number of timer interrupts that occurred in each*/
/* subroutine.  In my system, a timer interrupt is produced ten */
/* times per second.   Hence the number of seconds spent in each*/
/* function is approximately the number in the table divided by */
/* ten. */

/* Programs should function the same with or without profile.   */
/* The only difference is that when exit() is called,  windup() */
/* will be executed.  Only when windup() returns will exit()    */
/* be entered */

#define MAXSLOTS    256 /* maximum number of slots */
#define DATASEG (&numslots) /* last address in code segment */
#define CLOCKOFF    2   /* timer interrupt offset */
#define SYMSIZE 7   /* size to reserve for address symbol*/
#define JP  0xc3    /* jump instruction */

static int numslots,    /* number of subroutines monitored */
    high, low, middle;/* used in binary search of table */

#include <stdio.h>

typedef char *ADDRESS;
typedef struct {
    ADDRESS address;    /* address of slot */
    unsigned count;     /* number of ticks in this slot */
    char symbol[SYMSIZE];   /* name assigned to address */
    } HISTOGRAM ;

static ADDRESS
    clock,      /* address of timer interrupt handler */
    retaddr,    /* address at timer interrupt */
    warmstart,  /* address of warmstart routine */
    *iptr;      /* interrupt vector entry for timer */

static char oexit[3];   /* original 1st 3 bytes of exit() function */

static HISTOGRAM hist[MAXSLOTS];

int acompare();
ADDRESS *ir();
void tick(), exit(), windup();

/*****************************************************************
setup - make table of addresses and start processing timer
interrupts
******************************************************************/
void
setup()
{
    register int i;
    ADDRESS addr, *aptr;
    FILE *fp;
    HISTOGRAM tslot[MAXSLOTS], *tptr[MAXSLOTS];

    if(!(fp = fopen("PROFILE.SYM","r")))
        error("PROFILE.SYM does not exist");

    /* fill in array */
    strcpy(tslot[0].symbol, "PAGE0");
    tslot[0].address = 0;

    i = 1;
    /* with addresses from symbol table file */
    while(2 == fscanf(fp, "%x %s", &addr, tslot[i].symbol)){
        if(addr > DATASEG) continue;
        tslot[i].address = addr;
        if(++i == (MAXSLOTS - 2))
            error("Too many addresses");
    }
    fclose(fp);

    /* and with CP/M addresses */
    tslot[i].address = *(ADDRESS *)6;
    strcpy(tslot[i].symbol, "ZRDOS");
    tslot[++i].address = *(ADDRESS *)1;
    strcpy(tslot[i].symbol, "BIOS");

    /* make array of ptrs for use by qsort() */
    numslots = i + 1;
    for(;i >= 0; --i){
        tptr[i] = &tslot[i];
        tslot[i].count = 0;
    }
    /* sort addresses in ascending order */
    qsort(tptr, numslots, acompare);

    /* load into static array */
    for(i = numslots;--i >= 0;)
        memcpy(&hist[i], tptr[i], sizeof(HISTOGRAM));

    /* redirect exit() to windup() */
    memcpy(oexit, exit, 3); /*store original 3 bytes of exit()*/
    *(char *)exit = JP;
    *((char *)exit + 1) = windup;
    *((char *)exit + 2) = windup >> 8;

    /* redirect warmstart to windup just in case program aborts */
    aptr = (ADDRESS *)(*(ADDRESS *)1 + 1);
    warmstart = *aptr;
    *aptr = windup;

    /* redirect timer interrupts to tick() */
    iptr = ir() + CLOCKOFFSET;
    clock = *iptr;      /* save address of timer interrupt */
    *iptr = tick;       /* start counting interrupts */
}
/******************************************************************
windup - executes before normal Q/C exit routine.
    Write histogram to a file PROFILE on currently selected disk.
*******************************************************************/
static void
windup(exitarg)
int exitarg;
{
    register i;
    FILE *fp;
    ADDRESS *aptr;
    unsigned tcnt;
    HISTOGRAM *hptr;

    /* restore original contents of exit() */
    memcpy(exit, oexit, 3);
    *iptr = clock;      /* stop counting timer interrupts */
    aptr = (ADDRESS *)(*(ADDRESS *)1 + 1);
    *aptr = warmstart;  /* reload original warmstart */

    if(!(fp = fopen("profile", "w")))
        error("Can't open file for execution profile");

    /* calculate total number of ticks */
    tcnt = 0;
    for(i = numslots; --i >= 0;) tcnt += hist[i].count;

    /* write out profile to disk */
    for(hptr = hist,i = numslots; --i >= 0;++hptr){
        if(hptr->count)
            fprintf(fp,"%s\t%5d\t%3d\n"
            ,hptr->symbol,hptr->count
            ,(hptr->count * 200 + tcnt) / (2 * tcnt));
    }
    fclose(fp);

    /* continue on with normal exit processing */
    exit(exitarg);
}
/*****************************************************************
tick - receives control on timer interrupt
******************************************************************/
static void
tick()
{
    /* save registers which might be altered */
#asm
    push    hl      ;save hl for a second
    ld  hl,(clock?) ;push interrupt address on top of stack
    ex  (sp),hl     ;so that tick() will return to clock
    push    hl      ;save registers to be modified on stack
    push    de      ;Q/C does not modify BC in this case
    push    af
    ld  hl,8        ;get address of interrupted program
    add hl,sp       ;into hl
    call    ?g
    ld  (retaddr?),hl   ;store into static variable
    call    bump?       ;increment histogram
    pop af      ;restore registers
    pop de
    pop hl
#endasm
    /* continue with original interrupt handler */
}
/****************************************************************
bump - increment appropriate counter in histogam
*****************************************************************/
static void
bump()
{
    /* note: local variables should not be declared */
    /* these will make Q/C call a special routine */
    /* which will alter registers that have not been saved */

    /* binary search of address table */
    high = numslots;
    low = 0;
    while((high - low) > 1){
        middle = (high + low) / 2;
        if(retaddr >= hist[middle].address)
            low = middle;
        else
            high = middle;
    }
    ++hist[low].count;
}
/***************************************************************
acompare - compare two members of the table
****************************************************************/
static int
acompare(a, b)
HISTOGRAM *a, *b;
{
    register int i;
    if(a->address == b->address)
        return 0;
    else
        return a->address > b->address ? 1 : -1;
}
/***************************************************************
ir - get value of interrupt pointer register
****************************************************************/
static ADDRESS *
ir()
{
#asm
    ld  a,i ;load interrupt vector address
    ld  h,a
    in0 l,(033h);for the HD64180
#endasm
}
******/
static int
aco