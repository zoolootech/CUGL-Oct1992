/* H.C - This is a simple help file for the AZTEC 'c' compiler
	    error codes.

   Copyright (c) 1983  Karl L Remmler
   This program may not be used on any IBM Computer System.

   Permission is granted for non-commercial use by members of the
   Heath User's Group and members of Public Domain (RCPM) databases.
   It may be downloaded, but it permission is not granted for
   uploading this file to any other database.  Permission is not
   granted for use on an IBM computer system.

   Example for the switch case.

   AUTHOR: KARL L. REMMLER
   BUGS: none

   A BDS version of AzHelp.
*/

atoi(s) 	/* convert s to integer */
char s[];
{
	register int i, n;

	n = 0;
	for (i = 0; s[i] >= '0' && s[i] <= '9'; ++i)
		n = 10 * n + s[i] - '0';
	return(n);
}

main(argc,argv)
int argc;
char *argv[];

{
  register int code;

  if (argc != 2) {
	puts("\nCopyright (c) 1983  Karl L. Remmler\n\n");
	puts("\nUsage: A>H ERROR# <cr>\n");
	puts("\n\nWhere ERROR# is the decimal error code number\n");
	puts(" and <cr> means press the carriage return\n");
	exit(0);
  }
  puts("\nCopyright (c) 1983  Karl L. Remmler\n\n");
  code = atoi(argv[1]);
  switch (code) {
    case 1:
	puts("\nBAD DIGIT IN OCTAL CONSTANT\n");
	break;
    case 2:
	puts("\nSTRING SPACE EXHAUSTED (USE COMPILER -Z OPTION\n");
	break;
    case 3:
	puts("\nUNTERMINATED STRING\n");
	break;
    case 4:
	puts("\nCOMPILER ERROR IN effaddr\n");
	break;
    case 5:
	puts("\nILLEGAL TYPE FOR FUNCTION\n");
	break;
    case 6:
	puts("\nINAPPROPRIATE ARGUMENTS\n");
	break;
    case 7:
	puts("\nBAD DECLARATION SYNTAX\n");
	break;
    case 8:
	puts("\nNAME NOT ALLOWED HERE\n");
	break;
    case 9:
	puts("\nMUST BE CONSTANT\n");
	break;
    case 10:
	puts("\nSIZE MUST BE POSITIVE INTEGER\n");
	break;
    case 11:
	puts("\nDATA TYPE TOO COMPLEX\n");
	break;
    case 12:
	puts("\nILLEGAL POINTER REFERENCE\n");
	break;
    case 13:
    case 14:
	puts("\nUNIMPLEMENTED TYPE\n");
	break;
    case 15:
	puts("\nSTORAGE CLASS CONFLICT\n");
	break;
    case 16:
    case 18:
	puts("\nDATA TYPE CONFLICT\n");
	break;
    case 17:
	puts("\nUNSUPPORTED DATA TYPE\n");
	break;
    case 19:
	puts("\nTOO MANY STRUCTURES\n");
	break;
    case 20:
	puts("\nSTRUCTURE REDECLARATION\n");
	break;
    case 21:
	puts("\nMISSING )'s\n");
	break;
    case 22:
	puts("\nSTRUCT DECL SYNTAX\n");
	break;
    case 23:
	puts("\nUNDEFINED STRUCT NAME\n");
	break;
    case 24:
	puts("\nNEED RIGHT PARENTHESIS\n");
	break;
    case 25:
	puts("\nEXPECTED SYMBOL HERE\n");
	break;
    case 26:
	puts("\nMUST BE STRUCTURE/UNION MEMBER\n");
	break;
    case 27:
	puts("\nILLEGAL TYPE CAST\n");
	break;
    case 28:
	puts("\nINCOMPATABLE STRUCTURES\n");
	break;
    case 29:
	puts("\nSTRUCTURE NOT ALLOWED HERE\n");
	break;
    case 30:
	puts("\nMISSING : ON ? EXPR\n");
	break;
    case 31:
	puts("\nCALL OF NON-FUNCTION\n");
	break;
    case 32:
	puts("\nILLEGAL POINTER CALCULATION\n");
	break;
    case 33:
	puts("\nILLEGAL TYPE\n");
	break;
    case 34:
	puts("\nUNDEFINED SYMBOL\n");
	break;
    case 35:
	puts("\nTYPEDEF NOT ALLOWED HERE\n");
	break;
    case 36:
	puts("\nNO MORE EXPRESSION SPACE (USE COMPILER -E OPTION)\n");
	break;
    case 37:
	puts("\nINVALID EXPRESSION\n");
	break;
    case 38:
	puts("\nNO AUTO AGGREGATE INITIALIZATION\n");
	break;
    case 39:
	puts("\nNO STRINGS IN AUTOMATIC\n");
	break;
    case 40:
	puts("\nTHIS SHOULD NOT HAPPEN\n");
	break;
    case 41:
	puts("\nINVALID INITIALIZER\n");
	break;
    case 42:
	puts("\nTOO MANY INITIALIZERS\n");
	break;
    case 43:
	puts("\nUNDEFINED STRUCTURE INITIALIZATION\n");
	break;
    case 44:
	puts("\nTOO MANY STRUCTURE INITIALIZERS\n");
	break;
    case 45:
	puts("\nBAD DECLARATION SYNTAX\n");
	break;
    case 46:
	puts("\nMISSING CLOSING BRACKET\n");
	break;
    case 47:
	puts("\nOPEN FAILURE ON INCLUDE FILE\n");
	break;
    case 48:
	puts("\nILLEGAL SYMBOL NAME\n");
	break;
    case 49:
	puts("\nALREADY DEFINED\n");
	break;
    case 50:
	puts("\nMISSING BRACKET\n");
	break;
    case 51:
	puts("\n\tMUST BE LVALUE\n");
	break;
    case 52:
	puts("\n\tSYMBOL TABLE OVERFLOW\n");
	break;
    case 53:
	puts("\n\tMULTIPLY DEFINED LABEL\n");
	break;
    case 54:
	puts("\n\tTOO MANY LABELS\n");
	break;
    case 55:
	puts("\n\tMISSING QUOTE\n");
	break;
    case 56:
	puts("\n\tMISSING APOSTROPHE\n");
	break;
    case 57:
	puts("\n\tLINE TOO LONG\n");
	break;
    case 58:
	puts("\n\tILLEGAL # ENCOUNTERED\n");
	break;
    case 59:
	puts("\n\tMACRO TABLE FULL (SEE COMPILER -X OPTION)\n");
	break;
    case 60:
	puts("\n\tOUTPUT FILE ERROR\n");
	break;
    case 61:
	puts("\n\tREFERENCE OF MEMBER OF UNDEFINED STRUCTURE\n");
	break;
    case 62:
	puts("\n\tFUNCTION BODY MUST BE COMPOUND STATEMENT\n");
	break;
    case 63:
	puts("\n\tUNDEFINED LABEL\n");
	break;
    case 64:
	puts("\n\tINAPPROPRIATE ARGUEMENTS\n");
	break;
    case 65:
	puts("\n\tILLEGAL ARGUEMENT NAME\n");
	break;
    case 66:
	puts("\n\tEXPECTED COMMA\n");
	break;
    case 67:
	puts("\n\tINVALID ELSE\n");
	break;
    case 68:
	puts("\n\tSYNTAX  ERROR\n");
	break;
    case 69:
	puts("\n\tMISSING SEMICOLON\n");
	break;
    case 70:
	puts("\n\tBAD GOTO SYNTAX\n");
	break;
    case 71:
    case 72:
    case 73:
	puts("\n\tSTATEMENT SYNTAX\n");
	break;
    case 74:
	puts("\n\tCASE VALUE MUST BE INTEGER CONSTANT\n");
	break;
    case 75:
	puts("\n\tMISSING COLON ON CASE\n");
	break;
    case 76:
	puts("\n\tTOO MANY CASES IN SWITCH (USE COMPILER -Y OPTION\n");
	break;
    case 77:
	puts("\n\tCASE OUTSIDE OF SWITCH\n");
	break;
    case 78:
	puts("\n\tMISSING COLON\n");
	break;
    case 79:
	puts("\n\tDUPLICATE DEFAULT\n");
	break;
    case 80:
	puts("\n\tDEFAULT OUTSIDE OF SWITCH\n");
	break;
    case 81:
	puts("\n\tBREAK/CONTINUE ERROR\n");
	break;
    case 82:
	puts("\n\tILLEGAL CHARACTER\n");
	break;
    case 83:
	puts("\n\tTOO MANY NESTED INCLUDES\n");
	break;
    case 84:
	puts("\n\tILLEGAL CHARACTER\n");
	break;
    case 85:
	puts("\n\tNOT AN ARGUMENT\n");
	break;
    case 86:
	puts("\n\tNULL DIMENSION\n");
	break;
    case 87:
	puts("\n\tINVALID CHARACTER CONSTANT\n");
	break;
    case 88:
	puts("\n\tNOT A STRUCTURE\n");
	break;
    case 89:
	puts("\n\tINVALID STORAGE CASE\n");
	break;
    case 90:
	puts("\n\tSYMBOL REDECLARED\n");
	break;
    case 91:
	puts("\n\tILLEGAL USE OF FLOATING POINT TYPE\n");
	break;
    case 92:
	puts("\n\tILLEGAL TYPE CONVERSION\n");
	break;
    case 93:
	puts("\n\tILLEGAL EXPRESSION TYPE FOR SWITCH\n");
	break;
    case 94:
	puts("\n\tBAD ARGUMENT TO DEFINE\n");
	break;
    case 95:
	puts("\n\tNO ARGUMENT LIST\n");
	break;
    case 96:
	puts("\n\tMISSING ARG\n");
	break;
    case 97:
	puts("\n\tBAD ARG\n");
	break;
    case 98:
	puts("\n\tNOT ENOUGH ARGS\n");
	break;
    case 99:
	puts("\n\tCONVERSION NOT FOUND IN CODE TABLE\n");
	break;
    default:
	puts("\n\t\033pYOU ARE IN BAD BAD Ceeeee TROUBLE\033q\n");
  }
  exit(0);
}
case 98:
	printf("\n\tNOT ENOUGH ARGS\n");
	break;
    case