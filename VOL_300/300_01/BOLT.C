/*   HEADER:   CUG300;
      TITLE:   Bolt stress demonstration program;
       DATE:   4/20/1989;
DESCRIPTION:   "Calculates the stress in a series of bolts";
    VERSION:   1.0;
   FILENAME:   BOLT.C;
   SEE-ALSO:   MAT_V2D.H;
*/

   #include  "mat_v2d.h"
   #include  <math.h>
   #include  <stdio.h>

   main()
   {
	unsigned rows,cols,toksz,i;
   enum     {ID_,LOAD,DIAM,THRDS};
   enum     {ID,AREA,STRESS,OUTFLDS};
   FILE     *FN;
   struct   tmat *inp,*out;
   float    Net_Area,Stress;

/*-- Scan and input the data from a textfile (BOLT.INP) --------------*/

      mtcnt("BOLT.INP",&rows,&cols,&toksz);
      tdim(inp,rows,cols,toksz);         /*   Note that array size is */
      tdim(out,rows,OUTFLDS,toksz+10);   /*   based on textfile       */
      mtget("BOLT.INP",inp);             /*   contents                */

/*-- Compute desired parameters --------------------------------------*/

      rows(inp,i) {

/*    Note: Casts of enum index references to integer may be required for
      some compilers
*/
         Net_Area = 0.7854 * pow(tf(inp,i,DIAM,4),2) -     0.9743/
                                                     /*---------------*/
                                                      tf(inp,i,THRDS,4);
         Stress   =     tf(inp,i,LOAD,4)/
                       /*------------*/
                           Net_Area;

         strcpy( t(out,i,ID), t(inp,i,ID_)); /*  Transfer results to  */ 
         ft( out,i,AREA, Net_Area,30,3,4);   /*  the output dynamic   */
         ft( out,i,STRESS, Stress,30,3,4);   /*  text array           */
      }

/*-- Output the results array with appropriate column headings -------*/

      flcreat("BOLT.OUT",FN);
      mtput(FN, out, " Sample Area(sq_in) Stress(lbs/sq_in)",
                     " Sample Area(sq_in) Stress(lbs/sq_in)", 72, 3);
      fclose(FN);
   }


