#include <stdarg.h>
#include <stdio.h>


void xprintf(int row, int col,int attr, va_list arg_list, ...)
/*
��������������������������������������������������������������������Ŀ
�Purpose: Provide for fast formatted output to video screen.         �
�                                                                    �
�                                                                    �
�                                                                    �
� Inputs: row = row to display at.                                   �
�         col = col to display at.                                   �
�         attr = attr to display text with.                          �
�         va_list = format string and arguments, these are exactly   �
�                   the same format as printf requires.              �
�                                                                    �
�Outputs: None                                                       �
�                                                                    �
� Return: None                                                       �
�                                                                    �
�Also see: writef, writef_n, video_type, make_window.                �
�                                                                    �
�Prototype in: tcutil.h                                              �
����������������������������������������������������������������������
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