#include "BDSCIO.H"

#define SECTORS 26
#define FIRST_TRACK 00
#define TRACKS 77

#define DMA 128
#define DMALEN 128

int sector,track,drive,c;
char *p;

main()
{
	for (track=FIRST_TRACK; track<TRACKS; track++) {
		printf("\n****** TRACK %d ******\n", track);
		for(sector=0; sector<SECTORS; sector++) {
			getcpm(1, track, sector);
			for(p=DMA; p<(DMA+DMALEN);p++) {
				if((c= *p)>0177)
					continue;
				if( c<040
				  && c != 012 && c !=011)
					continue;
				putchar(c);
				if(c == 012)
					putchar(015);
			}
		}
	}
}
�
  �C	 �D	 ��
��
�E	A�R����3s�V����r�=  u�� ;D}����ݣ�
�>D	 u	�D	S�[��
 t6�>C	 u�C	�#3ҡ�
�6�
D��
;�
�i�<�&�
����
  ��
3��6�
�t@�ȡ�
�&�
�衝
�+D�3��6�
�t@���� ;�t
.�
s��
K���K�SQR����N���7��s������ ��� �����7ZY[À>D	 u�R�t�s	��������
 tK��	�� 
 ���X�K�����
�t��� ���Non-System disk or disk err/*
	from STDLIB2.C for BDS C 1.41


	This version of _spr adds the '0' padding option (see K&R, section
	7.3) to the printf, etc. functions.
	If a leading 0 is given in the field length for numeric conversions
	the leading spaces are changed to leading 0s. Useful when two
	numbers are to be concatenated for printing, e.g., split octal
	notation: "017.002".
	One side effect of this is that a right justified character or
	string is also allowed to have leading 0s, but K&R's description
	doesn't appear to disallow it, so...
*/

_spr(line,fmt)
char *line, **fmt;
{
	char _uspr(), c, base, *sptr, *format;
	char wbuf[MAXLINE], *wptr, pf, ljflag;

/*
	the first modification: define a variable to hold the
	padding character.
*/
	char padchr;

	int width, precision,  *args;

	format = *fmt++;    /* fmt first points to the format string	*/
	args = fmt;	    /* now fmt points to the first arg value	*/

	while (c = *format++)
	  if (c == '%') {
	    wptr = wbuf;
	    precision = 6;
	    ljflag = pf = 0;

	    if (*format == '-') {
		    format++;
		    ljflag++;
	     }
/*
	here's the additional logic: sim