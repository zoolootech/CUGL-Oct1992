* TRAPcc instructions tests.

        bra.s       label
        trapt
        trapt.w     #5
        trapt.l     #$50000
        trapf
        trapf.w     #5
        trapf.l     #$50000
        traphi
        traphi.w    #5
        traphi.l    #$50000
        trapls
        trapls.w    #5
        trapls.l    #$50000
        trapcc
        trapcc.w    #5
        trapcc.l    #$50000
        traphs                      equal trapcc
label   traphs.w    #5
        traphs.l    #$50000
        trapcs
        trapcs.w    #5
        trapcs.l    #$50000
        traplo                      equal trapcs
        traplo.w    #5
        traplo.l    #$50000
        trapne
        trapne.w    #5
        trapne.l    #$50000
        trapeq
        trapeq.w    #5
        trapeq.l    #$50000
        trapvc
        trapvc.w    #5
        trapvc.l    #$50000
        trapvs
        trapvs.w    #5
        trapvs.l    #$50000
        trappl
        trappl.w    #5
        trappl.l    #$50000
        trapmi
        trapmi.w    #5
        trapmi.l    #$50000
        trapge
        trapge.w    #5
        trapge.l    #$50000
        traplt
        traplt.w    #5
        traplt.l    #$50000
        trapgt
        trapgt.w    #5
        trapgt.l    #$50000
        traple
        traple.w    #5
        traple.l    #$50000

        trapf.w     #32767      Highest valid immediate data
        trapf.w     #-32768     Lowest valid immediate data
        trapf.l     #2147483647
        trapf.l     #-2147483648
        trapf.l     #$ffffffff  Highest valid immediate data

* Erroneous instructions

        trapf       #5          superfluous operand - treated as a comment
        trapf.b                 byte size invalid
        trapf.k                 invalid size code			
        trapf.                  missing size code after dot
        trapf.w     #5a         superfluous character in operand field
        trap[f.l    #$50&       superfluous character in operand field

* Missing operand:
        trapf.w

        trapf.w                 Missing operand. Comment taken as operand
        trapf.l                 Missing operand. Comment taken as operand
        trapf.w     #32768      Immediate value exceeds word size
        trapf.w     #-32769     Immediate value exceeds word size
        trapf.l     #32767      Long size unnecessary
        trapf.l     #-32768     Long size unnecessary
        trapf.l     #$100000000 Immediate value exceeds long size

        end
