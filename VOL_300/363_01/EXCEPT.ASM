    ORG $50
    CHK (A1),D5
    ILLEGAL
    RESET
    STOP    #$fFFF
    TRAP    #$8
    TRAPV
    END
