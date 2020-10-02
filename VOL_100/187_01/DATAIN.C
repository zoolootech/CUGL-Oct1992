/*@*****************************************************/
/*@                                                    */
/*@ datain - prototype input routine which reads       */
/*@        transparently from either the keyboard      */
/*@        or a file, but checks the keyboard for      */
/*@        override on each call.                      */
/*@                                                    */
/*@   Usage:     datain();                             */
/*@       returns an int sized char.                   */
/*@                                                    */
/*@*****************************************************/

int datain()
{
	int nc, c, kbdin();
	unsigned i;

	if (IFd != -1) {
		if ((c = kbdin()) == CMND) {
			conout("\nFile Abort from Console \n",NORM);
			fclose(IFd);
			IFd = -1;
			getchar();
			return(c);
		}
		if ((c = getc(IFd)) == EOF) {
			conout("\nEnd of File on ",NORM);
			conout(Infile,NORM);
			fclose(IFd);
			IFd = -1;
			return(kbdin());
		}
		else {
			if (TrmMd) {
				if (c == SIMCTRL) {
					if ((nc = getc(IFd)) == EOF) {
						conout("\nPremature End of File on ",NORM);
						conout(Infile,NORM);
						fclose(IFd);
						IFd = -1;
						return(kbdin());
					}
					if (nc != c)
						c = 0xbf & nc;	/* convert to control character */
				}
				for (i=0; i <10000; i++) ;
			}
			return(c);
		}
	}
	else
		return(kbdin());
}

