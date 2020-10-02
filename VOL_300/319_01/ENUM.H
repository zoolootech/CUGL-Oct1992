/*
	CPP V5 -- Header file for all enumerations.

	source:  enum.h
	started: June 20, 1986
	version: December 12, 1987; January 19, 1988

	Written by Edward K. Ream.
	This software is in the public domain.

	See the read.me file for disclaimer and other information.
*/

/*
	----------    PARSING    ----------
*/

/*
	Input Codes and Internal Operator Codes:
	Type fields of parse nodes.
*/
typedef enum /* en_tokens */ { NULL_TOK, ERR_TOK,

/*	start of key subenum.				*/
/*	kdecl subenum.					*/

K_AUTO,  K_CHAR, K_CONST, K_DOUBLE,  K_EXTERN,
K_FLOAT, K_INT, K_LONG, K_REGISTER, K_SHORT,
K_STATIC, K_TYPEDEF, K_SIGNED, K_STRUCT, K_UNION,
K_UNSIGNED, K_VOID, K_VOLATILE,

/*	kcontrol subenum.				*/

K_BREAK, K_CASE,
K_CONTINUE, K_DEFAULT, K_DO, K_ELSE, K_ENUM, K_FOR,
K_GOTO, K_IF, K_RETURN, K_SWITCH, K_WHILE,

/*	Remainder of key subenum.			*/

K_ENTRY, K_SIZEOF,

/*	Separator and grouping tokens.			*/
/*	Start of is_op subenum.				*/

SEPARATOR_TOK, NL_TOK,
SEMICOLON_TOK, LBRACK_TOK, LCURLY_TOK, LPAREN_TOK, RBRACK_TOK,
RCURLY_TOK, RPAREN_TOK,

/*	is_ternop.					*/

COLON_TOK, QUESTION_TOK,

/*	Start of is_binop enum.				*/

ARRAY_TOK, ARROW_TOK, DOT_TOK, LAND_TOK, LOR_TOK, COMMA_TOK,

/*	is_assnop subenum.				*/

ASSN_TOK,
AND_ASSN_TOK, DIV_ASSN_TOK, LSHIFT_ASSN_TOK, MINUS_ASSN_TOK, MOD_ASSN_TOK,
OR_ASSN_TOK, PLUS_ASSN_TOK, RSHIFT_ASSN_TOK, STAR_ASSN_TOK, XOR_ASSN_TOK,

/*	is_aop, is_abelian  subenum.			*/

AND_TOK, OR_TOK, PLUS_TOK, STAR_TOK, XOR_TOK,

DIV_TOK, LSHIFT_TOK, MINUS_TOK, MOD_TOK, RSHIFT_TOK,

/* 	end of is_aop.					*/

/*	is_relop subenum.				*/
/*	Final entries of is_binop subenum.		*/

EQUAL_TOK, GE_TOK, GT_TOK, LE_TOK, LT_TOK, NE_TOK,

/*	Unary operators returned by get_token().	*/

DEC_TOK, INC_TOK,

/*	Start of is_unop subenum.			*/

NOT_TOK, TILDE_TOK,

/*	Unary operators created by expr().		*/
/*	End of is_unop subenum.				*/

CAST_TOK, POST_DEC_TOK, POST_INC_TOK, PRE_DEC_TOK, PRE_INC_TOK,

/*	Artificial unary operators; also is_unop	*/

UAND_TOK, UMINUS_TOK, UPLUS_TOK, USTAR_TOK,

/*	Operators with variable operand count		*/
/*	Final entries in is_op subenum.			*/

CALL_TOK,

/*	Class tokens.					*/

CHAR_TOK, EOP_TOK, ID_TOK, INT_TOK, FLOAT_TOK, LONG_TOK, STRING_TOK,

/*	Miscellaneous tokens			*/

DOTS3, LABEL_TOK

} en_tokens;

/*
	Most of the unary operators are specials generated in the parser.
	Only NOT_TOK and TILDE_TOK are unambiguously unary.
*/
#define is_kdecl(n)	(n >= K_AUTO	&& n <= K_VOLATILE)
#define is_kcontrol(n)	(n >= K_BREAK	&& n <= K_WHILE)
#define is_key(n)	(n >= K_AUTO	&& n <= K_SIZEOF)

#define is_op(n)	(n >= SEPARATOR_TOK	&& n <= CALL_TOK)
#define is_binop(n)	(n >= ARRAY_TOK	&& n <= NE_TOK)
#define is_assnop(n)	(n >= ASSN_TOK	&& n <= XOR_ASSN_TOK)
#define is_abelian(n)	(n >= AND_TOK	&& n <= XOR_TOK)
#define is_aop(n)	(n >= AND_TOK	&& n <= RSHIFT_TOK)
#define is_relop(n)	(n >= EQUAL_TOK	&& n <= NE_TOK)
#define is_unop(n)	(n >= NOT_TOK	&& n <= USTAR_TOK)
#define is_argop(n)	(n >= COLON_TOK && n <= CALL_TOK)
