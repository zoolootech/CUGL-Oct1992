/*
        HEADER:         CUG000.10;
        TITLE:          NODE;
        DATE:           Mar 89;
        DESCRIPTION:    Node Pointer Type Declaration Header File;
        VERSION:        1.0;
        FILENAME:       NODELIST.H;
        SEE-ALSO:       TSP.C, NN.C, POPT.C, 2OPT.C, HYBRID.C, 3OPT.C, FN.C,
                        BOOLEAN.H, TSP.H,
                        BLDMTX.C, PRTMTX.C, TIME.C;
        AUTHORS:        Kevin E. Knauss;
*/

typedef struct virtex {
                        struct virtex *prior;
                        unsigned      position;
                        struct virtex *next;
} NODE;
