/* CHELP.C - This is a simple help file for the AZTEC 'c' compiler
	    error codes.

   Copyright (c) 1983 Karl Remmler
   This program may not be used on an IBM Computer System.

   Permission is granted for non-commercial use of this program by
   members of Public Domain (RCPM) data bases and members of the
   Heath Users Group on Heath/Zenith Computer Systems.	Permission
   is not granted for use on any IBM Computer System.

   Example for the switch case.

   AUTHOR: KARL L. REMMLER
   BUGS: none

   Source code is written here for C/80 and AZTEC.
   Use -cn & -wn swithces when compiling with C/80.
*/

#include "printf.c"

atoi(s) 	/* convert s to integer */
char s[];
{
	int i, n;

	n = 0;
	for (i = 0; s[i] >= '0' && s[i] <= '9'; ++i)
		n = 10 * n + s[i] - '0';
	return(n);
}

main(argc,argv)
int argc;
char *argv[];

{
  int code;

  if (argc != 2) {
	printf("\nCopyright (c) 1983  Karl L. Remmler\n\n");
	printf("\nUsage: A>CHELP ERROR# <cr>\n");
	printf("\n\nWhere ERROR# is the decimal error code number\n");
	printf(" and <cr> means press the carriage return\n");
	exit(0);
  }
  printf("\nCopyright (c) 1983	Karl L. Remmler\n\n");
  code = atoi(argv[1]);
  switch (code) {
    case 1:
	printf("\nBAD DIGIT IN OCTAL CONSTANT\n");
	break;
    case 2:
	printf("\nSTRING SPACE EXHAUSTED (USE COMPILER -Z OPTION\n");
	break;
    case 3:
	printf("\nUNTERMINATED STRING\n");
	break;
    case 4:
	printf("\nCOMPILER ERROR IN effaddr\n");
	break;
    case 5:
	printf("\nILLEGAL TYPE FOR FUNCTION\n");
	break;
    case 6:
	printf("\nINAPPROPRIATE ARGUMENTS\n");
	break;
    case 7:
	printf("\nBAD DECLARATION SYNTAX\n");
	break;
    case 8:
	printf("\nNAME NOT ALLOWED HERE\n");
	break;
    case 9:
	printf("\nMUST BE CONSTANT\n");
	break;
    case 10:
	printf("\nSIZE MUST BE POSITIVE INTEGER\n");
	break;
    case 11:
	printf("\nDATA TYPE TOO COMPLEX\n");
	break;
    case 12:
	printf("\nILLEGAL POINTER REFERENCE\n");
	break;
    case 13:
    case 14:
	printf("\nUNIMPLEMENTED TYPE\n");
	break;
    case 15:
	printf("\nSTORAGE CLASS CONFLICT\n");
	break;
    case 16:
    case 18:
	printf("\nDATA TYPE CONFLICT\n");
	break;
    case 17:
	printf("\nUNSUPPORTED DATA TYPE\n");
	break;
    case 19:
	printf("\nTOO MANY STRUCTURES\n");
	break;
    case 20:
	printf("\nSTRUCTURE REDECLARATION\n");
	break;
    case 21:
	printf("\nMISSING )'s\n");
	break;
    case 22:
	printf("\nSTRUCT DECL SYNTAX\n");
	break;
    case 23:
	printf("\nUNDEFINED STRUCT NAME\n");
	break;
    case 24:
	printf("\nNEED RIGHT PARENTHESIS\n");
	break;
    case 25:
	printf("\nEXPECTED SYMBOL HERE\n");
	break;
    case 26:
	printf("\nMUST BE STRUCTURE/UNION MEMBER\n");
	break;
    case 27:
	printf("\nILLEGAL TYPE CAST\n");
	break;
    case 28:
	printf("\nINCOMPATABLE STRUCTURES\n");
	break;
    case 29:
	printf("\nSTRUCTURE NOT ALLOWED HERE\n");
	break;
    case 30:
	printf("\nMISSING : ON ? EXPR\n");
	break;
    case 31:
	printf("\nCALL OF NON-FUNCTION\n");
	break;
    case 32:
	printf("\nILLEGAL POINTER CALCULATION\n");
	break;
    case 33:
	printf("\nILLEGAL TYPE\n");
	break;
    case 34:
	printf("\nUNDEFINED SYMBOL\n");
	break;
    case 35:
	printf("\nTYPEDEF NOT ALLOWED HERE\n");
	break;
    case 36:
	printf("\nNO MORE EXPRESSION SPACE (USE COMPILER -E OPTION)\n");
	break;
    case 37:
	printf("\nINVALID EXPRESSION\n");
	break;
    case 38:
	printf("\nNO AUTO AGGREGATE INITIALIZATION\n");
	break;
    case 39:
	printf("\nNO STRINGS IN AUTOMATIC\n");
	break;
    case 40:
	printf("\nTHIS SHOULD NOT HAPPEN\n");
	break;
    case 41:
	printf("\nINVALID INITIALIZER\n");
	break;
    case 42:
	printf("\nTOO MANY INITIALIZERS\n");
	break;
    case 43:
	printf("\nUNDEFINED STRUCTURE INITIALIZATION\n");
	break;
    case 44:
	printf("\nTOO MANY STRUCTURE INITIALIZERS\n");
	break;
    case 45:
	printf("\nBAD DECLARATION SYNTAX\n");
	break;
    case 46:
	printf("\nMISSING CLOSING BRACKET\n");
	break;
    case 47:
	printf("\nOPEN FAILURE ON INCLUDE FILE\n");
	break;
    case 48:
	printf("\nILLEGAL SYMBOL NAME\n");
	break;
    case 49:
	printf("\nALREADY DEFINED\n");
	break;
    case 50:
	printf("\nMISSING BRACKET\n");
	break;
    case 51:
	printf("\n\tMUST BE LVALUE\n");
	break;
    case 52:
	printf("\n\tSYMBOL TABLE OVERFLOW\n");
	break;
    case 53:
	printf("\n\tMULTIPLY DEFINED LABEL\n");
	break;
    case 54:
	printf("\n\tTOO MANY LABELS\n");
	break;
    case 55:
	printf("\n\tMISSING QUOTE\n");
	break;
    case 56:
	printf("\n\tMISSING APOSTROPHE\n");
	break;
    case 57:
	printf("\n\tLINE TOO LONG\n");
	break;
    case 58:
	printf("\n\tILLEGAL # ENCOUNTERED\n");
	break;
    case 59:
	printf("\n\tMACRO TABLE FULL (SEE COMPILER -X OPTION)\n");
	break;
    case 60:
	printf("\n\tOUTPUT FILE ERROR\n");
	break;
    case 61:
	printf("\n\tREFERENCE OF MEMBER OF UNDEFINED STRUCTURE\n");
	break;
    case 62:
	printf("\n\tFUNCTION BODY MUST BE COMPOUND STATEMENT\n");
	break;
    case 63:
	printf("\n\tUNDEFINED LABEL\n");
	break;
    case 64:
	printf("\n\tINAPPROPRIATE ARGUEMENTS\n");
	break;
    case 65:
	printf("\n\tILLEGAL ARGUEMENT NAME\n");
	break;
    case 66:
	printf("\n\tEXPECTED COMMA\n");
	break;
    case 67:
	printf("\n\tINVALID ELSE\n");
	break;
    case 68:
	printf("\n\tSYNTAX  ERROR\n");
	break;
    case 69:
	printf("\n\tMISSING SEMICOLON\n");
	break;
    case 70:
	printf("\n\tBAD GOTO SYNTAX\n");
	break;
    case 71:
    case 72:
    case 73:
	printf("\n\tSTATEMENT SYNTAX\n");
	break;
    case 74:
	printf("\n\tCASE VALUE MUST BE INTEGER CONSTANT\n");
	break;
    case 75:
	printf("\n\tMISSING COLON ON CASE\n");
	break;
    case 76:
	printf("\n\tTOO MANY CASES IN SWITCH (USE COMPILER -Y OPTION\n");
	break;
    case 77:
	printf("\n\tCASE OUTSIDE OF SWITCH\n");
	break;
    case 78:
	printf("\n\tMISSING COLON\n");
	break;
    case 79:
	printf("\n\tDUPLICATE DEFAULT\n");
	break;
    case 80:
	printf("\n\tDEFAULT OUTSIDE OF SWITCH\n");
	break;
    case 81:
	printf("\n\tBREAK/CONTINUE ERROR\n");
	break;
    case 82:
	printf("\n\tILLEGAL CHARACTER\n");
	break;
    case 83:
	printf("\n\tTOO MANY NESTED INCLUDES\n");
	break;
    case 84:
	printf("\n\tILLEGAL CHARACTER\n");
	break;
    case 85:
	printf("\n\tNOT AN ARGUMENT\n");
	break;
    case 86:
	printf("\n\tNULL DIMENSION\n");
	break;
    case 87:
	printf("\n\tINVALID CHARACTER CONSTANT\n");
	break;
    case 88:
	printf("\n\tNOT A STRUCTURE\n");
	break;
    case 89:
	printf("\n\tINVALID STORAGE CASE\n");
	break;
    case 90:
	printf("\n\tSYMBOL REDECLARED\n");
	break;
    case 91:
	printf("\n\tILLEGAL USE OF FLOATING POINT TYPE\n");
	break;
    case 92:
	printf("\n\tILLEGAL TYPE CONVERSION\n");
	break;
    case 93:
	printf("\n\tILLEGAL EXPRESSION TYPE FOR SWITCH\n");
	break;
    case 94:
	printf("\n\tBAD ARGUMENT TO DEFINE\n");
	break;
    case 95:
	printf("\n\tNO ARGUMENT LIST\n");
	break;
    case 96:
	printf("\n\tMISSING ARG\n");
	break;
    case 97:
	printf("\n\tBAD ARG\n");
	break;
    case 98:
	printf("\n\tNOT ENOUGH ARGS\n");
	break;
    case 99:
	printf("\n\tCONVERSION NOT FOUND IN CODE TABLE\n");
	break;
    default:
	printf("\nTHERE IS NO SUCH ERROR CODE FOR AZTEC 'C'\n");
  }
  exit(0);
}
ÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿ