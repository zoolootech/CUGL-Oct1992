/*@*****************************************************/
/*@                                                    */
/*@ chgattr - get or set attribute on named file.      */
/*@                                                    */
/*@   Usage:    chgattr(getset, path, attr);           */
/*@       where getset is 'S' or anything else.  If    */
/*@                it is 'S', the file attribute is    */
/*@                changed to attr.  Otherwise the     */
/*@                current file attribute is returned  */
/*@                in attr.                            */
/*@                                                    */
/*@       returns zero if successful, non-zero otherwise.*/
/*@       1 => invalid function (from DOS).            */
/*@       3 => path not found (from DOS).              */
/*@       5 => access denied (from DOS).               */
/*@                                                    */
/*@*****************************************************/


extern unsigned _rax, _rbx, _rcx, _rdx, _rsi, _rdi, _res, _rds;
extern char _carryf, _zerof;


int chgattr(getset, path, attr)
char getset, *path;
int *attr;
{
	if (getset == 'S') {
		_rcx = *attr;
		_rax = 0x4301;		/* DOS change attributes function */
	}
	else
		_rax = 0x4300;		/* DOS get attributes function */

	_rds = _showds();		/* segment of path */	
	_rdx = path;			/* offset of path */

	_doint(0x21);			/* call DOS */

	if (!_carryf)			/* error ? */
		_rax = 0;			/* ..no, clear reg */

	if (_rax == 0)
		*attr = _rcx;
	return(_rax);
}		
