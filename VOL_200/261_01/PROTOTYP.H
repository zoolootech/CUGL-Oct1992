/* as68.c */ 
    int main(int ,char * *);
    int label_do(void);
    int dump_code(int ,char *,int );

/* aserr.c */ 
    int err_out(int );

/* asetc.c */ 
    int preparse(void);
    int nextc(char * *);
    char *nextfield(char * *);
    char *nextline(void);
    int savefield(char *,int ,char *);
    int list_dump(char *);
    int obj_out(int ,char *,int );
    int new_rcrd(char *,long ,int *);
    int flush_rcrd(char *,char ,char );
    int hex_byt(unsigned char *,unsigned char *,int );
    int get_source(char *);

/* asinit.c */ 
    int asinit(int ,char * *);
    int finis(void);
    int too_long(char *,int );
    char *cisat(char ,char *);

/* asmt.c */ 

/* bc.c */ 
    int match(struct _mtable *);
    int type_search(struct _mtable *,int ,int );
    int bbb(struct _oprnd *);
    int bbbx(int ,struct _oprnd *);
    int ccc(struct _oprnd *);
    int rsd(int ,struct _oprnd *);
    int mmkk(char ,struct _oprnd *);
    int efghj(char ,int ,struct _oprnd *);
    int _d16(int ,struct _oprnd *);
    int _d8_i(int ,struct _oprnd *);
    int xxxx(int ,struct _oprnd *);
    int yy(struct _oprnd *);
    int oyz(int ,struct _oprnd *);
    int nn(void);
    int nnoy(void);
    int nnyy(void);
    int nx(void);
    int nnxx(void);
    int nd(void);
    int nd2(void);
    int ndob(void);
    int ndkk(void);
    int ndoy(void);
    int ndyy(void);
    int ndyz(void);
    int ne(void);
    int nf(void);
    int nf2(void);
    int nfob(void);
    int nfoy(void);
    int nfyy(void);
    int nfyz(void);
    int nhmm(void);
    int nj(void);
    int njmm(void);
    int nr(void);
    int nr2(void);
    int nrxx(void);
    int nryy(void);
    int nsmm(void);
    int nv(void);
    int cf(void);
    int cd(void);
    int dnoy(void);
    int dnyy(void);
    int dnyz(void);
    int de(void);
    int dj(void);
    int dr(void);
    int dsxx(void);
    int dy(void);
    int gnoy(void);
    int gnyy(void);
    int gnyz(void);
    int ge(void);
    int gs(void);
    int rd(void);
    int rf(void);
    int sdxx(void);
    int sf(void);

/* bc1.c */ 
    int p1_mnem(struct _mtable *);
    int p1_codelen(struct _oprnd *);

/* bc2.c */ 
    int p2_mnem(struct _mtable *);

/* lib68.c */ 
    int word_copy(char *,int ,char *);
    int a1strcmp(char *,char *);
    int purge_uc(char *);
    int movmem(char *,char *,int );

/* pop.c */ 
    int op_clear(struct _oprnd *);
    int op_eval(struct _oprnd *);
    int reduce(int );
    long val_push(long );
    long val_pop(void);
    int opr_push(int );
    int opr_pop(void);
    int op_scan(struct _oprnd *);
    int reg_scan(struct _oprnd *);
    int reg_name(void);
    int term_char(char );
    int reg_range(void);
    long _dtol(void);
    long _htol(void);
    long _btol(void);
    long _actol(void);

/* psdo.c */ 
    int px_dc(char *,int );
    int px_ds(char *,int );
    int px_orgx(char *,int );
    int px_end(char *,int );
    int px_incl(char *,int );
    int eval(int ,int );
    char *token(char *,char *);
    struct _ptable *psdosearch(char *);

/* psdo1.c */ 
    int p1_equ(char *,int );
    int p1_set(char *,int );
    int p1_ifeq(char *,int );
    int p1_ifne(char *,int );
    int p1_endc(char *,int );
    int no_action(char *,int );

/* psdo2.c */ 
    int p2_equ(char *,int );
    int p2_set(char *,int );
    int p2_ifeq(char *,int );
    int p2_ifne(char *,int );
    int p2_endc(char *,int );
    int p2_xlst(char *,int );

/* symtbl.c */ 
    struct _mtable *mnemsearch(char *);
    int symsearch(char *);
    int symadd(char *,long ,char ,char );
    int symenter(char *,long ,char );
