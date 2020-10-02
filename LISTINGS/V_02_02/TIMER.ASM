

.model small

.code


public _readtimer
_readtimer proc
   cli             ; Disable interrupts */
   mov  dx,020h     ; Address PIC ocw3   */
   mov  al,00Ah     ; Ask to read irr    */
   out  dx,al
   mov  al,00h     ; Latch timer 0 */
   out  043h,al
   in   al,dx      ; Read irr      */
   mov  di,ax      ; Save it in DI */
   in   al,040h     ; Counter --> bx*/
   mov  bl,al      ; LSB in BL     */
   in   al,040h
   mov  bh,al      ; MSB in BH     */
   not  bx         ; Need ascending counter */
   in   al,021h     ; Read PIC imr  */
   mov  si,ax      ; Save it in SI */
   mov  al,00FFh    ; Mask all interrupts */
   out  021h,al
   mov  ax,040h     ; read low word of time */
   mov  es,ax      ; from BIOS data area   */
   mov  dx,es:[06Ch]
   mov  ax,si      ; Restore imr from SI   */
   out  021h,al
   sti             ; Enable interrupts */
   mov  ax,di      ; Retrieve old irr  */
   test al,001h     ; Counter hit 0?    */
   jz   done       ; Jump if not       */
   cmp  bx,0FFh     ; Counter > 0x0FF?    */
   ja   done       ; Done if so        */
   inc  dx         ; Else count int req. */
done:
   mov ax,bx   ; set function result */
        ret
_readtimer endp



public _restoretimer
_restoretimer proc
        mov     al,36h
        out     43h,al
        jmp     short $+2
        xor     al,al
        out     40h,al
        jmp     short $+2
        xor     al,al
        out     40h,al
        ret
_restoretimer endp


public _initializetimer
_initializetimer proc
        mov     al,34h
        out     43h, al
        jmp     short $+2
        xor     al,al
        out     40h,al
        jmp     short $+2
        xor     al,al
        out     40h, al
        ret
_initializetimer endp



end

