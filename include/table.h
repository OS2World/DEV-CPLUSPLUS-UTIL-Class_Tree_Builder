/*
** Module   :TABLE.H
** Abstract :Name table imlementation
**
** Copyright (C) Sergey I. Yevtushenko
** Log:
** Update :Fri  10-05-96
** Update :Wed  06-16-93
** Update :Mon  27-04-92
*/

#ifndef __cplusplus
#error Types "name" and "table" supported only in C++
#else

#include <string.h>
#include <stdlib.h>

#ifndef __TABLE_H
#define __TABLE_H

class link;
class method;
class Name
{
    public:
        Name * next;
        char *str;
        char declaration_file[_MAX_FNAME];
        long declaration_line;
        link *base;
        link *childs;
        method *funcs;
        char used;

        Name(char *, Name *);
        ~Name();
};
class link
{
    public:
        Name * name;
        link *next;

        link(Name * nm, link * ln = 0) { name = nm;next = ln;}
    friend link* Sort(link *);
};

class method
{
    public:
        int flags;
        char *name;
        method* next;

        method(char *, method *);
        ~method() { delete name;}
};

class Table
{
        Name **tbl;
        int size;
    public:
        Table(int sz = 101);
        ~Table();

        Name *look(char *, int = 0);
        Name *insert(char *s) { return look(s, 1);}
        Prepare(Name *);
        void show();
        void show(Name *, int = 1);
        void show_tree(char* =0, char* =0);
        void show_tree(Name *);
};
#endif
#endif
