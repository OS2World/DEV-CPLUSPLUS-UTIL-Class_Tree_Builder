/*
** Module   :TSTREAM.H
** Abstract :Token stream
**
** Copyright (C) Sergey I. Yevtushenko
** Log:
** Update : Sat  11-05-96
*/


#ifndef  __TSTREAM_H
#define  __TSTREAM_H

typedef char * CPtr;

struct token_rec
{
    char *str;
    int  type;
    operator CPtr() { return str;}
};
enum token_type
{
    _eof = -1,
    _ident=1,
    _digit,
    _float,
    _const,
    _other,
    _badconst
};

class TokenStream:public File
{
        char *token;
        int get_char_token();
        int lastchar;
        int line_num;
        token_rec rec;
    public:
        TokenStream();
        TokenStream(char *name,char *def="");
        ~TokenStream(){if(token) delete token;}
        token_rec& next(){rec.type = get_char_token();return rec;}
        token_rec& current(){ return rec;}
        int line(){return line_num;}
        int open(char *name,char *def="");
};

#endif
