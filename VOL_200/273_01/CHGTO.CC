#include <dir.h>

change_to(char *dir)
/*
��������������������������������������������������������������������Ŀ
�Purpose: To change the current disk drive and directory with        �
�         one call.                                                  �
� Inputs: Char *dir points to directory to change to. This may       �
�         contain a drive letter if required.                        �
�Outputs: None.                                                      �
�                                                                    �
� Return:  0 = successful.                                           �
�         -1 = directory not found.                                  �
����������������������������������������������������������������������
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