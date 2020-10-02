D                       [0-9]
L                       [a-zA-Z_]
H                       [a-fA-F0-9]
E                       [Ee][+-]?{D}+
FS                      (f|F|l|L)
IS                      (u|U|l|L)*

%{
#include <stdio.h>
#include "ytab.h"
#include "ctocxx.h"
extern int lineno;
extern int edit_mode;
char file_name[132] = {0};

void count();
void pound();
%}

%%
"/*"                    { comment(); }
"#line"                 { pound(); }
"#"                     { pound(); }
"near"                  { count(); /* ignore */ };
"far"                   { count(); /* ignore */ };
"cdecl"                 { count(); /* ignore */  };
"..."                   { count(); return(INT); /*  ignore ... */ };
"auto"                  { count(); return(AUTO); }
"break"                 { count(); return(BREAK); }
"case"                  { count(); return(CASE); }
"char"                  { count(); return(CHAR); }
"const"                 { count(); return(CONST); }
"continue"              { count(); return(CONTINUE); }
"default"               { count(); return(DEFAULT); }
"do"                    { count(); return(DO); }
"double"                { count(); return(DOUBLE); }
"else"                  { count(); return(ELSE); }
"enum"                  { count(); return(ENUM); }
"extern"                { count(); return(EXTERN); }
"float"                 { count(); return(FLOAT); }
"for"                   { count(); return(FOR); }
"goto"                  { count(); return(GOTO); }
"if"                    { count(); return(IF); }
"int"                   { count(); return(INT); }
"long"                  { count(); return(LONG); }
"register"              { count(); return(REGISTER); }
"return"                { count(); return(RETURN); }
"short"                 { count(); return(SHORT); }
"signed"                { count(); return(SIGNED); }
"sizeof"                { count(); return(SIZEOF); }
"static"                { count(); return(STATIC); }
"struct"                { count(); return(STRUCT); }
"switch"                { count(); return(SWITCH); }
"typedef"               { count(); return(TYPEDEF); }
"union"                 { count(); return(UNION); }
"unsigned"              { count(); return(UNSIGNED); }
"void"                  { count(); return(VOID); }
"volatile"              { count(); return(VOLATILE); }
"while"                 { count(); return(WHILE); }

{L}({L}|{D})*           { count(); return(check_type()); }

0[xX]{H}+{IS}?          { count(); return(CONSTANT); }
0[xX]{H}+{IS}?          { count(); return(CONSTANT); }
0{D}+{IS}?              { count(); return(CONSTANT); }
0{D}+{IS}?              { count(); return(CONSTANT); }
{D}+{IS}?               { count(); return(CONSTANT); }
{D}+{IS}?               { count(); return(CONSTANT); }
'(\\.|[^\\'])+'         { count(); return(CONSTANT); }

{D}+{E}{FS}?            { count(); return(CONSTANT); }
{D}*"."{D}+({E})?{FS}?  { count(); return(CONSTANT); }
{D}+"."{D}*({E})?{FS}?  { count(); return(CONSTANT); }

\"(\\.|[^\\"])*\"       { count(); return(STRING_LITERAL); }

">>="                   { count(); return(RIGHT_ASSIGN); }
"<<="                   { count(); return(LEFT_ASSIGN); }
"+="                    { count(); return(ADD_ASSIGN); }
"-="                    { count(); return(SUB_ASSIGN); }
"*="                    { count(); return(MUL_ASSIGN); }
"/="                    { count(); return(DIV_ASSIGN); }
"%="                    { count(); return(MOD_ASSIGN); }
"&="                    { count(); return(AND_ASSIGN); }
"^="                    { count(); return(XOR_ASSIGN); }
"|="                    { count(); return(OR_ASSIGN); }
">>"                    { count(); return(RIGHT_OP); }
"<<"                    { count(); return(LEFT_OP); }
"++"                    { count(); return(INC_OP); }
"--"                    { count(); return(DEC_OP); }
"->"                    { count(); return(PTR_OP); }
"&&"                    { count(); return(AND_OP); }
"||"                    { count(); return(OR_OP); }
"<="                    { count(); return(LE_OP); }
">="                    { count(); return(GE_OP); }
"=="                    { count(); return(EQ_OP); }
"!="                    { count(); return(NE_OP); }
";"                     { count(); return(';'); }
"{"                     { count(); return('{'); }
"}"                     { count(); return('}'); }
","                     { count(); return(','); }
":"                     { count(); return(':'); }
"="                     { count(); return('='); }
"("                     { count(); return('('); }
")"                     { count(); return(')'); }
"["                     { count(); return('['); }
"]"                     { count(); return(']'); }
"."                     { count(); return('.'); }
"&"                     { count(); return('&'); }
"!"                     { count(); return('!'); }
"~"                     { count(); return('~'); }
"-"                     { count(); return('-'); }
"+"                     { count(); return('+'); }
"*"                     { count(); return('*'); }
"/"                     { count(); return('/'); }
"%"                     { count(); return('%'); }
"<"                     { count(); return('<'); }
">"                     { count(); return('>'); }
"^"                     { count(); return('^'); }
"|"                     { count(); return('|'); }
"?"                     { count(); return('?'); }

[ \t\v\f]               { count(); }
[\n]                    { lineno++; count(); }
.                       {; /* ignore bad characters */ }

%%

int yywrap()
    {
    return(1);
    }

comment()
    {
    char c, c1;

loop:
    while ((c = input()) != '*' && c != 0)
        putchar(c);

    if ((c1 = input()) != '/' && c != 0)
        {
        unput(c1);
        goto loop;
        }

    if (c != 0)
        putchar(c1);
    }

int column = 0;

void count()
    {
    int i;

    for (i = 0; yytext[i] != '\0'; i++)
        if (yytext[i] == '\n')
            column = 0;
        else if (yytext[i] == '\t')
            column += 8 - (column % 8);
        else
            column++;

    stuff();
/* for further debug remove comments from the ECHO macro below */
/*  ECHO; */
    }

/*-------------------------------------------------------------------*/
/* lex_input and unput - routines callable elsewhere since input and  */
/* unput are macros */
lex_input() { return input(); }

lex_unput(ch) int ch; { unput(ch); }

/*-------------------------------------------------------------------*/
/* pound - handles # lines, extracts line number and file name */
void pound()
    {
    char c;
    char buf[132];
    int i = 0;
    int num = 0;
    extern FILE *efd;
    static int first_time = TRUE;
    char cur_file[132];

    while ( ( c=input() ) != '\n' && c != 0 )
        buf[i++] = c;
    buf[i] = '\0';
    if ( (i = sscanf(buf,"%d %s",&num,cur_file)) == 2 )
        {
        if ( first_time )
           {
           strcpy(file_name,cur_file); 
           first_time = FALSE;
           }
        if ( strcmp(file_name,cur_file) == 0 )
            {
            lineno = num;
            edit_mode = TRUE;
            }
        else
            edit_mode = FALSE;
        }
    else
        fprintf(stderr,"#line couldn't convert into a number, %s\n",buf);
    }

/*----------------------------------------------------------------*/
int check_type()
    {
    if ( findsym(yytext) )  /* added by cdh to handle typedef names */
        return(TYPE_NAME);  /* preloaded with int, char etc */
    else
        return(IDENTIFIER);
    }
