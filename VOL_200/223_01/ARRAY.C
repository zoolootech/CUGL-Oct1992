/*	
** prog	array.c	  Array Test  Program	by F.A.Scacchitti  3/15/86
**		    Copyright 1986 	   25 Glenview Lane
**		  All rights reserved	   Roch, NY  14609
**
**		Written in Small-C  (Version 2.7)
**
**
**
**
**
*/

#include <stdio.h>

/*
** Global Variables
*/

int i2[2][2] = { 0, 1,
                 2, 3};

int i3[3][3][3] = {0, 1, 2,
                   3, 4, 5,
                   6, 7, 8,

                   9, 10, 11,
                   12, 13, 14,
                   15, 16, 17,

                   18, 19, 20,
                   21, 22, 23,
                   24, 25, 26};

int i,j,k;

char c2[2][2] = { 0, 1,
                  2, 3};

char c3[3][3] = {0, 1, 2,
                 3, 4, 5,
                 6, 7, 8};

char c4[4][4] = {0, 1, 2, 3,
                 4, 5, 6, 7,
                 8, 9, 10, 11,
                 12, 13, 14, 15};


main(argc,argv) int argc; char *argv[]; {

int li,lj,lk;

   for(li = 0; li <= 1; li++)
      for(lj = 0; lj <= 1; lj++)
         prntf("element %d = %d\n",li ,i2[li][lj]);


   prntf("element 0 0 = %d\n",i2[0][0]);
   prntf("element 0 1 = %d\n",i2[0][1]);
   prntf("element 1 0 = %d\n",i2[1][0]);
   prntf("element 1 1 = %d\n",i2[1][1]);


   for(i = 0; i <= 1; i++)
      for(j = 0; j<= 1; j++)
         prntf("element %d %d = %d\n",i ,j ,i2[i][j]);

   holdit();

   for(i = 0; i <= 1; i++)
      for(j = 0; j<= 1; j++)
         i2[i][j]=i*j;


   for(i = 0; i <= 1; i++)
      for(j = 0; j<= 1; j++)
         prntf("element %d %d = %d\n",i ,j ,i2[i][j]);

   holdit();

   prntf("\n\nelement 0 0 0 = %d\n", i3[0][0][0]);
   prntf("element 0 0 1 = %d\n", i3[0][0][1]);
   prntf("element 0 0 2 = %d\n", i3[0][0][2]);
   prntf("element 0 1 0 = %d\n", i3[0][1][0]);
   prntf("element 0 1 1 = %d\n", i3[0][1][1]);
   prntf("element 0 1 2 = %d\n", i3[0][1][2]);
   prntf("element 0 2 0 = %d\n", i3[0][2][0]);
   prntf("element 0 2 1 = %d\n", i3[0][2][1]);
   prntf("element 0 2 2 = %d\n", i3[0][2][2]);
   prntf("element 1 0 0 = %d\n", i3[1][0][0]);
   prntf("element 1 0 1 = %d\n", i3[1][0][1]);
   prntf("element 1 0 2 = %d\n", i3[1][0][2]);
   prntf("element 1 1 0 = %d\n", i3[1][1][0]);
   prntf("element 1 1 1 = %d\n", i3[1][1][1]);
   prntf("element 1 1 2 = %d\n", i3[1][1][2]);
   prntf("element 1 2 0 = %d\n", i3[1][2][0]);
   prntf("element 1 2 1 = %d\n", i3[1][2][1]);
   prntf("element 1 2 2 = %d\n", i3[1][2][2]);

   holdit();

   prntf("element 2 0 0 = %d\n", i3[2][0][0]);
   prntf("element 2 0 1 = %d\n", i3[2][0][1]);
   prntf("element 2 0 2 = %d\n", i3[2][0][2]);
   prntf("element 2 1 0 = %d\n", i3[2][1][0]);
   prntf("element 2 1 1 = %d\n", i3[2][1][1]);
   prntf("element 2 1 2 = %d\n", i3[2][1][2]);
   prntf("element 2 2 0 = %d\n", i3[2][2][0]);
   prntf("element 2 2 1 = %d\n", i3[2][2][1]);
   prntf("element 2 2 2 = %d\n", i3[2][2][2]);

   holdit();

   for(i = 0; i <= 2; i++)
      for(j = 0; j<= 2; j++)
         for(k = 0; k <= 2; k++)
            prntf("element %d %d %d = %d\n",k ,j ,i , i3[k][j][i]);

   holdit();

   for(i = 0; i <= 2; i++)
      for(j = 0; j<= 2; j++)
         for(k = 0; k <= 2; k++)
            i3[i][j][k] = k + 10*j + 100*i;

   for(i = 0; i <= 2; i++)
      for(j = 0; j<= 2; j++)
         for(k = 0; k <= 2; k++)
            prntf("element %d %d %d = %d\n",k ,j , i, i3[k][j][i]);



}

/*
** pause to enable evaluation
*/
holdit(){
   prntf("\n Depress [RETURN] to continue\n");
   getchar();
}

