/*
** Module   :TSTREAM.CPP
** Abstract :Token stream implementation
**
** Copyright (C) Sergey I. Yevtushenko
** Log:
** Last update Sat  11-05-96
*/

#include <dlc.h>

File OUT;
Table NT;
Table DEFS;
Table INCLUDES;

TokenStream::TokenStream():File()
{
	lastchar = ' ';
    rec.str  = 0;
    token    = 0;
	line_num = 1;
}
TokenStream::TokenStream(char *name, char *def):File(name, def, RD)
{
    lastchar = ok() ? get() : EOFILE;
	token = new char[1024];
	memset(token, 0, 1024);
	rec.str = token;
	line_num = 1;
}
int TokenStream::open(char *name, char *def)
{
	if (!token)
	{
		token = new char[1024];
		if (!token)
			return -1;
		rec.str = token;
		memset(token, 0, 1024);
	}
	line_num = 1;
    return File::open(name, def, RD);
}

/*
** id        ::= aplha[a_d_delim]
** a_d_delim ::= alpha|digit|delim
** alpha     ::= isaplha(ch)
** digit     ::= isdigit(ch)
** delim     ::= '$'|'@'|'_'|'.' ;
** string    ::= 'any_char' | "any_char"
** comment   ::= //any_char
** any_char  ::= ch > 0x20 & ch != "'" | '"'
*/

int TokenStream::get_char_token()
{
	char *tok = token;
	int trm;

	if (!tok)
        lastchar = EOFILE;
    if (lastchar == EOFILE)
		return _eof;
	while (isspace(lastchar) || lastchar == '\r' || lastchar == '\n')
	{
		if (lastchar == '\r')
			line_num++;
		lastchar = get();
        if (lastchar == EOFILE)
			return _eof;
	}
	if (isalpha(lastchar) || lastchar == '_')
	{
		do
		{
			*tok++ = lastchar;
			lastchar = get();
		}
        while (isalpha(lastchar) || isdigit(lastchar) || lastchar == '_' || lastchar == '.' && lastchar != EOFILE);
		*tok = 0;
		return _ident;
	}
	if (isdigit(lastchar))
	{
		trm = 0;
		do
		{
			*tok++ = lastchar;
			if (lastchar == '.')
				if (trm)
					break;
				else
					trm = 1;
			lastchar = get();
		}
        while (isdigit(lastchar) || lastchar == '.' && lastchar != EOFILE);
		*tok = 0;
		return (trm) ? _float : _digit;
	}
	if (lastchar == '/')
	{
		lastchar = get();
		if (lastchar == '/')
		{
			while (lastchar != '\r' && lastchar > 0)
				lastchar = get();
			return get_char_token();
		}
		if (lastchar == '*')
		{
			lastchar = get();
			do
			{
				trm = lastchar;
				lastchar = get();
				if (lastchar == '\r')
					line_num++;
			}
            while ((lastchar != '/' || trm != '*') && lastchar != EOFILE);
            if (lastchar == EOFILE)
				return _eof;
			lastchar = get();
			return get_char_token();
		}
		*tok++ = '/';
		*tok = 0;
		return _other;
	}
	if (lastchar == '"' || lastchar == '\'')
	{
		trm = lastchar;
		do
		{
			lastchar = get();
			if (lastchar == '\\')
			{
				lastchar = get();
				if (lastchar == '\r' || lastchar == '\n')
					continue;
                else
                    *tok++ = '\\';
			}
			if (lastchar == trm || lastchar < 0)
				break;
			*tok++ = lastchar;
		}
		while (lastchar != '\r');
		*tok = 0;
        if (lastchar == EOFILE)
			return _eof;
		if (lastchar == '\r')
			return _badconst;
		lastchar = get();
		return _const;
	}
    if (lastchar == EOFILE)
		return _eof;
	*tok++ = lastchar;
	lastchar = get();
	*tok = 0;
	return _other;
}
