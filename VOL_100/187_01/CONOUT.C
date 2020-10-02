/*@*****************************************************/
/*@                                                    */
/*@ conout - output a string to the screen using       */
/*@        DeSmet interrupt 10 routine.  Only          */
/*@        exists to allow quick modification.         */
/*@                                                    */
/*@   Usage:     conout(string, attribute);            */
/*@       where attribute defined in IBM Tech Manual.  */
/*@       (copout till I find my list).                */
/*@                                                    */
/*@*****************************************************/

conout(s,attr)
char *s;
int attr;
{
	scr_aputs(s,attr);
}
