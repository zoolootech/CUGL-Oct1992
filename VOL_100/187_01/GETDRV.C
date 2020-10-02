/*@*****************************************************/
/*@                                                    */
/*@ getdrv - get the currently logged in drive id.     */
/*@                                                    */
/*@   Usage:     getdrv();                             */
/*@       Returns an integer sized value with the      */
/*@         numeric value of the drive.  E.G. C:       */
/*@         is 0x03.                                   */
/*@                                                    */
/*@*****************************************************/

int getdrv()
{
#asm
	MOV	AH,19h
	INT	21h
	MOV	AH,0
#endasm
	return;
}
