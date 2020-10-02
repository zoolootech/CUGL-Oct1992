* Test source file. Includes all mnemonics listed in Motorola "MC68020
* 32-bit Microprocessor User's Manual.

* Instructions are tested with one, usually the simplest, operand(s). The
* purpose of this test is to check that all mnemonics are present in the
* mnemonics table (insttabl.c)

        abcd        d0,d1
        add         d0,d1
        adda        a0,a1
        addi        #0,d1
        addq        #1,d1
        addx        d0,d1
        and         d0,d1
        andi        #0,d1
        asl         d0,d1
        asr         d0,d1

        bcc         $1000
        bcs         $1000
        beq         $1000
        bge         $1000
        bgt         $1000
        bhi         $1000
        ble         $1000
        bls         $1000
        blt         $1000
        bmi         $1000
        bne         $1000
        bpl         $1000
        bvc         $1000
        bvs         $1000

        bchg        d0,d1
        bclr        d0,d1
        bfchg       d0{d1:1}
        bfclr       d0{d1:1}
        bfexts      d0{d1:1},d1
        bfextu      d0{d1:1},d1
        bfffo       d0{d1:1},d1
        bfins       d0,d1{d1:1}
        bfset       d0{d1:1}
        bftst       d0{d1:1}
        bkpt        #1
        bra         $1000
        bset        d0,d1
        btst        d0,d1
        callm       #1,$1000
        cas         d0,d1,$1000
        cas2        d0:d1,d2:d3,(a0):(a1)
        chk         d0,d1
        chk2        (a0),d0
        clr         d0
        cmp         d0,d1
        cmpa        d0,a1
        cmpi        #1,d0
        cmpm        (a0)+,(a1)+
        cmp2        (a0),d0

        dbcc        d0,$1000
        dbcs        d0,$1000
        dbeq        d0,$1000
        dbf         d0,$1000
        dbge        d0,$1000
        dbgt        d0,$1000
        dbhi        d0,$1000
        dble        d0,$1000
        dbls        d0,$1000
        dblt        d0,$1000
        dbmi        d0,$1000
        dbne        d0,$1000
        dbpl        d0,$1000
        dbt         d0,$1000
        dbvc        d0,$1000
        dbvs        d0,$1000

        divs        d0,d1
        divsl       $1000,d0:d1
        divu        d0,d1
        divul       $1000,d0:d1
        eor         d0,d1
        eori        #0,d1
        exg         d0,d1
        ext         d0
        extb        d0
        illegal
        jmp         $1000
        jsr         $1000
        lea         $1000,a0
        link        a0,#1
        lsl         d0,d1
        lsr         d0,d1
        move        d0,d1
        movea       d0,a1
        movec       d0,sfc
        movem       d0,$1000
        movep       d0,(1,a0)
        moveq       #1,d0
        moves       d0,$1000
        muls        d0,d1
        mulu        d0,d1
        nbcd        $1000
        negx        $1000
        nop
        not         d0
        or          d0,d1
        ori         #0,d1
        pack        d0,d1,#1
        pea         $1000
        reset
        rol         d0,d1
        ror         d0,d1
        roxl        d0,d1
        roxr        d0,d1
        rtd         #0
        rte
        rtm         d0
        rtr
        rts
        sbcd        d0,d1
        scc         d0
        scs         d0
        seq         d0
        sf          d0
        sge         d0
        sgt         d0
        shi         d0
        sle         d0
        sls         d0
        slt         d0
        smi         d0
        sne         d0
        spl         d0
        st          d0
        svc         d0
        svs         d0
        stop        #1
        sub         d0,d1
        suba        d0,a1
        subi        #1,d0
        subq        #1,d0
        subx        d0,d1
        swap        d0
        tas         d0
        trap        #1

        trapcc      #1
        trapcs      #1
        trapeq      #1
        trapf       #1
        trapge      #1
        trapgt      #1
        traphi      #1
        traple      #1
        trapls      #1
        traplt      #1
        trapmi      #1
        trapne      #1
        trappl      #1
        trapt       #1
        trapvc      #1
        trapvs      #1

        trapv
        tst			d0
        unlk		a0
        unpk		d0,d1,#1
        end
