/**************************************************************/
/*							      */
/*  NAME						      */
/*							      */
/*	 locate - position cursor on screen		      */
/*							      */
/*  SYNOPSIS						      */
/*							      */
/*	 locate(row,col);				      */
/*	 short row;	    row to place cursor on	      */
/*	 short col;	    column to place cursor on	      */
/*							      */
/*  DESCRIPTION 					      */
/*							      */
/*	 This function directly controls the cursor	      */
/*	 placing it on a specified row and column.  It	      */
/*	 uses the "csysint" C-to-assembler interface.         */
/*							      */
/*  RETURNS						      */
/*							      */
/*	 There are no return codes.			      */
/*							      */
/*  CAUTIONS						      */
/*							      */
/*	 The arguements row and col are limited by the	      */
/*	 terminal mode (i.e., 24x40, 24x80) and are in	      */
/*	 the range 1-24 and 1-40 or 1-80 respectively.	      */
/*	 This routine does not check the arguements.	      */
/*							      */
/**************************************************************/
locate(row,col)
short row,col;
{
     struct regset {
	  char al;
	  char ah;
	  char bl;
	  char bh;
	  char cl;
	  char ch;
	  char dl;
	  char dh;
     };
     unsigned csysint();
     struct regset sreg;     /* registers passed to csysint */
     struct regset rreg;     /* registers returned from cysint */
     unsigned flags;	     /* machine status register */
     int interrupt;	     /* type of interrupt requested */

     interrupt = 0x10;
     sreg.ah = 0x02;
     sreg.al = 0;
     sreg.bl = 0;
     sreg.bh = 0;
     sreg.cl = 0;
     sreg.ch = 0;
     sreg.dh = row;
     sreg.dl = col;

     flags = csysint(interrupt,&sreg,&rreg);
}
/* routine to test the locate command  ---- commented out
main()
{
     short row,col;

     cls();		     /* clear the screen */

     row = 4; col = 20;
     locate(row,col);
     cprintf("Should be row 4, column 20.\n");

     locate(15,30);
     cprintf("Should be row 15, columen 30.\n");
}
	--- end of commented code to test the locate command */
