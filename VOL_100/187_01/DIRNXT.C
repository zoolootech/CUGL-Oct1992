/*@*****************************************************/
/*@                                                    */
/*@ dirnxt - search directory for next matching entry. */
/*@                                                    */
/*@   Usage:     dirnxt(dta);                          */
/*@       where dta is the DTA specified in a previous */
/*@            call to dirfst().                       */
/*@                                                    */
/*@       returns a pointer to the file in the DTA.    */
/*@                                                    */
/*@           an extern of dirret has the last return  */
/*@               code from DOS.                       */
/*@                                                    */
/*@       NOTE:  You must not tamper with the DTA      */
/*@           between dirfst() and dirnxt() calls.     */
/*@                                                    */
/*@*****************************************************/


extern unsigned _rax, _rbx, _rcx, _rdx, _rsi, _rdi, _res, _rds;
extern char _carryf, _zerof;

extern int dirret;

char *dirnxt(dta)
char *dta;
{
	int sav_carry, sav_rax;

	savedta();				/* save old dta */
	setdta(dta);			/* use caller's area */
	dirret = 0;				/* clear return code */

	_rax = 0x4f00;			/* DOS find first function */
	_doint(0x21);			/* call DOS */
	sav_carry = _carryf;
	sav_rax = _rax;
	restdta();				/* reset to DOS's DTA */
	if (!sav_carry)
		return &dta[30];
	else {
		dirret = sav_rax;		/* save return code */
		return 0;
	}
}
