* Miscellaneous instructions' tests
        bra.s       bkpoint
        bkpt        #0
        bkpt        #7
        bkpt        #-1             immediate operand must be within 0 - 7
bkpoint bkpt        #-8             immediate operand must be within 0 - 7
*  Missing operand, no comment to take as operand
        bkpt
*
        bkpt                        missing operand. Comment taken as operand
        bkpt.b      #5              no size allowed
        bkpt.w      #5              no size allowed
        bkpt.l      #5              no size allowed
        bkpt.k      #5              superfluous characters in operator field
        bkpt        #5abc           superfluous characters in operand
*
        callm.b     #0,(a0)         no size allowed
        callm.w     #0,(a0)         no size allowed
        callm.l     #0,(a0)         no size allowed
        callm.k     #0,(a0)         superfluous characters in operator field
        callm       #255,([$55555555,PC],d7,$aaaaaaaa)
        callm       #-1,(a0)        source must be within 0 - 255
        callm       #256,(a0)       source must be within 0 - 255
        callm       #128,$1000.w
        callm       #128,$10000000.w
        callm       #128,$10000000.l
        callm       #128,$1000.l
        callm       #128,(a0)abc            superfluous characters
*
        cas         d1,d2,(a0)+
        cas.b           d1,d2,(a0)+
        cas.w       d1,d2,(a0)+
        cas.l       d1,d2,(a0)+
        cas         d0,d7,([$55555555,a3],a5.l*8,$aaaaaaaa)
        cas         a3,d4,-(a0)             address register illegal
        cas         d3,a5,(128,a3)          address register illegal
        cas         d1,d2,d3                illegal effective address
        cas         d1.d2,(a0)              dot instead of comma
        cas         d1,d2,(a0)abc           superfluous characters in operand
        cas2        d1:d2,d3:d4,(d5):(d6)
        cas2.b      d1:d2,d3:d4,(d5):(d6)   illegal size
        cas2.w      d1:d2,d3:d4,(d5):(d6)
        cas2.l      d1:d2,d3:d4,(d5):(d6)
        cas2.k      d1:d2,d3:d4,(d5):(d6)   superfluous characters in operand
        cas2        d1:d2,d3:d4,(d5):(d6)ab superfluous characters in operator
*
        chk2.b      (a3),a0
        chk2.w      (a3),a0
        chk2        (a3),a0
        chk2.l      (a3),a0
        chk2.k      (a3),a0             invalid size letter
        chk2        (a3)+,a0            relative postincremented illegal
        chk2        (128,a5),a3
        chk2        ([127,a3],d5.l*4,-128),d7
        chk2        ([127,a3],d5.l*4,-128),d7abc superfluous characters
*
        cmp2.b      (a3),a0
        cmp2.w      (a3),a0
        cmp2        (a3),a0
        cmp2.l      (a3),a0
        cmp2.k      (a3),a0             invalid size letter
        cmp2        (a3)+,a0            relative postincremented illegal
        cmp2        (128,a5),a3
        cmp2        ([127,a3],d5.l*4,-128),d7
        cmp2        ([127,a3],d5.l*4,-128),d7abc superfluous characters
*
        bra.s       divide
        divs        ([127,a3],d5.l*4,-128),d7
        divs.w      ([127,a3],d5.l*4,-128),d7
        divs.l      ([$55555555,PC,d1.w*2],$aaaaaaaa),d1
        divs.l      (a5)+,d0:d1
        divsl.l     (a5)+,d2:d3
divide  divu.w      ([127,a3,d5.l*4],-128),d0
        divu.l      ([$55555555,PC,d1.w*2],$aaaaaaaa),d1
        divu.l      (a5)+,d4:d5
        divul.l     (a5)+,d6:d7
        divs.k      (128,a7),d0         invalid size letter
        divs.b      (128,a7),d0         invalid size
        divs        a7,d0               invalid addressing mode
        divs        -(a7),d0:d1         invalid format
        divs        -(a7),d0xyz         superfluous characters
*
        extb.l      d0
        extb.l      d7
        extb.b      d0                  invalid size
        extb.l       a0                  invalid register
*
        bra.s       packl
        pack        -(a0),-(a1),#32767
        pack        d3,d5,#-32768
        unpk        -(a2),-(a6),#-256
        unpk        d6,d7,#0
packl   pack        a0,a1,#0            invalid source mode
        pack        d0,d1               missing operand
        pack.b      d0,d1,#0            unsized operation
        pack.w      d0,d1,#0            unsized operation
        pack.l      d0,d1,#0            unsized operation
        pack.k      d0,d1,#0            invalid size letter
        pack        d0,d1,#32768        exceeds 16-bit signed
        pack        d0,d1,#-32769       exceeds 16-bit signed
        unpk        -(a2),-(a6),#-256
        unpk        d6,d7,#0
*
        rtm         d0
        rtm         d3
        rtm         a7
        rtm.b       d0                  unsized instruction
        rtm.w       d0                  unsized instruction
        rtm.l       d0                  unsized instruction
        rtm.k       d0
        rtm         d0@                 superfluous character

        end                             comment
