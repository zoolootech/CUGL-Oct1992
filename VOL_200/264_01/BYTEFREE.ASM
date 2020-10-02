; bytefree.asm
; Works with Aztec C v3.20d.
; David MacKenzie
; 09/21/87

codeseg segment para public 'code'
dataseg segment para public 'data'
dataseg ends
	assume cs:codeseg,ds:dataseg,es:dataseg,ss:dataseg

codeseg segment

; long disk_bytes_free(drive)
; char drive;
; Returns the number of bytes free on drive, which can be in either upper
; or lower case.
; If drive is \0, the current drive will be used.
; Returns -1 if drive is illegal.
	public disk_bytes_free_
disk_bytes_free_ proc	near
	push	bp			; save bp on stack
	mov	bp, sp			; save sp in bp
	mov	dl, byte ptr 4[bp]
	cmp	dl, 0			; 0 -> 0 (default drive)
	je	dbf_gds
	cmp	dl, 'Z'			; uppercase have lower ASCII values
	jg	dbf_lowercase
	sub	dl, 40h			; map uppercase to binary, A -> 1
	jmp	dbf_gds
dbf_lowercase:
	sub	dl, 60h			; map lowercase to binary, a -> 1
dbf_gds:
	mov	ah, 36h			; get disk space
	int	21h
	cmp	ax, 0ffffh
	je	dbf_err			; bad drive specifier
; returns from get disk space call:
; ax = sectors per allocation unit
; bx = number of free allocation units on drive
; cx = bytes per sector
; (dx = total number of allocation units on drive)
	mul	cx			; free disk bytes = ax * bx * cx
	mul	bx			; dxax = free disk bytes, hi-lo
	mov	sp, bp			; restore sp from bp
	pop	bp			; restore bp from stack
	ret
dbf_err:
	mov	dx, 0ffffh
	mov	sp, bp
	pop	bp
	ret
disk_bytes_free_ endp
    
codeseg ends
dataseg segment para public 'data'
dataseg ends
	end
