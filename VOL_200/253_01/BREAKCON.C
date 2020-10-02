                                          /* Chapter 3 - Program 5 */
main()
{
int xx;

   for(xx = 5;xx < 15;xx = xx + 1){
      if (xx == 8)
         break;
      printf("In the break loop, xx is now %d\n",xx);
   }

   for(xx = 5;xx < 15;xx = xx + 1){
      if (xx == 8)
         continue;
      printf("In the continue loop, xx is now %d\n",xx);
   }
}



/* Result of execution

In the break loop, xx is now 5
In the break loop, xx is now 6
In the break loop, xx is now 7
In the continue loop, xx is now 5
In the continue loop, xx is now 6
In the continue loop, xx is now 7
In the continue loop, xx is now 9
In the continue loop, xx is now 10
In the continue loop, xx is now 11
In the continue loop, xx is now 12
In the continue loop, xx is now 13
In the continue loop, xx is now 14

*/
