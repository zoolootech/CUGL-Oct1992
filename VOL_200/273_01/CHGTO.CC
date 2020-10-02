#include <dir.h>

change_to(char *dir)
/*
ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
³Purpose: To change the current disk drive and directory with        ³
³         one call.                                                  ³
³ Inputs: Char *dir points to directory to change to. This may       ³
³         contain a drive letter if required.                        ³
³Outputs: None.                                                      ³
³                                                                    ³
³ Return:  0 = successful.                                           ³
³         -1 = directory not found.                                  ³
ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
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