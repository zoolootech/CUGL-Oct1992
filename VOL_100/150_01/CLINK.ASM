         name      clink
         page      55,132
         title     'CLINK - Load and Link Graphics Characters'
         assume    cs:cseg
;
; CLINK - Load and Link Graphics Character Table
;
; Original by Ray Duncan, published in DDJ #74
; Revised by Patrick Banchy, 1249 Park Ave. #5C, NYC
;
; The IBM PC allows the user to define the meanings of the
; characters in the range 80H-FFH in the graphics modes.
;
; This program when first called will allocate the 1 KB of
; memory needed for the table.  Subsequent calls will load
; the table specified in the invocation into memory.
;
fcb      equ       05ch      ;default file control block
;
eom      equ       '$'       ;literal ending of string
cr       equ       13        ;ASCII carriage return
lf       equ       10        ;ASCII line feed
;
cseg     segment   para public 'CODE'
         org       100h

clink:                       ;entry from PC-DOS
         xor       ax,ax     ;see if table has been
         mov       ds,ax     ;previously allocated
         mov       bx,07ch   ;offset of vector
                             ;pick up address of
                             ;table in DS:DX
         lds       dx,dword ptr [bx]
         mov       ax,ds
         or        ax,dx     ;have we been here before?
         jnz       not_1st   ;yes,so read the table
                             ;no,set up table
         mov       ax,cs     ;address (restore proper
         mov       ds,ax     ;contents of DS first)
         xor       dx,dx
         mov       ah,37     ;using DOS Set Interrupt
         mov       al,1fh    ;call
         int       21h
                             ;tell the operator whats up
         mov       dx,offset nxt_job
         mov       ah,9
         int       21h
                             ;save 1 kbytes for the
         mov       dx,400h   ;table, terminate but
         int       27h       ;stay resident.

not_1st:                     ;read in graphics table
         mov       ah,26     ;first set DTA address
         int       21h
         mov       ax,cs     ;restore DS
         mov       ds,ax
         mov       dx,offset fcb
         mov       ah,15     ;try and open file
         int       21h
         or        al,al     ;does it exist?
         jz        file_ok   ;yes,proceed
                             ;no,warn operator
         mov       dx,offset Boo_boo
         mov       ah,9
         int       21h
         mov       ah,0      ;return to PC-DOS
         int       21h

file_ok:                     ;file exists,read table
         mov       bx,offset fcb
                             ;set record size = 1024
         mov       word ptr 14 [bx],400h
                             ;set current rec=zero
         mov       byte ptr 32 [bx],0
         mov       dx,offset fcb
         mov       ah,20     ;sequential read
         int       21h
         mov       dx,offset loaded
         mov       ah,9      ;tell operator load
         int       21h       ;was successful
         mov       ah,0      ;and return to PC-DOS
         int       21h

;
; messages for console
;
Boo_boo  db        cr,lf,'No such file',cr,lf,eom
Loaded   db        cr,lf,'Character table loaded',cr,lf,eom
Nxt_job  db        cr,lf,'Memory and Link for table initialized,'
         db        cr,lf,'Rerun to load the table',cr,lf,lf,eom

cseg     ends

         end       clink

 À>�,/t</t<\Ê\ �	���-u�;�!r���?�] � �2�Ë6�-N�D����t ;�t(���tN��|.t�D�<:t��u��� �;�!