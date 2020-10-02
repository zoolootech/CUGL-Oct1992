/*@*****************************************************/
/*@                                                    */
/*@ help - output help data structure.  Parameter is   */
/*@        assumed to be an array of pointers to       */
/*@        strings giving help.  The last one must     */
/*@        be an empty string.                         */
/*@                                                    */
/*@   Usage:     help(str);                            */
/*@       where str is a pointer to the array.         */
/*@       (see most pgms in CPGMS.ARC).                */
/*@                                                    */
/*@*****************************************************/

/*******************************************************/

help(hp)
char **hp;  /* dns added extra '*'  */
/*
 * Give good help
 */
{
   register char   **dp;

   for (dp = hp; *dp; dp++)
      printf("%s\n", *dp);
}


