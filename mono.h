#define HEAPSIZE 1024
#define SYMSIZE 256
#define BUFSIZE 256
#define NIL     0
#define T 1

typedef enum tag {EMP,NUM,SYM,LIS,FUN} tag;
typedef enum flag {FRE,USE} flag;
typedef enum ftype {SUBR,FSUBR,LAMBDA} ftype;

struct cell {
        tag tag;
    flag flag;
    ftype ftype;
        char *name;
    int num;
    int bind;
        int ( *subr) ();
    int car;
    int cdr;
};

typedef struct cell cell;

typedef enum toktype {LPAREN,RPAREN,QUOTE,NUMBER,SYMBOL,OTHER} toktype;
typedef enum backtrack {GO,BACK} backtrack;

struct token {
        char ch;
    backtrack flag;
        toktype type;
    char buf[BUFSIZE];
};

typedef struct token token;

#define GET_CAR(addr)           heap[addr].car
#define GET_CDR(addr)           heap[addr].cdr
#define GET_NUMBER(addr)        heap[addr].num
#define GET_NAME(addr)          heap[addr].name
#define GET_TAG(addr)           heap[addr].tag
#define GET_FTYPE(addr)         heap[addr].ftype
#define GET_BIND(addr)          heap[addr].bind
#define SET_TAG(addr,val)       heap[addr].tag = val
#define SET_CAR(addr,val)       heap[addr].car = val
#define SET_CDR(addr,val)       heap[addr].cdr = val
#define SET_NUMBER(addr,val)    heap[addr].num = val
#define SET_BIND(addr,val)      heap[addr].bind = val
#define SET_NAME(addr,x)        heap[addr].name = (char *)malloc(SYMSIZE); strcpy(heap[addr].name,x);
#define SET_SUBR(addr,val)      heap[addr].subr = (int (*)())val
#define SET_FTYPE(addr,val)     heap[addr].ftype = val
#define IS_SYMBOL(addr)         heap[addr].tag == SYM
#define IS_NUMBER(addr)         heap[addr].tag == NUM
#define IS_LIST(addr)           heap[addr].tag == LIS
#define IS_FUNC(addr)           heap[addr].tag == FUN
#define IS_NIL(addr)            heap[addr].tag == SYM && HAS_NAME(addr,"nil")
#define IS_SUBR(addr)           heap[addr].ftype == SUBR
#define IS_FSUBR(addr)          heap[addr].ftype == FSUBR
#define IS_LAMBDA(addr)         heap[addr].ftype == LAMBDA
#define HAS_NAME(addr,x)        strcmp(heap[addr].name,x) == 0
#define SAME_NAME(addr1,addr2) strcmp(heap[addr1].name, heap[addr2].name) == 0

//------レジスタ----
int E; //環境レジスタ
int H; //ヒープレジスタ
int EP; //直前環境の保存用レジスタ


//-------read--------
#define EOL             '\n'
#define TAB             '\t'
#define SPACE   ' '
#define ESCAPE  033
#define NUL             '\0'

//-------error code---
#define EVAL_ERR        1
#define ARG_ERR         2


void initcell(void);
int freshcell(void);
void bindsym(int symaddr, int valaddr);
int findsym(char *name);
void cellprint(int addr);
void dump(int start, int end);

int car(int addr);
int cdr(int addr);
int cons(int car, int cdr);
int cadr(int addr);
int caddr(int addr);
int makenum(int num);
int makesym(char *name);
int makeempty(void);
void gettoken(void);
int numbertoken(char buf[]);
int symboltoken(char buf[]);
int issymch(char c);
int read(void);
int readlist(void);
void print(int addr);
void printlist(int addr);
int eval(int addr);
void bind(int lambda, int arglist);
void unbind(void);
int atomp(int addr);
int numberp(int addr);
int symbolp(int addr);
int listp(int addr);
int nullp(int addr);
int symnamep(int addr, char *name);
int evlisp(int addr);
int apply(char *symname, int arg);
char* symname(int addr);
int subrp(int addr);
int fsubrp(int addr);
void initsubr(void);
void bindfunc(char *name, ftype ftype, int func);

//---subr-------
int f_plus(int addr);
int f_minus(int addr);
int     f_mult(int addr);
int f_exit(int addr);
int f_dump(int addr);
int f_error(int addr);
int f_car(int addr);
int f_cdr(int addr);
int f_cons(int addr);
int f_eq(int addr);
int f_setq(int addr);
int f_oblist(int addr);
int f_defun(int addr);
