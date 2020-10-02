// SUBDIR.CPP - carry out a DOS command in all subdirectories of current one
//		to compile use: tcc -ms subdir dblibs.lib
//
static char help[] = 
"SUBDIR  cmd  - program carries out cmd on all subdirs of current dir"
"\n               specify \'SUBDIR # cmd\' to act on subdirs themselves"
"\n               ex: \'SUBDIR erase *.bak\' to erase all backup files"
"\n                   \'SUBDIR # rd \' to remove all empty subdirectories"
"\n by D BLUM\n";


#include <dos.h>
#include <dir.h>
#include <sys\stat.h>
#include <signal.h>

#include "dblib.h"


/*  */


// redefining wmalloc() and wrealloc() allow the Vlist and String routines
//		to work without linking in the whole windows package.
extern "C" void *wmalloc ( size_t n, char *msg )
	{
	void *ptr = malloc ( n );
	_NORMALIZE ( ptr, void* );
	if ( ptr == NULL && msg != NULL )
		{
		puts ( "\nOut of memory" );
		exit (1);
		}
	return ptr;
	}


extern "C" void *wrealloc  ( void *block,  size_t size,  char *errmsg )
	{
	void *ptr = realloc ( block, size );
	_NORMALIZE ( ptr, void* );
	if ( ptr == NULL && errmsg != NULL )
		{
		puts ( "\nOut of memory" );
		exit (1);
		}
	return ptr;
	}




static void dirVlist ( Vlist &vl );		// create Vlist of subdirectories
static void process_dir ( void );		// loop thru subdirs and this dir
static String command="";
static int  dir_flag =0;				// command acts on subdirs 

static void ctrlbrk ( int sig )			// control break handler
	{
	exit (sig);
	return;
	}


main ( int argc, char **argv )
	{
	if ( argc == 1 )
		{
		puts ( help );
		exit (1);	
		}
	if ( SIG_ERR == signal ( SIGINT, ctrlbrk ) )
		{
		puts ( "ERROR setting control-break status" );
		exit (1);
		}
		
	if ( argv[1][0] == '#' )
		{
		dir_flag = ON;
		argv[1] = " ";		// skip past the # 
		}
		
	// BUILD COMMAND
	for ( int narg=1; narg<argc; ++narg )
		{
		command += argv[narg];
		command += " ";
		}		
		
	process_dir ();
	
	return (0);	
	}


static void process_dir (void)
	{
	Vlist vl;
	char  cwd[MAXPATH +1];
	String  msg;
	String  full_cmd;
	
	getcwd ( cwd, MAXPATH );
	
	dirVlist (vl);		// list all dirs in the current dir;

	if ( vl.count() > 0 )
	for ( char **newdir= vl; *newdir != NULL; ++newdir )
		{
		chdir ( *newdir );
		process_dir ( );
		chdir ( cwd );
		} 
	
	msg  =  "\nSUBDIR:    " ;
	msg += cwd;
	msg += "> ";
	full_cmd = command;
	if ( dir_flag )
		{
		full_cmd += cwd;	
		chdir ( ".." );		// work in parent to act on this directory
		} 
	msg += full_cmd;	
	puts ( msg );

	system( full_cmd );
	
	return;		// process_dir ()	
	}
	
static void dirVlist ( Vlist &vl )
	{
	char far *dta;
	struct ffblk  ffb;
	char *dirname;	
	int retcode;	
	
	dta =getdta();
	
	for ( 	retcode = findfirst ( "*.*", &ffb, FA_DIREC );
			retcode != -1;
			retcode = findnext ( &ffb ) 
		)
			{
			// stack only directories, but avoid current and root.
			if ( ( ffb.ff_attrib & FA_DIREC ) && ( ffb.ff_name[0] != '.' ) )
				{
				vl.push ( ffb.ff_name );
				}
			}	
	
	setdta ( dta );
	return;
	}