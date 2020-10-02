/*
 * build pcboard dir.lst
 */

#if 0
123456789012345678901234567890
J:\BBS\PCB\ADA                
12345678901234567890123456789012345678901234567890123456789012345
E:\MSDOS\ADA\                                                    
#endif

#include <stdio.h>

/*
 * directory file, directory, directory name, sort type
 */
void _Cdecl
main(int argc, char *argv[]) {
	char buf[80];
	
	strcpy(buf, strlwr(argv[3]));
	strcat(buf, " files");
	
	printf("%-30s%-30s%-35s", argv[1], argv[2], buf);
}
