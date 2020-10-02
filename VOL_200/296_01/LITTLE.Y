/* This is a short version of the C Grammar, it includes mainly */
/* the productions that had semantic action code attached for the */
/* migrator prototype. The full 400 line grammar is supplied with */
/* the source code for the article by the C Users Journal   */

%token IDENTIFIER CONSTANT STRING_LITERAL SIZEOF
%token PTR_OP INC_OP DEC_OP LEFT_OP RIGHT_OP LE_OP GE_OP EQ_OP NE_OP
%token AND_OP OR_OP MUL_ASSIGN DIV_ASSIGN MOD_ASSIGN ADD_ASSIGN
%token SUB_ASSIGN LEFT_ASSIGN RIGHT_ASSIGN AND_ASSIGN
%token XOR_ASSIGN OR_ASSIGN TYPE_NAME

%token TYPEDEF EXTERN STATIC AUTO REGISTER
%token CHAR SHORT INT LONG SIGNED UNSIGNED FLOAT DOUBLE CONST VOLATILE VOID
%token STRUCT UNION ENUM ELIPSIS RANGE

%token CASE DEFAULT IF ELSE SWITCH WHILE DO FOR GOTO CONTINUE BREAK RETURN

%start program
%%

primary_expr
	: identifier
	| CONSTANT
	| STRING_LITERAL
	| '(' expr ')'
	;

declaration
	: declaration_specifiers ';'
	| declaration_specifiers init_declarator_list ';'
	| TYPEDEF { in_tdef = TRUE; su = FALSE; } tdef ';' { in_tdef = FALSE; }
	;

tdef
	: type_specifier_list opt_decl
	| declarator
	;

type_specifier
	: CHAR
	| SHORT
	| INT
	| LONG
	| SIGNED
	| UNSIGNED
	| FLOAT
	| DOUBLE
	| CONST
	| VOLATILE
	| VOID
	| struct_or_union_specifier  { su = ( su > 0 ? --su : 0); }
	| enum_specifier  { enumflag = FALSE; }
	| TYPE_NAME
	;

struct_or_union_specifier
	: struct_or_union identifier '{' struct_declaration_list '}'
	| struct_or_union '{' struct_declaration_list '}'
	| struct_or_union identifier
	;

struct_or_union
	: STRUCT
	    { su += 1; }
	| UNION
	    { su += 1; }
	;

enum_specifier
	: enum '{' enumerator_list '}'
	| enum identifier '{' enumerator_list '}'
	| enum identifier
	;

enum
	: ENUM { enumflag = TRUE; }
	;

enumerator_list
	: enumerator
	| enumerator_list ',' enumerator
	;

enumerator
	: identifier
	| identifier '=' constant_expr
	;

declarator
	: declarator2
	| pointer declarator2
	;

declarator2
	: identifier
	| '(' declarator ')'
	| declarator2 '[' ']'
	| declarator2 '[' constant_expr ']'
	| declarator2 '(' ')' 
		{
		func_decl = TRUE;
		if ( edit_mode )
		    {
		    write_proto(NOARGS);
		    ed_delete();
		    }
		}
	| declarator2 '(' parameter_type_list ')'
	| declarator2 '(' parameter_identifier_list
		{
		if ( edit_mode )
		    {
		    write_proto(WITH_ARGS);
		    ed_delete();
		    }
		}  ')' 
	;

left_curly
	: '{' {proto_flag=FALSE;}
	;

declaration_list
	: declaration
	| declaration_list declaration
	;

statement_list
	: statement
	| statement_list statement
	;

expression_statement
	: ';'
	| expr ';'
	;

program
	: file { ed_flush(); }
	;

file
	: external_definition
	| file external_definition
	;

external_definition
	: function_definition
	| declaration 
	    {
	    if ( func_decl == TRUE )
		{
		if ( edit_mode )
		    ed_delete();
		func_decl = FALSE;
		}
	    };

function_definition
	: declarator function_body
	| declaration_specifiers declarator function_body
	;

function_body
	: compound_statement
	| declaration_list compound_statement
	;

identifier
	: IDENTIFIER
	    {
	    if ( in_tdef && su == 0 && !enumflag)
		storesym(yytext,0L);  /* save only typedef names */
	    }
	;
%%

#include <stdio.h>
#include "ctocxx.h"

extern char *yytext;
extern int proto_flag;
extern int lineno;
int in_tdef = FALSE;    /* flag used to control storing typedef idents in table */
int su = FALSE;		/* TRUE if just after a struct/union keyword */
int enumflag = FALSE;	/* TRUE if in an enum specifier */
int func_decl = FALSE;
int edit_mode = FALSE;  /* TRUE only when in the original source file, not includes */
extern FILE *efd;

