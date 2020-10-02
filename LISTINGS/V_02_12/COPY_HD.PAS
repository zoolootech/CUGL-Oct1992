{********************************************************}

PROGRAM COPY_HIGH_DENSITY ;

                  {10 SEPT 88 CJB}

{COPIES 1.2 MEG DRIVE TO A SINGLE FILE ON HARD DISK, THEN
 COPIES FILE FROM HARD DISK TO ANOTHER DISK IN THE 1.2
 MEG DRIVE.  PURPOSE IS TO COPY A DISK WITH A BLOWN DIRECTORY }

{********************************************************}
uses CRT, dos ;

TYPE
BUFF = ARRAY[0..$11ff] OF BYTE ;

{********************************************************}

CONST
DISK              = $13 ;
READ_DISK         =   2 ;
WRITE_DISK        =   3 ;
HIGH_DEN          =   3 ;
HEADS             =   2 ;
TRACKS_PER_DISK   =  80 ;
SECTORS_PER_TRACK =  15 ;

{********************************************************}

VAR
HEAD, TRACK, SECTOR : BYTE ;
I, J, K, L, N, M : INTEGER ;
TRANSFER : FILE OF BUFF ;
BUFFER : BUFF ;
ASK : STRING[80] ;

REGS : REGISTERS ;

{********************************************************}

BEGIN {MAIN}
    TEXTCOLOR(7) ;
    WRITELN(#7) ;
    WRITE('INSERT OLD DISK IN DRIVE <A> AND PRESS <RETURN> ') ;
    READLN( ASK) ;
    WRITELN ;

    IF HIGH_DEN = 3 THEN
        WITH REGS DO
            BEGIN
                AL := HIGH_DEN ;
                AH := $17 ;
                DL := 0 ;
                INTR(DISK, REGS) ;
            END ; {WITH REGS}
    ASSIGN(TRANSFER, 'C:TRANSFER.$$$') ;
    REWRITE( TRANSFER) ;

    FOR TRACK := 0 TO TRACKS_PER_DISK - 1 DO
      BEGIN
        IF TRACK MOD 10 = 0 THEN
            BEGIN
               WRITELN ;
               WRITE(TRACK DIV 10, '0:') ;
            END ;
            WRITE(TRACK MOD 10) ;

        FOR HEAD := 0 TO HEADS - 1 DO
                BEGIN
                    WITH REGS DO
                        BEGIN
                            AL := SECTORS_PER_TRACK ; {ONE TRACK}
                            AH := READ_DISK ;
                            CL := 1 ;
                            CH := TRACK ;
                            DH := HEAD ;
                            DL := 0 ; {DRIVE A}
                            ES := SEG(BUFFER) ;
                            BX := OFS(BUFFER) ;
                            INTR(DISK, REGS) ;

                            IF (AL <> 0) THEN
                                   BEGIN
                                       TEXTCOLOR(143) ;
                                       GOTOXY(WHEREX-1,WHEREY) ;
                                       WRITE(TRACK MOD 10, #7) ;
                                       TEXTCOLOR(7) ;
                                       FOR SECTOR := 1 TO SECTORS_PER_TRACK DO
					   BEGIN
                                           N := 0 ;
                                           REPEAT
                                               N := N + 1 ;
                                               AL := 1; {ONE SECTOR}
                                               AH := READ_DISK ;
                                               CL := SECTOR ;
                                               CH := TRACK ;
                                               DH := HEAD ;
                                               DL := 0 ; {DRIVE A}
                                               ES := SEG(BUFFER) ;
                                               BX := OFS(BUFFER) +
                                                     (SECTOR - 1) * 512 ;
                                               INTR(DISK, REGS) ;
                                           UNTIL (AL = 0) OR (N = 5) ;
					   END 
                                   END

                        END ;
                    WRITE(TRANSFER, BUFFER) ;
                END ;{ HEAD, SECTOR}
      END ;{TRACK}
    CLOSE( TRANSFER) ;

    {**********************************************************}

    WRITELN ; WRITELN(#7) ;
    WRITE('INSERT NEW DISK IN DRIVE <A> AND PRESS <RETURN> ') ;
    READLN( ASK) ;
    WRITELN ;
    RESET( TRANSFER) ;
    IF HIGH_DEN = 3 THEN
        WITH REGS DO
            BEGIN
                AL := HIGH_DEN ;
                AH := $17 ;
                DL := 0 ;
                INTR(DISK, REGS) ;
            END ; {WITH REGS}

    FOR TRACK := 0 TO TRACKS_PER_DISK - 1 DO
      BEGIN
        IF TRACK MOD 10 = 0 THEN
            BEGIN
               WRITELN ;
               WRITE(TRACK DIV 10, '0:') ;
            END ;

        FOR HEAD := 0 TO HEADS - 1 DO
                BEGIN
                    READ(TRANSFER, BUFFER) ;
                    WITH REGS DO
                        BEGIN
                            AL := SECTORS_PER_TRACK; {ONE track}
                            AH := WRITE_DISK ;
                            CL := 1 ;
                            CH := TRACK ;
                            DH := HEAD ;
                            DL := 0 ; {DRIVE A}
                            ES := SEG(BUFFER) ;
                            BX := OFS(BUFFER) ;
                        END ;
                       INTR(DISK, REGS) ; {SKIP BOOT SECTOR}
                END ; {TRACK, HEAD}
                WRITE(TRACK MOD 10) ;
      END ; {TRACK}

    CLOSE( TRANSFER) ;
    ERASE( TRANSFER) ;
    WRITELN ; WRITELN(#7) ;
    WRITELN('***ALL DONE!') ; WRITELN ;

END . {MAIN}
