* CHK2 instruction tests
        bra.s   label

        chk2    (a3),a0
        chk2    (128,a5),a3
label   chk2    ([127,a3],d5.l*4,-128),d7
        end
