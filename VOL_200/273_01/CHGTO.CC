#include <dir.h>

change_to(char *dir)
/*
旼컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴커
쿛urpose: To change the current disk drive and directory with        �
�         one call.                                                  �
� Inputs: Char *dir points to directory to change to. This may       �
�         contain a drive letter if required.                        �
쿚utputs: None.                                                      �
�                                                                    �
� Return:  0 = successful.                                           �
�         -1 = directory not found.                                  �
읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴켸
*/
{
char disk;

	if(*(dir + 1) == ':') {
		disk = *dir;
		disk = toupper(disk);
		setdisk(disk - 'A');
	}

	return(chdir(dir));
}
