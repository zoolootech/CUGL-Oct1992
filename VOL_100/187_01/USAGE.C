/*@*****************************************************/
/*@                                                    */
/*@ usage is a template for a standard syntax error    */
/*@        report.  It was adapted from the DECUS      */
/*@        GREP utility and I use it for all my        */
/*@        main programs.  Change the first and the    */
/*@        third line for the individual program.      */
/*@                                                    */
/*@*****************************************************/


usage(s)
char    *s;
{
   puts("?CHANGE-E-");
   puts(s);
   puts("\n");
   puts("Usage: chg [-eo] pattern subst [file ...].  chg ? for help\n");
   exit(1);
}



