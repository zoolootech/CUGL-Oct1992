/*@*****************************************************/
/*@                                                    */
/*@ getdir - get text of current path on given drive.  */
/*@                                                    */
/*@   Usage:     getdir(drive, buffer);                */
/*@         where drive is the drive NUMBER (e.g.      */
/*@                  A: is 1, B: is 2, etc.  0 is the  */
/*@                  default drive.                    */
/*@               buffer receives the path name.       */
/*@                                                    */
/*@       returns the address of buffer.               */
/*@                                                    */
/*@       NOTE: buffer must be at least 64 bytes long. */
/*@                                                    */
/*@*****************************************************/

extern unsigned _rax, _rbx, _rcx, _rdx, _rsi, _rdi, _res, _rds;
extern char _carryf, _zerof;

char *getdir(drive, buffer)
int drive;
char *buffer;
{
	buffer[0] = 0x00;		/* clear buffer */
	_rax = 0x4700;			/* set current directory DOS function */
	_rdx = drive;			/* drive number */
	_rds = _showds();		/* current DS segment */
	_rsi = &buffer[1];		/* allow for leading \ to be inserted */
	_doint(0x21);			/* call DOS */
	buffer[0] = '\\';
	return buffer;
}
