// TEXTFILE.CPP  --  routines for manipulating small text files.
//
// readASCIIfile ()
//	
//		this routine reads an ASCII file (text mode) into a buffer.
//		the buffer is allocated based on the size of the ASCII file.
//		extra bytes of padding may be specified, and a maximimum size can be set
//		PARMS:	char *dir, char *fn, char *ext - as used by make_fullfilename()
//				int pad = number of extra bytes to add to the end of the buffer
//							(so you can add more to the file later)
//				NOTE: the buffer is NEVER going to be larger than 32000 bytes.
//				defined in RD_ASCII_MAXBUFFER
//		RETURNS: char *buffer - ptr to the buffer holding the file's contents.
//								must be freed later.
//								IF FILE did not exist, buffer[0] = 0.
//
//	writeASCIIfile()
//		this routine writes a buffer to the named ASCII file.
//	editASCIIfile()
//		edit an ASCII file by user.
//		
#include "dblib.h"

#define RD_ASCII_MAXBUFFER 32000
char *readASCIIfile ( char *dir, char *fn, char *ext, size_t pad )
	{
	FILE *f;
	char *buffer;
	String *fname = make_fullfilename ( dir, fn, ext );
	
	long fsize = filesize ( *fname );
	
	size_t bsize;	
	
	if ( fsize >0 )
		{
		bsize = (size_t) min ( (fsize+pad+1), RD_ASCII_MAXBUFFER );
		buffer = (char *)wmalloc ( bsize, "rdASCII" );
		memset ( buffer, 0 ,bsize );
		if ( NULL!=(f=fopen ( *fname, "rt")))
			{
			fread ( buffer, bsize, 1, f );
			fclose ( f );
			}	
		}
	else
		{
		buffer = (char*) wmalloc ( pad+1, "rdASCII" );
		buffer[0] =0;
		} 
	
	delete fname;
	return buffer;		// readASCIIfile ()
	}
	
void writeASCIIfile ( char *dir, char *fn, char *ext, char *buffer )
	{
	String *fname = make_fullfilename ( dir, fn, ext );
	FILE *f; 
	if ( NULL!= (f= fopen ( *fname, "wt" ) ))
		{
		fputs ( buffer, f );
		fclose (f);
		}
	delete fname;
	return;				// writeASCIIfile ()
	
	}

void editASCIIfile ( char *title, char *dir, char *fn, char *ext, int pad )
	{
	char *buffer = readASCIIfile ( dir, fn, ext, pad );
	
	if ( ESCAPE != wprompts ( "FILE EDIT", title, buffer,
				min (RD_ASCII_MAXBUFFER, (pad+strlen (buffer)) ) ) )
		{
		writeASCIIfile ( dir,fn,ext,buffer );
		}
		
	free (buffer);
	
	return;	
	}	
