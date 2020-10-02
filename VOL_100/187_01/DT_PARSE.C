/*@*****************************************************/
/*@                                                    */
/*@ time_parse - parse a time parameter.               */
/*@        Format is -t{HH{:MM{:SS}}},{HH{:MM{:SS}}}   */
/*@        The first is the minimum value, the         */
/*@        second is the maximum.  Missing minimum     */
/*@        values default to 00, missing maximum       */
/*@        values default to 99.                       */
/*@                                                    */
/*@   Usage:     time_parse(p);                        */
/*@       where p is a pointer to the input arg.       */
/*@       Returns p pointing to the end of string.     */
/*@       Exits via call to error() on error.          */
/*@                                                    */
/*@*****************************************************/

#include	"dtparse.mon"

char strbuf[30];		/* string work area */

/***********************************************************************/
/*                                                                     */
/*	Parse time input parameters.                                       */
/*                                                                     */
/***********************************************************************/

char *time_parse(p)
char *p;
{
	int i, strlen(), strcat(), len;
	char ct, ct2, *dt_parse();

    p = dt_parse(p, time_sel.min_sel, ':');
    if ((len = strlen(time_sel.min_sel)) > sizeof(time_sel.min_sel))
    		error("-t minimum parameter too long. Max is ", itoa(sizeof(time_sel.min_sel)-1, strbuf));
    if (time_sel.min_sel[0]) {  		/* if parameter was given */
   		ct = time_sel.min_sel[2];	/* first possible colon */
   		ct2 = time_sel.min_sel[5];	/* second possible colon */
   		if (ct)
   			if ((ct != ':') || (ct2 && (ct2 != ':'))) 	/* and no colon */
 			error("-t minimum not in HH{:MM{:SS}} format.", "");
     	strncat(time_sel.min_sel, ":00:00", sizeof(time_sel.min_sel)-len-1);	/* normalize */
     	time_sel.min_sel[sizeof(time_sel.min_sel)-1] = 0x00;	/* EOS */
	}
    if (*p == ',')
     p++;				/* skip comma */
    p = dt_parse(p, time_sel.max_sel, ':');
    if ((len = strlen(time_sel.max_sel)) > sizeof(time_sel.max_sel))
    		error("-t maximum parameter too long. Max is ", itoa(sizeof(time_sel.max_sel)-1, strbuf));
    if (time_sel.max_sel[0]) {  		/* if parameter was given */
   		ct = time_sel.max_sel[2];	/* first possible colon */
   		ct2 = time_sel.max_sel[5];	/* second possible colon */
   		if (ct)
   			if ((ct != ':') || (ct2 && (ct2 != ':'))) 	/* and no colon */
       			error("-t maximum not in HH{:MM{:SS}} format.", "");
       	strncat(time_sel.max_sel, ":99:99", sizeof(time_sel.max_sel)-len-1);	/* normalize */
       	time_sel.max_sel[sizeof(time_sel.max_sel)-1] = 0x00;	/* EOS */
	}
	return p;
}

/*@*****************************************************/
/*@                                                    */
/*@ date_parse - parse a date parameter.               */
/*@        Format is -d{MM{-DD{-YY}}},{MM{-DD{-YY}}}   */
/*@        The first is the minimum value, the         */
/*@        second is the maximum.  Missing minimum     */
/*@        values default to 00, missing maximum       */
/*@        values default to 99.                       */
/*@                                                    */
/*@   Usage:     date_parse(p);                        */
/*@       where p is a pointer to the input arg.       */
/*@       Returns p pointing to the end of string.     */
/*@       Exits via call to error() on error.          */
/*@                                                    */
/*@*****************************************************/

/***********************************************************************/
/*                                                                     */
/*	Parse date input parameters.                                       */
/*                                                                     */
/***********************************************************************/

char *date_parse(p)
char *p;
{
	int i, strlen(), isdigit(), yymmdd(), strcat(), len, dates();
	char ct, ct2, *dt_parse();

    p = dt_parse(p, date_sel.min_sel, '-');
    if ((len = strlen(date_sel.min_sel)) > sizeof(date_sel.min_sel))
   		error("-d parameter too long. Max is ", itoa(sizeof(date_sel.min_sel)-1, strbuf));
    if (date_sel.min_sel[0]) {  		/* if parameter was given */
   		ct = date_sel.min_sel[2];	/* first possible dash */
   		ct2 = date_sel.min_sel[5];	/* second possible dash */
   		if (ct)
   			if ((ct != '-') || (ct2 && (ct2 != '-'))) 	/* and no dash */
      			error("-d minimum not in MM{-DD{-YY}} format.", "");
      	if (len == 2) {
	      	strcat(date_sel.min_sel, "-00-");	/* normalize */
	      	dates(strbuf);						/* get year */
	      	strcat(date_sel.min_sel, &strbuf[6]);
	      }
	      else if (len == 5) {
	      	strcat(date_sel.min_sel, "-");		/* normalize */
	      	dates(strbuf);						/* get year */
	      	strcat(date_sel.min_sel, &strbuf[6]);
	      }
       	date_sel.min_sel[sizeof(date_sel.min_sel)-1] = 0x00;	/* EOS */
       	if ((!isdigit(date_sel.min_sel[3])) || (!isdigit(date_sel.min_sel[6])))
   			error("-d minimum not in MM{-DD{-YY}} format.", "");
       	yymmdd(date_sel.min_sel, '-');		/* change it to yymmdd form */
	}
    if (*p == ',')
       p++;				/* skip comma */
    p = dt_parse(p, date_sel.max_sel, '-');
    if ((len = strlen(date_sel.max_sel)) > sizeof(date_sel.max_sel))
   		error("-d parameter too long. Max is ", itoa(sizeof(date_sel.max_sel)-1, strbuf));
    if (date_sel.max_sel[0]) {  		/* if parameter was given */
 		ct = date_sel.max_sel[2];	/* first possible dash */
 		ct2 = date_sel.max_sel[5];	/* second possible dash */
 		if (ct)
 			if ((ct != '-') || (ct2 && (ct2 != '-'))) 	/* and no dash */
       			error("-d maximum not in MM{-DD{-YY}} format.", "");
      	strncat(date_sel.max_sel, "-99-99", sizeof(date_sel.max_sel)-len-1);	/* normalize */
       	date_sel.max_sel[sizeof(date_sel.max_sel)-1] = 0x00;	/* EOS */
       	if ((!isdigit(date_sel.max_sel[3])) || (!isdigit(date_sel.max_sel[6])))
   			error("-d maximum not in MM{-DD{-YY}} format.", "");
      	yymmdd(date_sel.max_sel, '-');		/* change it to yymmdd form */
	}
	return p;
}

/*@*****************************************************/
/*@                                                    */
/*@ dt_parse - get one date/time value.                */
/*@        Expands one digit values by adding a        */
/*@        leading zero.  Allows a variable number     */
/*@        of delimited groups of one or two digits.   */
/*@                                                    */
/*@   Usage:     dt_parse(p, s, c);                    */
/*@       where p is a pointer to the input arg.       */
/*@             s is a pointer to the output area.     */
/*@             c is a char holding the delimiter.     */
/*@                                                    */
/*@     Returns the updated pointer p.                 */
/*@                                                    */
/*@*****************************************************/

/***********************************************************************/
/*                                                                     */
/*	 Parse a date/time parameter.                                      */
/*                                                                     */
/*   Assumes that input is xxcxxcxx where the xx are integers and      */
/*      c is the given separator.  If only one digit is given for      */
/*      any pair, a leading zero is added to normalize it.             */
/*                                                                     */
/***********************************************************************/

char *dt_parse(p, s, c)
char *p, *s, c;
{
	char *save;

	while (*p && (*p != ',')) {
		save = s;			/* save start of output area */
	    while (*p && (*p != ',') && (*p != c))	/* copy min/max hour/month */
	   		*s++ = *p++;
	    if ((s - save) == 1){		/* need leading zero */
	   		s[0] = s[-1];
	   		s[-1] = '0';
	   		s++;
	   	}
	   	if (*p == c)			/* if a separator, copy it too */
	   		*s++ = *p++;
	}
    *s = 0x00;			/* EOS */


	return p;
}

