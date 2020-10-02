
/*
     HEADER:     CUG270.08 ;
     TITLE:      TTT3D Move conversion subroutines ;
     DATE:       06/16/1988 ;
     VERSION:    1.0 ;
     FILENAME:   T3CONV.C ;
     SEE-ALSO:   T3.DOC ;
     AUTHORS:    Scott Holland ;
*/

/* COPYRIGHT 1988  SCOTT HOLLAND */

/* This routine converts a level, row, column input to a
   square between 0 ond 63 */
   
conv_move(l,r,c)
  int l,r,c ;
  {
      return( (r-1)*16+(l-1)*4+c-1 );
  }



/* This routine converts a move in the range 0 to 63 to
   a three digit move in the form level,row,column */

conv_to3(temp)
  int temp ;
  {
      int temp3,temp2,temp1 ;
      /* temp2 1st digit, temp3 2nd digit, temp1 3rd digit */
      temp3 = temp/16 ;
      temp1 = temp - temp3 * 16 ;
      temp2 = temp1/4 ;
      temp1 = temp1 - temp2 * 4 ;
      return( temp2*100+temp3*10+temp1+111 );
  }

