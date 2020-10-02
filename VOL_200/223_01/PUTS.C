#define NOCCARGC  /* no argument count passing */

/*
** puts(string)
*/

   static int i;

puts(string) char *string;  {
   i=0;
   while(string[i] != 0){
      putchar(string[i]);
      i++;
   }
   putchar('\n');
}

