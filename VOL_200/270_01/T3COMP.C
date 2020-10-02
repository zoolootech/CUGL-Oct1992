/*
     HEADER:     CUG270.06 ;
     TITLE:      TTT3D computer move subroutine ;
     DATE:       06/08/1988 ;
     VERSION:    1.0 ;
     FILENAME:   T3COMP.C ;
     AUTHORS:    Scott Holland ;
     SEE-ALSO:   T3.DOC ;
*/

/* COPYRIGHT 1988  SCOTT HOLLAND */

#include <stdio.h>
#include <stdlib.h>
#include "t3global.h"
#define P 0
#define C 1

computer_move()
{
  int i,j,k,l,m,n,o,p,q,r,s ;
  int sum, start ;
  int sum_list[2][4][76];
  if (game_over) return ;
  for (i=0;i<4;i++)
  {
    for (j=0;j<2;j++) sum_count[j][i] = 0 ;
  }
  strategy = 15 ;
  for (i=0;i<76;i++)
  {
    /* calculate sum of each potential winning row */
    sum  = board[ win_square[i][0] ] +
           board[ win_square[i][1] ] +
	   board[ win_square[i][2] ] +
	   board[ win_square[i][3] ] ;
    switch (sum)
    {
      case 0 :
      case 1 :
      case 2 :
      case 3 :
	/* Player sums */
	sum_list[P][sum][ sum_count[P][sum]++ ] = i ;
	break ;
      case 5 :
      case 10 :
      case 15 :
	/* Computer sums */
	sum = sum/5 ;
	sum_list[C][sum][ sum_count[C][sum]++ ] = i ;
	break ;
      case 4 :
	/* Player wins */
	strategy = 1 ;
	board[ win_square[i][0] ] = 2 ;
	board[ win_square[i][1] ] = 2 ;
	board[ win_square[i][2] ] = 2 ;
	board[ win_square[i][3] ] = 2 ;
	game_over = 1 ;
	return ;
    }
  } /* End for loop */
	
  if (diag) printf("1");
  
  if ( sum_count[C][3] > 0 )
  {
    strategy = 2 ; /* Computer wins */
    i = sum_list[C][3][0] ;
    board[ win_square[i][0] ] = 2 ;
    board[ win_square[i][1] ] = 2 ;
    board[ win_square[i][2] ] = 2 ;
    board[ win_square[i][3] ] = 2 ;
    game_over = 1 ;
    return ;
  }

  if (diag) printf("2");
  
  for (i=0;i<sum_count[P][3];i++)
  /* Computer checks for 3 in row for player */
  {
    k = sum_list[P][3][i] ;
    for (j=0;j<4;j++) /* Look for empty square */
    {
      if ( board[ win_square[k][j] ] == 0 )
      {
	/* Block 3 in row for player */
	move = win_square[k][j] ;
	board[move] = 5 ;
	strategy = 3 ;
	return ;
      }
    }
  } /* End i for loop */
	  
  if (diag) printf("3");
  
  /* For computer */
  /* Check if two intersecting rows with two in a row */
  for (i=0;i<sum_count[C][2]-1;i++)
  {
    for (j=i+1;j<sum_count[C][2];j++)
    {
      /* 2 rows - 2 in row */
      /* now find empty intersecting square */
      k = sum_list[C][2][i] ; /* win_square index */
      l = sum_list[C][2][j] ; /* win_square index */
      for (m=0;m<4;m++)
      {
	if (board [ win_square[k][m] ] ==0 )
	{
	  for (n=0;n<4;n++)
	  {
	    if (win_square[k][m]== win_square[l][n] )
	    {
	      move = win_square[k][m] ;
	      board[move] = 5;
	      strategy = 4;
	      return ;
	    }
	  }
	}
      }
    }
  }
	      
  if (diag) printf("4");
  
	      
  /* For player */
  /* Check if two intersecting rows with two in a row */
  for (i=0;i<sum_count[P][2]-1;i++)
  {
    for (j=i+1;j<sum_count[P][2];j++)
    {
      /* 2 rows - 2 in row */
      /* now find empty intersecting square */
      k = sum_list[P][2][i] ; /* win_square index */
      l = sum_list[P][2][j] ; /* win_square index */
      for (m=0;m<4;m++)
      {
	if (board [ win_square[k][m] ] ==0 )
	{
	  for (n=0;n<4;n++)
	  {
	    if (win_square[k][m]== win_square[l][n] )
	    {
	      move = win_square[k][m] ;
	      board[move] = 5;
	      strategy = 5;
	      return ;
	    }
	  }
	}
      }
    }
  }
	      
  if (diag) printf("5");
  
	      
  /* For computer */
  /* Check for a row with 2 intersecting
     a row with 1 intersecting a different row with 2
     with blank squares as intersecting squares */
  for (i=0;i<sum_count[C][2]-1;i++)
  {
    for (j=i+1;j<sum_count[C][2];j++)
    {
      /* 2 rows - 2 in row */
      k = sum_list[C][2][i] ; /* win_square index */
      l = sum_list[C][2][j] ; /* win_square index */
      for (m=0;m<sum_count[C][1];m++)
      {
	/* a row - 1 in row */
	/* now find 2 empty intersecting squares */
	n = sum_list[C][1][m] ; /* win_square index */
	for (o=0;o<4;o++)
	{
	  if (board [ win_square[n][o] ] ==0 )
	  {  /* square empty */
	    for (p=0;p<4;p++)
	    {
	      if (win_square[n][o]== win_square[l][p] )
	      { /* 2 intersects 1 in blank square */
		for (q=0;q<4;q++)
		{
		  if (board [ win_square[n][q] ] ==0 && q != p )
		  {  /* square empty in 1 in row */
		    for (r=0;r<4;r++)
		    {
		      if (win_square[n][q] == win_square[k][r] )
		      {
			 move = win_square[k][r] ;
			 move2 = win_square[l][p] ;
			 board[move] = 5;
			 strategy = 6;
			 return ;
		       }
		     }
		   }
		 }
	       }
             }
           }
	 }
       }
     }
   }
      
   if (diag) printf("6");
   

   /* For player */
   /* Check for a row with 2 intersecting
      a row with 1 intersecting a different row with 2
      with blank squares as intersecting squares */
   for (i=0;i<sum_count[P][2]-1;i++)
   {
     for (j=i+1;j<sum_count[P][2];j++)
     {
       /* 2 rows - 2 in row */
       k = sum_list[P][2][i] ; /* win_square index */
       l = sum_list[P][2][j] ; /* win_square index */
       for (m=0;m<sum_count[P][1];m++)
       {
	 /* a row - 1 in row */
	 /* now find 2 empty intersecting squares */
	 n = sum_list[P][1][m] ; /* win_square index */
	 for (o=0;o<4;o++)
	 {
	   if (board [ win_square[n][o] ] ==0 )
	   {  /* square empty */
	     for (p=0;p<4;p++)
	     {
	       if (win_square[n][o]== win_square[l][p] )
	       { /* 2 intersects 1 in blank square */
		 for (q=0;q<4;q++)
		 {
		   if (board [ win_square[n][q] ] ==0 && q != p )
		   {  /* square empty in 1 in row */
		      for (r=0;r<4;r++)
		      {
			if (win_square[n][q] == win_square[k][r] )
			{
			  move = win_square[k][r] ;
			  move2 = win_square[l][p] ;
			  board[move] = 5;
			  strategy = 7;
			  return ;
			}
		      }
		    }
		  }
		}
              }
            }
	  }
	}
      }
    }
      
    if (diag) printf("7");
   

  /* For computer */
  /* Check for a row with 2 intersecting
     a row with 0 intersecting a different row with 2
     with blank squares as intersecting squares   
     take a non-intersecting square in the 0 row */
  for (i=0;i<sum_count[C][2]-1;i++)
  {
    for (j=i+1;j<sum_count[C][2];j++)
    {
      /* 2 rows - 2 in row */
      k = sum_list[C][2][i] ; /* win_square index */
      l = sum_list[C][2][j] ; /* win_square index */
      for (m=0;m<sum_count[P][0];m++)
      {
	/* a row - 0 in row */
	/* now find 2 empty intersecting squares */
	n = sum_list[P][0][m] ; /* win_square index */
        /* n is row with 0 in a row */
	for (o=0;o<4;o++)
	{
	  if (board [ win_square[n][o] ] ==0 )
	  {  /* square empty */
	    for (p=0;p<4;p++)
	    {
	      if (win_square[n][o]== win_square[l][p] )
	      { /* 2 intersects 1 in blank square */
		for (q=0;q<4;q++)
		{
		  if (board [ win_square[n][q] ] ==0 && q != p )
		  {  /* square empty in 1 in row */
		    for (r=0;r<4;r++)
		    {
		      if (win_square[n][q] == win_square[k][r] )
		      {
			 move1 = win_square[k][r] ;
                         /* 2nd intersecting square */
			 move2 = win_square[l][p] ;
                         /* 1st intersecting square */
                         for (s = 0; s<4 ; s++)
                         {
                           move = win_square[n][s] ;
                           if ( move != move1 && move != move2 )
                           {
                             board[move] = 5;
                             strategy = 6;
                             return ;
                           }
                         }
		       }
		     }
		   }
		 }
	       }
             }
           }
	 }
       }
     }
   }
   

    start=rand()/2048 ; /* get random number from 0 to 15 */
    if (start<0 || start>15)
      printf("Random number error./n");

    for (k=start;k<16;k++)
    /* Computer takes move from major diagonal */
      {
        i=k/4;
        j=k-i*4;
	if ( board[ win_square[i][j] ] == 0 )
	{
	  /* Take square */
	  strategy = 15 ;
	  move = win_square[i][j] ;
	  board[move] = 5 ;
	  return ;
	}
      }

    for (k=1;k<16;k++)
    /* Computer takes move from major diagonal */
      {
        i=k/4;
        j=k-i*4;
	if ( board[ win_square[i][j] ] == 0 )
	{
	  /* Take square */
	  strategy = 15 ;
	  move = win_square[i][j] ;
	  board[move] = 5 ;
	  return ;
	}
      }


    start=rand()/108 ; /* ??? get random number from 0 to 303 */
    if (start<0 || start>303)
      printf("Random number error./n");


    for (k=start;k<304;k++)
    /* Computer takes move from major diagonal */
      {
        i=k/4;
        j=k-i*4;
	if ( board[ win_square[i][j] ] == 0 )
	{
	  /* Take square */
	  strategy = 15 ;
	  move = win_square[i][j] ;
	  board[move] = 5 ;
	  return ;
	}
      }


    for (k=1;k<304;k++)
    /* Computer takes move from major diagonal */
      {
        i=k/4;
        j=k-i*4;
	if ( board[ win_square[i][j] ] == 0 )
	{
	  /* Take square */
	  strategy = 15 ;
	  move = win_square[i][j] ;
	  board[move] = 5 ;
	  return ;
	}
      }

	
    if (diag) printf(" 15");
    
    strategy = 16 ; /* No move found */
    game_over = 1 ;
    return;
  }

