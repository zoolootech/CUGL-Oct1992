range   reg     d1/d2/d3/a0/a1/a2/a3
range1  reg     d1-d3/a0-a3
range2  reg     a3-a0/d3-d1
        movem   d1/d2/d3/a0/a1/a2/a3,-(a0)
        movem   range,-(a0)
        movem   range1,-(a0)
        movem   range2,-(a0)
        end
