#define	PRIVATE	static
#define	PUBLIC	extern
#define	NULL	0
#define	VOID	int
#define	BOOLEAN	int

VOID _chain(program, args)
char *program, *args ;
	{
	char *path ;
	PRIVATE char *Full_Path_Name() ;

	if (path = Full_Path_Name(program))
		{
		chain(path, args)) ;
		}
	}


int _exec(program, args)
char *program, *args ;
	{
	char *path ;
	PRIVATE char *Full_Path_Name() ;

	if (path = Full_Path_Name(program))
		{
		return (exec(path, args)) ;
		}
	return 255 ;
	}

PRIVATE char *Full_Path_Name(file)
char *file ;
	{
	char cur_path[200], sav_dir[80], *dir, *ptr, program[80] ;
	static char path[80] ;
	int sav_drv ;
	PRIVATE char *Strip_Drive() ;
	PRIVATE VOID Get_Cur_Path(), Get_Cur_Dir() ;
	PRIVATE VOID Set_Cur_Drv(), Set_Cur_Dir() ;
	PRIVATE int Get_Cur_Drv() ;
	PRIVATE BOOLEAN Found() ;

	/* convert program name to upper case */
	strcpy(program, file) ;
	for (ptr = program; *ptr; ptr++)
		{
		*ptr = toupper(*ptr) ;
		}

	/* look in current directory first */
	if (Found(program))
		{
		strcpy(path, program) ;
		return path ;
		}

	/* save context */
	Get_Cur_Dir(sav_dir) ;
	sav_drv = Get_Cur_Drv() ;

	/* look for COMSPEC in environment */
	if (!strcmp(program, "COMMAND.COM") && (dir = Get_Env_Str("COMSPEC")))
		{
		if (Found(dir))
			{
			strcpy(path, dir) ;
			Set_Cur_Drv(sav_drv) ;
			Set_Cur_Dir(sav_dir) ;
			return path ;
			}
		}

	Get_Cur_Path(cur_path) ;
	if (!*cur_path)
		{
		return 0 ;
		}

	dir = cur_path ;
	do
		{
		if (ptr = index(dir, ';'))
			{
			*ptr++ = NULL ;
			}
		dir = Strip_Drive(dir, sav_drv) ;

		if (*dir == '\\')
			{
			Set_Cur_Dir("\\") ;
			dir++ ;
			}
		else
			{
			Set_Cur_Dir(sav_dir) ;
			}
		Set_Cur_Dir(dir) ;

		if (Found(program))
			{
			path[0] = 'A' + Get_Cur_Drv() ;
			path[1] = ':' ;
			Get_Cur_Dir(&path[2]) ;
			if (path[3])
				{
				strcat(path, "\\") ;
				}
			strcat(path, program) ;
			Set_Cur_Drv(sav_drv) ;
			Set_Cur_Dir(sav_dir) ;
			return path ;
			}
		dir = ptr ;
		}
	while (*ptr) ;

	/* restore context */
	Set_Cur_Drv(sav_drv) ;
	Set_Cur_Dir(sav_dir) ;

	return 0 ;
	}

PRIVATE VOID Get_Cur_Path(cur_path)
char *cur_path ;
	{
	PRIVATE char *Get_Env_Str() ;
	char *ptr ;

	*cur_path = NULL ;
	if (ptr = Get_Env_Str("PATH"))
		{
		strcpy(cur_path,ptr) ;
		}
	}

PRIVATE char *Get_Env_Str(var)
char *var ;
	{
	PUBLIC VOID chain() ;
	unsigned offset = 0 ;
	char *ptr ;
	static char bfr[200] ;
	extern unsigned _pcb ;	/* Pgm Ctl Blk: contents is segment of PSP */
	union
		{
		unsigned seg ;	/* environment segment */
		char byte[2] ;	/* unpacked form */
		} env ;

	/* retrieve paragraph address of environment strings */
	env.byte[0] = _peek(0x2C, _pcb) ;
	env.byte[1] = _peek(0x2D, _pcb) ;

	while (_peek(offset, env.seg))
		{
		ptr = bfr ;
		do /* copy string */
			{
			} while (*ptr++ = toupper(_peek(offset++, env.seg))) ;
		*(ptr = index(bfr, '=')) = NULL ;
		if (!strcmp(bfr, var))
			{
			return ptr + 1 ;
			}
		}
	return 0 ;
	}

PRIVATE VOID Get_Cur_Dir(bfr)
char *bfr ;
	{
	PUBLIC int _rax, _rdx, _rds, _rsi ;

	*bfr = '\\' ;
	_rax = 0x4700 ;
	_rsi = bfr + 1 ;
	_rds = _showds() ;
	_rdx = 0 ;
	_doint(0x21) ;
	}

PRIVATE VOID Set_Cur_Dir(dir)
char *dir ;
	{
	PUBLIC int _rax, _rdx, _rds ;

	_rax = 0x3B00 ;
	_rdx = dir ;
	_rds = _showds() ;
	_doint(0x21) ;
	}

PRIVATE int Get_Cur_Drv()
	{
	PUBLIC int _rax ;

	_rax = 0x1900 ;
	_doint(0x21) ;
	return _rax & 0xFF ;
	}

PRIVATE VOID Set_Cur_Drv(drive)
unsigned drive ;
	{
	PUBLIC int _rax, _rdx ;

	_rax = 0x0E00 ;
	_rdx = drive ;
	_doint(0x21) ;
	}

PRIVATE BOOLEAN Found(file)
char *file ;
	{
	PUBLIC int _rax, _rdx, _rcx, _rds, _carryf ;
	char bfr[64] ;

	_rax = 0x1A00 ;
	_rds = _showds() ;
	_rdx = bfr ;
	_doint(0x21) ;

	_rax = 0x4E00 ;
	_rds = _showds() ;
	_rdx = file ;
	_rcx = 0 ;
	_doint(0x21) ;

	return ! _carryf ;
	}

PRIVATE char *Strip_Drive(dir, drv)
char *dir ;
int drv ;
	{
	PRIVATE Set_Cur_Drv() ;

	if (dir[1] == ':')
		{
		Set_Cur_Drv(toupper(*dir) - 'A') ;
		dir += 2 ;
		}
	else
		{
		Set_Cur_Drv(drv) ;
		}
	return dir ;
	}
