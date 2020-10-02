/* csubst.l */
/* lex source of csubst utility.
This programme is shareware.
If you found this programme useful then send me a colourful postcard
with the words "Happy Birthday" (or equivalent) so that it arrives
at my address around the first of January 1991.

Henri de Feraudy
27 rue Chef de Ville
92140 Clamart 
France
*/
%{
extern int  Mode, Truncation;
extern char *lookup_subst();
#include <stdio.h>
#include "csubst.h"
#define cond_putchar(C) if(Mode == APPLY_SUBSTS || Mode == PRINT_TRUNCATED)putchar(C);

#ifdef yywrap
#undef yywrap
#define yywrap() (!next_yyin())
#endif

void action_keyword(), default_action(), action_symbol(), read_comment();
%}
letter 		[a-zA-Z]
rest		[a-zA-Z0-9_]
identifier	{letter}{rest}*
space		[ \t]*
sinclude	"#"{space}"include"{space}"<"[a-zA-Z\.\\\/0-9]+">"
hexa		[a-fA-F0-9]
begin_define	"#"{space}"define"{space}
begin_cond	"#"{space}("ifdef"|"ifndef")

%start		CPP_MACRO
%%

"/*"			{read_comment();}
{sinclude}		{default_action();}
{begin_cond}		{default_action();BEGIN CPP_MACRO;}
{begin_define}		{default_action();BEGIN CPP_MACRO;}
"#"{space}{identifier}  {default_action();}
"auto"			{action_keyword();}
"break"			{action_keyword();}
"case"			{action_keyword();}
"char"			{action_keyword();}
"const"			{action_keyword();}
"continue"		{action_keyword();}
"default"		{action_keyword();}
"do"			{action_keyword();}
"double"		{action_keyword();}
"else"			{action_keyword();}
"enum"			{action_keyword();}
"extern"		{action_keyword();}
"float"			{action_keyword();}
"for"			{action_keyword();}
"goto"			{action_keyword();}
"if"			{action_keyword();}
"int"			{action_keyword();}
"long"			{action_keyword();}
"register"		{action_keyword();}
"return"		{action_keyword();}
"short"			{action_keyword();}
"signed"		{action_keyword();}
"sizeof"		{action_keyword();}
"static"		{action_keyword();}
"struct"		{action_keyword();}
"switch"		{action_keyword();}
"typedef"		{action_keyword();}
"union"			{action_keyword();}
"unsigned"		{action_keyword();}
"void"			{action_keyword();}
"volatile"		{action_keyword();}
"while"			{action_keyword();}
\"(\\.|[^\\"])*\"	{action_symbol(TYPE_STRING);}
'(\\.|[^\\'])+'		{default_action();}
<CPP_MACRO>{identifier}	{action_symbol(TYPE_MACRO);BEGIN 0;}
{identifier}		{action_symbol(TYPE_IDENTIFIER);BEGIN 0;}
\n			{cond_putchar('\n');}
"0x"{hexa}		{default_action();}
.			{default_action();}
%%

#ifndef yywrap /* flex defines this as a macro */
/* this is the action yylex() executes on end of file 
   if it returns 0 then processing continues.
 */
yywrap()
{
return (!next_yyin());
}
#endif

void action_symbol(type_flag)
symbol_type_t type_flag;
{ 
extern  char O_string_buffer[];
static int reading_original_string = 1;
char *o_string;

  switch(Mode)
  {
  case PRINT_TRUNCATED:
	if(!symbol_lookup(yytext) && type_flag == TYPE_IDENTIFIER)
  	  printf("%.*s", Truncation, yytext);
	else
	  printf("%s", yytext);
	break;

  case APPLY_SUBSTS:
  o_string =  lookup_subst(yytext);
  if(o_string == NULL)
   printf("%s", yytext);
  else
   printf("%s", o_string);
  break;

  case READ_IGNORES:
  case EXTRACT_SYMBOLS:
    symbol_install(yytext, type_flag);
  break;

  case READ_SUBSTS:
    if(reading_original_string)
      {
      strncpy(O_string_buffer,yytext,SYMBOL_SIZE);
      }
    else
      {
      install_subst(O_string_buffer,yytext);
      }
    reading_original_string = !reading_original_string;
  }

}   


void read_comment()
{
int c, previous;

previous = '*';

if(Mode == APPLY_SUBSTS || Mode == PRINT_TRUNCATED)
   printf("/*");

for(;;){
	c = input();
	if(c == EOF)
	  fatal("EOF in comment");
        else 
	cond_putchar(c);
	if(c == '/'){
	  if(previous == '*')
	    break;
	  }
	previous = c;
	}

}

void default_action()
{
if(Mode == APPLY_SUBSTS || Mode == PRINT_TRUNCATED)
  printf("%s", yytext);
}

void action_keyword()
{
if(Mode == READ_SUBSTS)
  fatal1("Cannot use %s in a replacement", yytext);
if(Mode == APPLY_SUBSTS || Mode == PRINT_TRUNCATED)
   printf("%s", yytext);
}

/* end of file */

