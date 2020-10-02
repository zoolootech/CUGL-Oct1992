/*@******************************************************************/
/*@                                                                 */
/*@ readabs - reads numsecs sectors from the given                  */
/*@        drive starting at logical sector logsec                  */
/*@        into buffer buff.  It uses DOS interrupt                 */
/*@        25h bypassing the normal DOS I/O controls.               */
/*@                                                                 */
/*@   Usage:  readabs(drive, numsecs, logsec, buff);                */
/*@       where drive is the NUMBER of the required                 */
/*@                 drive (e.g. C is 0x03).                         */
/*@             numsecs is the number of sectors to                 */
/*@                 be read.                                        */
/*@             logsec is the starting logical sector.              */
/*@             buff is a pointer to a buffer into                  */
/*@                  which readabs will read.                       */
/*@                                                                 */
/*@ It returns 0 if successful, a 16 bit value otherwise:           */
/*@        Lower Byte              Upper Byte                       */
/*@        0  Write Protect        80h Attachment Failed to Respond */
/*@        1  Unknown Unit         40h Seek Operation Failed        */
/*@        2  Not Ready            08h Bad CRC on Diskette Read     */
/*@        3  Unknown Command      04h Requested Sector Not Found   */
/*@        4  Data Error (CRC)     03h Write Protect Error          */
/*@        5  Bad Request Length   02h Error other than types listed*/     
/*@        6  Seek Error                                            */
/*@        7  Unknown Media Type                                    */
/*@        8  Sector not Found                                      */
/*@        9  <printer only>                                        */
/*@       10  Write Fault                                           */ 
/*@       11  Read Fault                                            */ 
/*@       12  General Failure                                       */
/*@                                                                 */
/*@******************************************************************/

int ldrive, lnsecs, llogsec;  /* static variables to make assembler */
char *lbuff;                  /* code simpler */


int readabs(drive, numsecs, logsec, buff)
int drive, numsecs, logsec;
char *buff;


{
    ldrive  = drive;         /* copy to easily accessed place */
    lnsecs  = numsecs;
    llogsec = logsec;
    lbuff   = buff;

#asm
        PUSH    BX
        PUSH    CX
        PUSH    DX
        PUSH    DI
        PUSH    SI
        PUSH    BP
        MOV     AX,word ldrive_
        MOV     CX,word lnsecs_
        MOV     DX,word llogsec_
        MOV     BX,word lbuff_
        INT     25H
        JC      RDASM1
        MOV     AX,0
RDASM1: NOP
        POPF
        POP     BP
        POP     SI
        POP     DI
        POP     DX
        POP     CX
        POP     BX
#endasm
        return;
}
