* Branch instructions' tests.
            bra     label1            8-bit branch sufficient
            bra     label2            OK
            bra     label3            32-bit branch
            bra     label4            8-bit branch sufficient
            bra     label5            OK
            bra     label6            32-bit branch
            bra.s   label1            OK
            bra.s   label2            16-bit branch
            bra.s   label3            32-bit branch
            bra.s   label4      	  OK
            bra.s   label5            16-bit branch
            bra.s   label6            32-bit branch
            bra.l   label1            8-bit branch
            bra.l   label2            16-bit branch
            bra.l   label3      	  OK
            bra.l   label4            8-bit branch
            bra.l   label5            16-bit branch
            bra.l   label6		  	  OK
label1      equ     $10
label2      equ     $1000
label3      equ     $10000000
label4      equ     -$10
label5      equ     -$1000
label6      equ     -$10000000
            bra     label1            OK - assembler determines offset
            bra     label2            OK - assembler determines offset
            bra     label3            OK - assembler determines offset
            bra     label4            OK - assembler determines offset
            bra     label5            OK - assembler determines offset
            bra     label6            OK - assembler determines offset
            bra.s   label1            OK
            bra.s   label2            16-bit branch
            bra.s   label3            32-bit branch
            bra.s   label4            OK
            bra.s   label5            16-bit branch
            bra.s   label6            32-bit branch
            bra.l   label1            8-bit branch
            bra.l   label2            16-bit branch
            bra.l   label3            OK
            bra.l   label4            8-bit branch
            bra.l   label5            16-bit branch
            bra.l   label6		      OK
            bra.s   label7		      short branch to immediately following 
*            						  instruction is not permitted
label7      bra.s   label7-$7e        OK, just
label8      bra.s   label8-$7f        too far
label9      bra.s   label9+$81        OK, just
label10     bra.s   label10+$82       too far
label11     bra     label11-$7ffe     OK, assembler finds correct size
label12     bra     label12-$7fff     OK, assembler finds correct size
label13     bra     label13+$8001     OK, assembler finds correct size
label14     bra     label14+$8002     OK, assembler finds correct size
            bra.s   $74.l             valid size-syntax is accepted and ignored
            bra.s   $74kV			  superfluous characters in operand
            end
