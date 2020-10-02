/*@*****************************************************/
/*@                                                    */
/*@ error - output two strings and exit abnormally.    */
/*@        Any CR/LFs must be in the strings.          */
/*@        error() exits with ERRORLEVEL = 1.          */
/*@                                                    */
/*@   Usage:     error(s1, s2);                        */
/*@       where s1 and s2 are strings to be displayed  */
/*@         on STDOUT.                                 */
/*@                                                    */
/*@*****************************************************/


error(s1, s2)
char *s1, *s2;
{
   puts(s1);
   puts(s2);
   exit(1);
}


