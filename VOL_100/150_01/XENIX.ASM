title MSDOS 2.00 Function Library for Lattice C
;
;........................................
;.					.
;.	DOS 2.00 Functions for		.
;.	       Lattice			.
;.					.
;.	T. Jennings 23 June 83		.
;.	  created 13 Sept 82		.
;.					.
;........................................
;
;MSDOS 2.00 support for Lattice. These will NOT
;work for version 1.xx of DOS. All support full
;paths. The function names are the same as the
;standard library names with "_x" prepended, to
;accomodate my file system nameing heirarchy.
;
;NOTE: You cannot mix these calls with the 
;Lattice library calls: i.e. open with _xopen()
;and write with open(). You must use ALL or
;NONE.
;
;These functions all use the DOS buffers. 
;
;Detailed info is given in the title block for
;each function. A quick description follows.
;
;handle= _xopen(pathname,access);
;int handle,access;
;char *pathname;
;
;	Open a file. handle returns either 
;the DOS handle, or -1 if error (file not 
;found). Access is: 0 == read only, 1 == write
;only, 2 == read/write.
;
;handle= _xcreate(pathname,access);
;
;	Create a new file, delete any existing
;copy. Access is not used: use 0. Returns the
;handle or -1 if error.
;
;v= _xread(handle,buffer,count);
;v= _xwrite(handle,buffer,count);
;int v,handle,count;
;char *buffer;
;
;	File read or write to an opened or
;created file. reads or writes 'count' bytes
;to the file 'handle', to or from 'buffer'.
;Returns the number of bytes processed: equal
;to 'count' if sucessful.
;
;error= _xclose(handle)
;int error;
;
;	Close an open file. Returns -1 if 
;error. Any buffers are flushed at this point.
;
;_xdelete(pathname);
;
;	Remove the file from the file
;system.
;
;fsize= _fsize(pathname);
;long fsize;
;
;	Returns the size of the file in bytes.
;Returns 0 if no file. Do NOT call inbetween
;calls to _xfind().
;
;found= _xfind(pathname,filename,&fsize,attrib,flag);
;int found,flag,attrib;
;char *pathname,filename[14];
;long fsize;
;
;Search for the specified pathname. flag should
;be 0 for the first call, non-zero for all
;subsequent calls. _xfind() returns true if a
;match was found, and the found file is 
;returned in filename[], in ASCIZ format.
;Attrib is the DOS attributes to match; I
;will not describe that mess here. _xfind()
;returns the size of the file in fsize. (Don't
;forget to pass the address of fsize.)
; For example:
;
;int i;
;char filename[14];
;long fsize;
;
;	i= 0;
;	while (_xfind("\\bin\\*.*",filename,&fsize,0,i)) {
;		printf("File: %14s Size: %lu\n",filename,fsize);
;		++i;
;	}
;	printf("%u matching files.\n",i);
;
;Prints the names of all matching disk files. 
;Any other calls (except_fsize()) can be made
;in between calls to _xfind().
;
dgroup group data
pgroup group prog

prog segment byte public 'prog'

public	_xopen,_xcreate,_xclose
public	_xread,_xwrite
public	_xfind
public	_fsize
public 	_xdelete
assume cs:pgroup,ds:dgroup
;;
;;	handle= _xcreate(name,access)
;;	handle= _xopen(name,access)
;;
;;	int handle;		-1 if error,
;;	int access;		0=r, 1=w, 2=r/w
;;	char *name;		null terminated
;;
;;Open and create functions. The name is a null
;;terminated string. The access byte is passed
;;directly to DOS. All errors are translated to
;;a -1 return value.
;;
_xopen proc near
	mov	ah,61
	jmp	short opncrt
_xcreate proc near
	mov	ah,60

opncrt:	push	bp
	mov	bp,sp
	mov	dx,[bp+4]	;pathname,
	mov	al,[bp+6]	;access,
	xor	bx,bx
	xor	cx,cx
	int	21h		;do it,
	jnc	opncrt1
	mov	ax,-1		;error!
opncrt1:pop	bp
	ret

_xcreate endp
_xopen endp
;;
;;xclose(handle)
;;int handle;
;;
;;Close a handle opened by XOPEN or XCREATE.
;;Returns -1 if close error.
;;
_xclose proc near
	mov	ah,62
	push	bp
	mov	bp,sp
	mov	bx,[bp+4]	;handle,
	int	21h
	pop	bp
	ret

_xclose endp
page
;;
;;	count= xread(handle,buffer,size)
;;	count= xwrite(handle,buffer,size)
;;
;;	int count;	bytes actually r/w
;;	int handle;
;;	char *buffer;
;;	int size;	byte count,
;;
;;	Read or write (size) bytes from the
;;file (handle). The return value is the number 
;;of bytes actually processed.
;;
;;	No text translation is done. All
;;bytes are processed as read or written. No
;;check is done (or is possible) on the buffer
;;size.
;;
_xread proc near
	mov	ah,63
	jmp	short rdwrt
_xwrite proc near
	mov	ah,64

rdwrt:	push	bp
	mov	bp,sp
	mov	bx,[bp+4]	;handle,
	mov	cx,[bp+8]	;count,
	mov	dx,[bp+6]	;buffer,
	int	21h
	pop	bp
	ret

_xwrite endp
_xread endp
page
;;
;;	ret= xfind(path,name,size,attrib,first)
;;	int ret;		0 if no match
;;	char *path;
;;	char *name;		dest name,
;;	long *size;		ptr to file siz
;;	int attrib;		attributes
;;	int first;		0 if 1st time,
;;
;;Find the Nth occurence of pathname. Returns
;;0 when no match. Only the filename portion
;;can contain wildcards. The returned filename
;;does not contain the path portion of the 
;;input string.
;;	Not recursive. Do not call _FSIZE
;;inbetween _XFIND calls.
;;
xfpath	equ	4	;path pointer,
xfname	equ	6	;retnd name,
xfsize	equ	8	;file size ptr,
xfaccess equ	10	;access,
xfflag	equ	12	;first time flag,

_xfind proc near
	push	bp
	mov	bp,sp
	mov	ah,26		;set DMA addr
	mov	dx,offset dgroup:xfbuf
	int	21h		;to buffer,
	test word ptr [bp+xfflag],-1
	mov	ah,78		;do right call,
	jz	xf1
	mov	ah,79		;0 == 1st time,
xf1:	mov	dx,[bp+xfpath]	;path name,
	mov	cx,[bp+xfaccess];access,
	int	21h
	mov	ax,0		;ret if no
	jc	xfr		;match,
;
;Copy the file size in.
;
	mov	bx,[bp+xfsize]	;size ptr,
	mov	ax,fsize
	mov	[bx],ax
	mov	ax,fsize+2
	mov	[bx+2],ax

	mov	di,[bp+xfname]	;dest string,
	mov	si,offset dgroup:fname
	mov	cx,12
	cld
;
;Fix a "slight" XENIX bug: Delete trailing 
;spaces, else it fails OPENs.
;
xf2:	lodsb			;get a byte,
	cmp	al,0		;if null
	je	xf3
	cmp	al,' '		;or space,
	je	xf3		;stop,
	stosb
	loop	xf2		;max 11 chars	
xf3:	mov byte ptr [di],0	;terminate,
	mov	ax,1		;good return.
xfr:	pop	bp
	ret
_xfind endp
page
;;
;;	fsize= _fsize(filename)
;;	long fsize;
;;	char *filename;
;;
;;Return the size of a file, in bytes. Returns
;;0 if file not found. Filename can contain
;;a path.
;;
;;Cannot be called in between any _XFIND calls.
;;
_fsize proc near
	push	bp
	mov	bp,sp
	mov	ah,26		;set DMA addr
	mov	dx,offset dgroup:xfbuf
	int	21h		;to buffer,
	mov	ah,78		;search 1st,
xs1:	mov	dx,[bp+4]	;path name,
	mov	cx,0
	int	21h
	mov	ax,0		;ret if no
	mov	bx,0		;match,
	jc	xsr

	mov	bx,fsize
	mov	ax,fsize+2
xsr:	pop	bp
	ret
_fsize endp
page
;
;error= _xdelete(path);
;int error;
;char *path;
;
_xdelete proc near
	push	bp
	mov	bp,sp
	mov	dx,[bp+4]
	mov	ah,41h
	int	21h
	mov	ax,0
	sbb	ax,0
	pop	bp
	ret
_xdelete endp

prog ends
page
data segment word public 'data'
;
;Structure for the FindFirst and FindNext
;function XFIND. NOT REENTRANT.
;
xfbuf	db	(?)	;search attrib
	db	(?)	;drive,
	db 11 dup (?)	;name,
	dw	(?)	;last ent
	dd	(?)	;DPB,
	dw	(?)	;dir start

	db	(?)	;attrib found,
	dw	(?)	;time?
	dw	(?)	;date?
fsize	dw	(?)	;size low
	dw	(?)	;size hi,
fname	db 13 dup (?)	;packed name,

data ends	

	end
