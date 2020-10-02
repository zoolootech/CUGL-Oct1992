#include <stdarg.h>
#include <stdio.h>


void xprintf(int row, int col,int attr, va_list arg_list, ...)
/*
ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
³Purpose: Provide for fast formatted output to video screen.         ³
³                                                                    ³
³                                                                    ³
³                                                                    ³
³ Inputs: row = row to display at.                                   ³
³         col = col to display at.                                   ³
³         attr = attr to display text with.                          ³
³         va_list = format string and arguments, these are exactly   ³
³                   the same format as printf requires.              ³
³                                                                    ³
³Outputs: None                                                       ³
³                                                                    ³
³ Return: None                                                       ³
³                                                                    ³
³Also see: writef, writef_n, video_type, make_window.                ³
³                                                                    ³
³Prototype in: tcutil.h                                              ³
ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
*/
{
    va_list arg_ptr;
    char *format;
    char output[161];

    va_start(arg_ptr, arg_list);
    format = arg_list;
    vsprintf(output, format, arg_ptr);
	writef(row,col,attr,output);

	va_end(arg_ptr);
}

