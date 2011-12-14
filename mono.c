#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <setjmp.h>
#include "mono.h"

cell heap[HEAPSIZE];
token stok = {GO,OTHER};
jmp_buf buf;

void main(void){
    int addr;
    
    initcell();
    initsubr();
    int ret = setjmp(buf);
    
    /*
    addr = read();
    dump(0,20);
    printf("addr=%d\n",addr);
    print(addr);
    */
    
        
    if(ret == 0)
        while(1){
                printf("Mono> "); fflush(stdout); fflush(stdin);
            print(eval(read ()));
            printf("\n"); fflush(stdout);
        }
    else
        return;

}


void initcell(void){
        int addr,addr1;
    
    for(addr=0; addr < HEAPSIZE; addr++){
        heap[addr].tag = EMP;
        heap[addr].cdr = addr+1;
    }
    H = 0;
    
    //0番地はnil、1番地はＴとして環境レジスタを設定する。初期環境
    addr = freshcell(); //symbol nil
    SET_TAG(addr,SYM);
    SET_NAME(addr,"nil");
    addr1 = freshcell(); //symbol t
    SET_TAG(addr1,SYM);
    SET_NAME(addr1,"t");
        SET_CDR(addr1,addr);
    E = addr1;
}

int freshcell(void){
        int res;
    
    res = H;
    H = heap[H].cdr;
    return(res);
}



//deep-bindによる。シンボルが見つからなかったら登録。
//見つかったらそこに値をいれておく。
void bindsym(int symaddr, int valaddr){
        int addr,num;
    char *name;
    
    name = symname(symaddr);
    if((addr=findsym(name)) == NIL){
        addr = freshcell();
        SET_NAME(addr,name);
        SET_CDR(addr,E);
        E = addr;
    }
    switch(GET_TAG(valaddr)){
                case NUM: {     SET_TAG(addr,NUM);
                                num = GET_NUMBER(valaddr);
                    SET_NUMBER(addr,num);
                    break; }
        case SYM: {     SET_TAG(addr,SYM);
                                name = GET_NAME(valaddr);
                                SET_NAME(addr,name);
                                break; }
        case LIS: {     SET_TAG(addr,LIS);
                                SET_BIND(addr,valaddr);
                                break; }
    }
}
        
    
//環境はリストになっていて次のよう。
// env = (sym1 sym2 ...nil)
//数ならtagに値の型を入れて、それに対応した値をnum=数、
//bind=シンボルのアドレスあるいはリストのアドレスをいれておく。
// nilは必ず0番地に割り当てられるので0番地までを手繰ればいい。
int findsym(char *name){
        int addr;
    
    addr = E;
    while(addr != 0){
        if(HAS_NAME(addr,name))
                return(addr);
        else
                addr = GET_CDR(addr);
    }
    return(NIL);
}

//デバッグ用    
void cellprint(int addr){
        switch(GET_TAG(addr)){
        case EMP:       printf("EMP "); break;
        case NUM:       printf("NUM "); break;
        case SYM:       printf("SYM "); break;
        case LIS:       printf("LIS "); break;
        case FUN:       printf("FUN "); break;
    }
    printf("name=%s ", GET_NAME(addr));
    printf("car=%d ", GET_CAR(addr));
    printf("cdr=%d ", GET_CDR(addr));
    printf("num=%d ", GET_NUMBER(addr));
    printf("bind=&d ", GET_BIND(addr));
    printf("subr=%d\n", heap[addr].subr);
}   
//デバッグ用 
void dump(int start, int end){
        int i;
    
    for(i=start; i<= end; i++){
        printf("%d ", i);
        cellprint(i);
    }
    printf("E=%d   H=%d\n", E, H);
}

int car(addr){
        return(GET_CAR(addr));
}


int cdr(addr){
        return(GET_CDR(addr));
}

int cadr(addr){
        return(car(cdr(addr)));
}

int caddr(addr){
        return(car(cdr(cdr(addr))));
}


int cons(int car, int cdr){
        int addr;
    
    addr = freshcell();
        SET_TAG(addr,LIS);
    SET_CAR(addr,car);
    SET_CDR(addr,cdr);
    return(addr);
}
                                
int makenum(int num){
        int addr;
    
    addr = freshcell();
    SET_TAG(addr,NUM);
    SET_NUMBER(addr,num);
    return(addr);
}

int makesym(char *name){
        int addr;
    
    addr = freshcell();
    SET_TAG(addr,SYM);
    SET_NAME(addr,name);
    return(addr);
}


//空リストを作る。シンボルnilを空リストとも解釈している。
int makeempty(void){
        int addr;
    
    addr = freshcell();
    SET_TAG(addr,SYM);
    SET_NAME(addr,"nil");
    return(addr);
}

//-------read()--------

void gettoken(void){
        char c;
    int pos;
    
    if(stok.flag == BACK){
        stok.flag = GO;
        return;
    }
    
    if(stok.ch == ')'){
        stok.type = RPAREN;
        stok.ch = NUL;
        return;
    }
    
    if(stok.ch == '('){
        stok.type = LPAREN;
        stok.ch = NUL;
        return;
    }
    
    c = getchar();
    while(c == SPACE)
        c=getchar();
    
    switch(c){
        case '(':       stok.type = LPAREN; break;
        case ')':       stok.type = RPAREN; break;
        case '\'':      stok.type = QUOTE; break;
        default: {
                pos = 0; stok.buf[pos++] = c;
                while(((c=getchar()) != EOL) && (pos < BUFSIZE) && 
                        (c != SPACE) && (c != '(') && (c != ')'))
                stok.buf[pos++] = c;
                
            stok.buf[pos] = NUL;
            stok.ch = c;
            if(numbertoken(stok.buf)){
                stok.type = NUMBER;
                break;
            }
            if(symboltoken(stok.buf)){
                stok.type = SYMBOL;
                break;
            }
            stok.type = OTHER;  
        }
    }
}

int numbertoken(char buf[]){
        int i = 0;
    char c;
    
    while((c=buf[i]) != NUL)
        if(isdigit(c))
                i++;
        else 
                return(0);
    
    return(1);
}

int symboltoken(char buf[]){
        int i;
    char c;
    
    if(isdigit(buf[0]))
        return(0);
    
    i = 1;
    while((c=buf[i]) != NUL)
        if((isalpha(c)) || (isdigit(c)) || (issymch(c)))
                i++;
        else 
                return(0);
    
    return(1);
}

int issymch(char c){
        switch(c){
        case '!':
        case '?':
        case '+':
        case '-':
        case '*':
        case '/': return(1);
        defalut:  return(0);
    }
}  
        

int read(void){
    gettoken();
    switch(stok.type){
        case NUMBER:    return(makenum(atoi(stok.buf)));
        case SYMBOL:    return(makesym(stok.buf));
        case QUOTE:             return(cons(makesym("quote"), cons(read(),makeempty())));
        case LPAREN:    return(readlist());
    }
}

int readlist(void){
    int car,cdr;
    
    gettoken();      
    if(stok.type == RPAREN) 
        return(makeempty());
    else{
        stok.flag = BACK;
        car = read();
        cdr = readlist();
        return(cons(car,cdr));
    }
}


void print(int addr){    
        switch(GET_TAG(addr)){
        case NUM:       printf("%d", GET_NUMBER(addr)); break;
        case SYM:       printf("%s", GET_NAME(addr)); break;
        case LIS: {     printf("(");
                                printlist(addr); break;}
    }
}       
        

void printlist(int addr){
        if(IS_NIL(addr))
        printf(")");
    else {
        print(GET_CAR(addr));
        if(! (IS_NIL(GET_CDR(addr))))
                printf(" ");
        printlist(GET_CDR(addr));
    }
}

        
int eval(int addr){
        int res;
    
    if(atomp(addr)){
                if(IS_NUMBER(addr))
                return(addr);
        if(IS_SYMBOL(addr)){
                res = findsym(GET_NAME(addr));
            switch(GET_TAG(res)){
                case NUM:       return(makenum(GET_NUMBER(res)));
                case SYM:       return(GET_BIND(res));
                case LIS:       return(GET_BIND(res));
            }
        }
    }
    else{
    if(HAS_NAME(car(addr),"quote"))
        return(cadr(addr));
    if(subrp(car(addr)))
        return(apply(symname(car(addr)),evlis(cdr(addr))));
    if(fsubrp(car(addr)))
        return(apply(symname(car(addr)),cdr(addr)));
    if(lambdap(car(addr)))
        return(apply(symname(car(addr)),evlis(cdr(addr))));
    }
    return(NIL);
}

int apply(char *symname, int args){
        int symaddr,lamlis,body,res;
      
    symaddr = findsym(symname);
    if(symaddr == NIL)
        return(NIL);
    else {
        switch(GET_FTYPE(symaddr)){
                case SUBR:      return((heap[symaddr].subr)(args));
            case FSUBR: return((heap[symaddr].subr)(args));                     
            case LAMBDA: {      lamlis = car(heap[symaddr].bind);
                                        body = cdr(heap[symaddr].bind);
                            bind(lamlis,args);
                            while(!(IS_NIL(body))){
                                res = eval(car(body));
                                body = cdr(body);
                            }
                            unbind();
                            return(res); }      
        }
    }
}

void bind(int lambda, int arglist){
        int arg1,arg2;

        EP = E;
    while(!(IS_NIL(lambda))){
        arg1 = car(lambda);
        arg2 = car(arglist);
        bindsym(arg1,arg2);
        lambda = cdr(lambda);
        arglist = cdr(arglist);
    }
}

void unbind(void){
        E = EP;
}


int evlis(int addr){
        int car_addr,cdr_addr;
    
    if(IS_NIL(addr))
        return(addr);
        else{
        car_addr = eval(car(addr));
        cdr_addr = evlis(cdr(addr));
        return(cons(car_addr,cdr_addr));
    }
}       
        

int atomp(int addr){
    if((IS_NUMBER(addr)) || (IS_SYMBOL(addr)))
        return(1);
    else
        return(0);
}

int numberp(int addr){  
    if(IS_NUMBER(addr))
        return(1);
    else
        return(0);
}

int symbolp(int addr){  
    if(IS_SYMBOL(addr))
        return(1);
    else
        return(0);
}

int listp(int addr){    
    if(IS_LIST(addr))
        return(1);
    else
        return(0);
}

int nullp(int addr){
        if(IS_NIL(addr))
        return(1);
    else
        return(0);
}


int eqp(int addr1, int addr2){
        if((numberp(addr1)) && (numberp(addr2))
        && ((GET_NUMBER(addr1)) == (GET_NUMBER(addr2))))
        return(1);
    else if ((symbolp(addr1)) && (symbolp(addr2))
        && (SAME_NAME(addr1,addr2)))
        return(1);
    else
        return(0);
}

int symnamep(int addr, char *name){
        if(HAS_NAME(addr,name))
        return(1);
    else
        return(0);
}

char* symname(int addr){
        return(GET_NAME(addr));
}

int subrp(int addr){
        int symaddr;
    
        symaddr = findsym(symname(addr));
    if(symaddr == NIL)
        return(0);
    else {
        if((IS_FUNC(symaddr)) && (IS_SUBR(symaddr)))
                return(1);
        else
                return(0);
    }
}

int fsubrp(int addr){
        int symaddr;
    
        symaddr = findsym(symname(addr));
    if(symaddr == NIL)
        return(0);
    else {
        if((IS_FUNC(symaddr)) && (IS_FSUBR(symaddr)))
                return(1);
        else
                return(0);
    }
}

int lambdap(int addr){
        int symaddr;
    
        symaddr = findsym(symname(addr));
    if(symaddr == NIL)
        return(0);
    else {
        if((IS_FUNC(symaddr)) && (IS_LAMBDA(symaddr)))
                return(1);
        else
                return(0);
    }
}



//--------組込み関数
//subrを環境に登録する。
void defsubr(char *symname, int func){
        bindfunc(symname, SUBR, func);
}
//fsubr(引数を評価しない組込関数）の登録。
void deffsubr(char *symname, int func){
        bindfunc(symname, FSUBR, func);
}

void bindfunc(char *name, ftype ftype, int func){
        int addr;

        addr = freshcell();
        SET_NAME(addr,name);
        SET_TAG(addr,FUN);
    SET_FTYPE(addr,ftype);
    switch(ftype){
        case SUBR:
        case FSUBR:             SET_SUBR(addr,func);
        case LAMBDA:    SET_BIND(addr,func);
    }
    SET_CDR(addr,E);
    E = addr;
}

void initsubr(void){
        defsubr("+",(int)f_plus);
    defsubr("-",(int)f_minus);
    defsubr("*",(int)f_mult);
    defsubr("exit",(int)f_exit);
    defsubr("dump",(int)f_dump);
    defsubr("error",(int)f_error);
    defsubr("car",(int)f_car);
    defsubr("cdr",(int)f_cdr);
    defsubr("cons",(int)f_cons);
    defsubr("eq",(int)f_eq);
    defsubr("oblist",(int)f_oblist);
    deffsubr("setq",(int)f_setq);
    deffsubr("defun",(int)f_defun);
    
}

int f_plus(int arglist){
        int arg,res;
    
        res = 0;
    while(!(IS_NIL(arglist))){
        arg = GET_NUMBER(car(arglist));
        arglist = cdr(arglist);
        res = res + arg;
    }
    return(makenum(res));
}

int f_minus(int arglist){
        int arg,res;
    
    res = GET_NUMBER(car(arglist));
    while(!(IS_NIL(arglist))){
        arg = GET_NUMBER(car(arglist));
        arglist = cdr(arglist);
        res = res - arg;
    }
    return(makenum(res));
}

int f_mult(int arglist){
        int arg,res;
    
    res = GET_NUMBER(car(arglist));
    arglist = cdr(arglist);
    while(!(IS_NIL(arglist))){
        arg = GET_NUMBER(car(arglist));
        arglist = cdr(arglist);
        res = res * arg;
    }
    return(makenum(res));
}


int f_exit(int arglist){
        printf("-----Long live Lisp-----\n");
    longjmp(buf,1); 
}

int f_dump(int arglist){
        int arg1;
    
    arg1 = GET_NUMBER(car(arglist));
        dump(arg1,arg1+10);
    return(0);
}

//要デバグ。replが落ちてしまう。
int f_error(int arglist){
        int arg1;
    
    switch(GET_NUMBER(car(arglist))){
        case EVAL_ERR:  printf("eval error\n"); break;
        case ARG_ERR:   printf("arg error\n"); break;
    }
    longjmp(buf,0);
}

int f_car(int arglist){
        int arg1;
    
    arg1 = car(arglist);
        return(car(arg1));
}

int f_cdr(int arglist){
        int arg1;
    
    arg1 = car(arglist);
        return(cdr(arg1));
}

int f_cons(int arglist){
        int arg1,arg2;
    
    arg1 = car(arglist);
    arg2 = cadr(arglist);
        return(cons(arg1,arg2));
}

//要デバグ
int f_eq(int arglist){
        int arg1,arg2;
    
    return(eqp(arg1,arg2));
}

int f_oblist(int arglist){
        int addr,addr1,res;
    
    res = makeempty();
    addr = E;
    while(addr != 0){
        addr1 = makesym(symname(addr));
        res = cons(addr1,res);
        addr = cdr(addr);
    }
        return(res);
}

        

//FSUBR
int f_setq(int arglist){
        int arg1,arg2;
        
    arg1 = car(arglist);
    arg2 = eval(cadr(arglist));
    bindsym(arg1,arg2);
    return(T);   
}

int f_defun(int arglist){
        int arg1,arg2;
    
    arg1 = car(arglist);
    arg2 = cdr(arglist);
    bindfunc(symname(arg1),LAMBDA,arg2);
    return(T);
}


//-----------------------------
