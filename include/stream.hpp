/*
** Module   :STREAM.HPP
** Abstract :Simple file i/o streams
**
** Copyright (C) Sergey I. Yevtushenko
** Log:
** Update : Sat  11-05-96
** Update : Thu  09-03-95
** Update : Wed  07-01-92
*/

#ifndef __STREAM_HPP
#define __STREAM_HPP

class File
{
    protected:
        char *buff;
        char *fname;
        int count;
        int pos;
        int hnd;
        int mode;
    public:
        File(){fname = 0; buff = 0; count = pos = mode = 0;hnd = -1;}
        File(char *name,int mode){open(name,"",mode);}
        File(char *name,char *ext,int mode){open(name,ext,mode);}
        ~File();
        int open(char *name,char *def,int mode);
        int get();
        int put(int);
        int put(char *);
        int ok(){return mode;}
        char *name(){return fname;}
};
enum open_mode
{
    BAD,RD,WR,EOFILE = -1
};

#endif

