/*
 *	乱数により４桁のテストデータを作成する
 *	作成：本間
 *	1990/10/01
 */
 
#include  <stdio.h>
#include  <stdlib.h>
#include  <time.h>
#include  <string.h>

#define   RANDOM(x)	(rand()%(x)) /* 0-(x-1) を生成するマクロ */
#define   MAX_NUMBER	2000	     /* 乱数生成数	　　　　 */

char *zero[] = {	/* 0 を埋めるテーブル  */
	"000",
	"00",
	"0"
	};
	
void main()
{
   int i, length;
   char char_rand[5], put[5];
   
   srand((unsigned int)time(NULL));
   for(i=1; i <= MAX_NUMBER; i++){
      itoa(RANDOM(9999), char_rand, 10); /* 非ＡＮＳＩ互換　*/
      length = strlen(char_rand);
      if(length < 4) {
          strcpy(put, zero[length-1]);
          strcat(put, char_rand);
      }
      else  strcpy(put, char_rand);
      printf("%s\n",put);
   }
}
