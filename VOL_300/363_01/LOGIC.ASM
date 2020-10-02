    ORG     $50
    AND.B   (A1),D5
    AND.W   D2,$900
    AND.L   D2,D4
    ANDI.B  #$AB,$900
    ANDI.W  #$ABCD,D5
    ANDI.L  #$ABCDEF12,(A2)
    ANDI    #$12,CCR    ANDI to CCR
    ANDI    #$ABCD,SR   ANDI to SR
    OR.B    (A1),D5
    OR.W    D2,$900
    OR.L    D2,D4
    ORI.B   #$AB,$900
    ORI.W   #$ABCD,D5
    ORI.L   #$ABCDEF12,(A2)
    ORI     #$12,CCR    ORI to CCR
    ORI     #$ABCD,SR   ORI to SR
    EOR.B   D2,$900
    EOR.W   D3,(A2)
    EOR.L   D6,-(A1)
    EORI.B  #$AB,D2
    EORI.W  #$ABCD,D4
    EORI.L  #$ABCDEF12,$900
    EORI    #$76,CCR    EORI to CCR
    EORI    #$ABCD,SR   EORI to SR
    NOT.B   D4
    NOT.W   (A5)
    NOT.L   $900
    END
