/*@*****************************************************/
/*@                                                    */
/*@ restdta -  restore the current DOS DTA.            */
/*@                                                    */
/*@   Usage:     restdta();                            */
/*@                                                    */
/*@       returns zero.                                */
/*@                                                    */
/*@       NOTE: savedta() orignially saved it.         */
/*@                                                    */
/*@*****************************************************/
extern unsigned _rax, _rbx, _rcx, _rdx, _rsi, _rdi, _res, _rds;
extern char _carryf, _zerof;
extern int dtads, dtadx;

restdta()
{
	_rax = 0x1a00;			/* set DTA function */
	_rds = dtads;			/* segment address */
	_rdx = dtadx;			/* offset */
	_doint(0x21);			/* call DOS */
	return 0;
}
