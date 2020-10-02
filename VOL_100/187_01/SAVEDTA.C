/*@*****************************************************/
/*@                                                    */
/*@ savedta -  save the current DOS DTA for later.     */
/*@                                                    */
/*@   Usage:     savedta();                            */
/*@                                                    */
/*@       returns zero.                                */
/*@                                                    */
/*@       NOTE: restdta() restores it.                 */
/*@                                                    */
/*@*****************************************************/


extern unsigned _rax, _rbx, _rcx, _rdx, _rsi, _rdi, _res, _rds;
extern char _carryf, _zerof;

int dtads, dtadx;

savdta()
{
	_rax = 0x2f00;			/* get dta func */
	_doint(0x21);			/* call DOS */
	dtads = _res;			/* segment for dta */
	dtadx = _rbx;			/* offset for dta */
	return 0;
}
