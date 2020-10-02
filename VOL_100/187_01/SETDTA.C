/*@*****************************************************/
/*@                                                    */
/*@ setdta - set a new DOS DTA buffer.                 */
/*@                                                    */
/*@   Usage:     setdta(buffer);                       */
/*@         where buffer is a buffer becomes the DTA.  */
/*@                                                    */
/*@       returns zero.                                */
/*@                                                    */
/*@       NOTE: buffer must be at least 44 bytes long. */
/*@       NOTE: buffer must not cross a segment.       */
/*@                                                    */
/*@*****************************************************/

extern unsigned _rax, _rbx, _rcx, _rdx, _rsi, _rdi, _res, _rds;
extern char _carryf, _zerof;

setdta(newdta)
char *newdta;
{
	_rax = 0x1a00;			/* set DTA DOS function */
	_rdx = newdta;			/* offset to DTA */
	_rds = _showds();		/* current DS segment */
	_doint(0x21);			/* call DOS */
	return 0;
}
