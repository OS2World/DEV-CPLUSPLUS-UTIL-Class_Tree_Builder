/*
** Module   :STREAM.CPP
** Abstract :Simple file i/o streams implementation
**
** Copyright (C) Sergey I. Yevtushenko
** Log:
** Update : Sat  11-05-96
** Update : Wed  07-01-92
*/

#include <stream.hpp>
#include <string.h>
#include <stdlib.h>
#include <io.h>
#include <fcntl.h>
#include <sys/stat.h>

#define BUFFER_SIZE 512
#define OPEN_RW     O_RDWR|O_BINARY
#define OPEN_CR     O_RDWR|O_BINARY|O_CREAT|O_TRUNC,S_IREAD|S_IWRITE

int File::open(char *name, char *ext, int a_mode)
{
	switch (mode)
	{
        case WR:
            write(hnd, buff, count);
		case RD:
            close(hnd);
			break;
	}
	if (!fname)
        fname = new char[_MAX_PATH];
	if (!fname)
		return -1;
	strcpy(fname, name);
	if (!strchr(name, '.'))
		strcat(fname, ext);
	mode = a_mode;
	if (!buff)
        buff = new char[BUFFER_SIZE];
	if (!buff)
		return -1;
    memset(buff, 0, BUFFER_SIZE);
	switch (a_mode)
	{
		case RD:
            hnd = ::open(fname, OPEN_RW);
			if (hnd >= 0)
			{
                count = ::read(hnd, buff, BUFFER_SIZE);
				pos = 0;
			}
			else
			{
				mode = BAD;
				return -2;
			}
			break;
		case WR:
            hnd = ::open(fname, OPEN_CR);
			if (hnd >= 0)
			{
				count = 0;
				pos = 0;
			}
			else
			{
				mode = BAD;
				return -3;
			}
			break;
		default:
			return -4;
	}
	return 0;
}
File::~File()
{
	switch (mode)
	{
		case WR:
            ::write(hnd, buff, count);
		case RD:
            ::close(hnd);
			break;
	}
	delete buff;
    buff = 0;
	delete fname;
    fname = 0;
}
int File::get()
{
	if (mode == BAD || mode == WR || !count)
		return -1;
	if (count == pos)
	{
        count = ::read(hnd, buff, BUFFER_SIZE);
		if (!count)
			return -1;
		pos = 0;
	}
	return buff[pos++];
}
int File::put(int ch)
{
	if (mode == BAD || mode == RD)
		return -1;
	buff[count++] = ch;
    if (count == BUFFER_SIZE)
	{
        ::write(hnd, buff, BUFFER_SIZE);
		count = 0;
	}
	return 0;
}
int File::put(char *str)
{
	int i;
	if (str)
	{
		while (*str)
		{
			i = put(*str++);
			if (i)
				return i;
		}
	}
	return 0;
}

