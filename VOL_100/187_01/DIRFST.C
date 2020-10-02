/*@*****************************************************/
/*@                                                    */
/*@ dirfst - search directory for first matching entry.*/
/*@                                                    */
/*@   Usage:     dirfst(path, dta, attr);              */
/*@       where path is the full DOS path+filename.    */
/*@            dta is the DOS func 0x4e work area      */
/*@               (must be at least 44 bytes long).    */
/*@            attr is the require file attributes:    */
/*@                                                    */
/*@               0x01 - read-only                     */
/*@               0x02 - hidden                        */
/*@               0x04 - system                        */
/*@               0x08 - volume id                     */
/*@               0x10 - directory                     */
/*@               0x20 - archive bit(1=modified)       */
/*@                                                    */
/*@               0x08 excludes all others.  Others    */
/*@               are additive.  0x00 implies read-only*/
/*@               also.                                */
/*@                                                    */
/*@       returns a pointer to the file in the DTA.    */
/*@          an extern of dirret contains the last     */
/*@             DOS return code.                       */
/*@                                                    */
/*@          DTA looks like:                           */
/*@                                                    */
/*@               21 bytes - reserved for func 0x4f    */
/*@                1 byte  - attributes found          */
/*@                2 bytes - time of file              */
/*@                2 bytes - date of file              */
/*@                2 bytes - low word of file size     */
/*@                2 bytes - high word of file size    */
/*@ returns ===>  13 bytes - name and extension of file*/
/*@                                                    */
/*@                                                    */
/*@*****************************************************/


extern unsigned _rax, _rbx, _rcx, _rdx, _rsi, _rdi, _res, _rds;
extern char _carryf, _zerof;

int dirret;

char *dirfst(path, dta, attr)
char *path, *dta;
int attr;
{
	int sav_carry, sav_rax;

	savedta();				/* save old dta */
	setdta(dta);			/* use caller's area */
	dirret = 0;				/* clear return code */

	_rds = _showds();		/* current data segment */
	_rdx = path;			/* path offset */
	_rcx = attr;			/* required attribute */
	_rax = 0x4e00;			/* DOS find first function */
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
