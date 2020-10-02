#include <210ctdl.h>
/*****************************************************************************/
/*				ctdlchng.c				     */
/*									     */
/*		 Allows fast changes of ctdlcnfg.sys parameters 	     */
/*****************************************************************************/

/*****************************************************************************/
/*				history 				     */
/*									     */
/* 84Jun22 HAW	Created.						     */
/*****************************************************************************/

/*****************************************************************************/
/*				contents				     */
/*									     */
/*	doerror()		Writes out an error msg 		     */
/*	explanation()		Explains use of program 		     */
/*	flip()			Flips a specified boolean value 	     */
/*	main()			Main controller for this program	     */
/*	setval()		Sets the specified variable to a value	     */
/*****************************************************************************/

/*****************************************************************************/
/*				External files				     */
/*									     */
/*				210rooma.c				     */
/*				210misc.c				     */
/*									     */
/*	The files cited above must be linked in -- additionally, this file   */
/*	must be compiled using the same -e address as Citadel.		     */
/*****************************************************************************/


/*****************************************************************************/
/*	doerror()	Writes out an error msg.			     */
/*****************************************************************************/
doerror(str)
char *str;
{
    printf("The argument \"%s\" is invalid!\n", str);
}

/*****************************************************************************/
/*	explanation()	explanation of use of program			     */
/*****************************************************************************/
explanation()
{
    printf(
"   IMPROPER USAGE: This program is used to change certain parameters\n");
    printf(
"of the CTDLCNFG.SYS file.  NOT ALL parameters may be changed.	However,\n");
    printf(
"using this program to change those accessible allows the SYSOP to bypass\n");
    printf(
"reconfiguration everytime s/he wishes to change a minor parameter of\n");
    printf(
"Citadel.  The file CTDLTABL.SYS should be on the logged in disk and in\n");
    printf("the logged in user area.\n\n");
    printf("USAGE:\n");
    printf(
"Z>CTDLCHNG <parameter1> <optional parameter2> <...> ... <last parameter>\n");
    printf("\n");
    printf(
"Thus, there must be at least one parameter, and there may be more.  Err-\n");
    printf(
"oneous parameters will be noted to the user.  In the following list, a\n");
    printf(
"parameter that is just listed as a name is a parameter which may be either\n");
    printf(
"TRUE (1) or FALSE (0). Therefore, CTDCHNG, upon finding it on the argument\n");
    printf(
"list, will simply reverse its value. For integer parameters, the parameter\n");
    printf(
"name should be followed by an '=' and then an integer, signifying the\n");
    printf(
"new value.  The legal parameters (and as they appear on the command line)\n");
    printf("are as follows\n");
    printf("  CLOCK\n");
    printf(
"  MDATA=x    x in this case would be the data port. This should be\n");
    printf("		 decimal!\n");
    printf(
"  MEGAHZ=x   x in this case is the new clock speed of your computer.\n");
    printf("  RCPM\n");
    printf(
"  HOMEDISK=x x in this case should be the new homedisk number (see\n");
    printf("		 CTDLCNFG.SYS)\n");
    printf(
"  HOMEUSER=x x in this case should be the new homeuser number (see\n");
    printf("		 CTDLCNFG.SYS)\n");
    printf(
"  MSGDISK=x  x in this case should be the new msgdisk number (see\n");
    printf("		 CTDLCNFG.SYS)\n");
    printf("  LOGINOK\n");
    printf("  READOK\n");
    printf("  ENTEROK\n");
    printf("  ROOMOK\n\n");
    printf(
"For example, to change stop letting unlogged users read msgs (i.e., your\n");
    printf("CTDLCNFG.SYS of READOK was originally 1), type\n");
    printf("A>CTDLCHNG READOK\n");
    printf("and the program will change the value for you.\n\n");
    printf(
"P.S.: All integer values should be decimal here, unlike CTDLCNG.SYS\n\n");
}

/*****************************************************************************/
/*	flip()	Flips the boolean value of the specified variable.	     */
/*****************************************************************************/
flip(str)
char *str;
{
    if (strCmp(str, "ROOMOK"   ) == SAMESTRING)
	printf("%s set to %d\n", str, nonAideRoomOk = !nonAideRoomOk);
    else if (strCmp(str, "ENTEROK"  ) == SAMESTRING)
	printf("%s set to %d\n", str, unlogEnterOk = !unlogEnterOk);
    else if (strCmp(str, "READOK"   ) == SAMESTRING)
	printf("%s set to %d\n", str, unlogReadOk = !unlogReadOk);
    else if (strCmp(str, "LOGINOK"  ) == SAMESTRING)
	printf("%s set to %d\n", str, unlogLoginOk = !unlogLoginOk);
    else if (strCmp(str, "RCPM"     ) == SAMESTRING)
	printf("%s set to %d\n", str, rcpm = !rcpm);
    else if (strCmp(str, "CLOCK"    ) == SAMESTRING)
	printf("%s set to %d\n", str, clock = !clock);
    else if (strCmp(str, "ALLMAIL"  ) == SAMESTRING)
	printf("%s set to %d\n", str, !(noMail = !noMail));
}

#define FLIP	1
#define NEWVAL	2

identify(str, val)
char *str;
int  *val;
{
    char notBoolean, *searchV, *searchB;
    int  loc;

    searchB = "ROOMOK|ENTEROK|READOK|LOGINOK|RCPM|CLOCK|ALLMAIL";
    searchV = "MSGDISK|HOMEUSER|HOMEDISK|MEGAHZ|MDATA";

    notBoolean = !((loc = index(str, "=")) == ERROR);

    if (!notBoolean)
	return (index(searchB, str) == ERROR) ? ERROR : FLIP;
    else {
	str[loc] = '\0';
	if (index(searchV, str) == ERROR) {
	    str[loc] = '=';
	    return ERROR;
	}
	*val = atoi(str + loc + 1);
	return NEWVAL;
    }
}

/*****************************************************************************/
/*	main()	the main controller					     */
/*****************************************************************************/
main(argc, argv)
int  argc;
char **argv;
{
    int i, val;

    if (argc < 2) exit(explanation());
    readSysTab();
    for (i = 1; i < argc; i++) {
	switch (identify(argv[i], &val)) {
	    case FLIP  :    flip(argv[i])		 ;    break;
	    case NEWVAL:    setval(argv[i], val)	 ;    break;
	    case ERROR :    doerror(argv[i])		 ;    break;
	    default    :    exit(printf("BUG-BUG-BUG!")) ;    break;
	}
    }
    writeSysTab();

    printf(
" ABSOLUTELY don't forget to change these values in CTDLCNFG.SYS.  No, you\n");
    printf(
"don't have to reconfigure now, but the next time you crash they had better\n");
    printf(
"be set or unknown, grisly things will happen to Citadel and you when you\n");
    printf(" sleep!\n");
}

/*****************************************************************************/
/*	setval()	Sets the specified variable to val.		     */
/*****************************************************************************/
setval(str, val)
char *str;
int  val;
{
    if (strCmp(str, "MSGDISK"  ) == SAMESTRING)
	printf("%s set to %d\n", str, msgDisk = val);
    else if (strCmp(str, "HOMEUSER" ) == SAMESTRING)
	printf("%s set to %d\n", str, homeUser = val);
    else if (strCmp(str, "HOMEDISK" ) == SAMESTRING)
	printf("%s set to %d\n", str, homeDisk = val);
    else if (strCmp(str, "MEGAHZ"   ) == SAMESTRING)
	printf("%s set to %d\n", str, megaHz = val);
    else if (strCmp(str, "MDATA"    ) == SAMESTRING)
	printf("%s set to %d\n", str, mData = val);
}
");
    printf(
"don't have to re