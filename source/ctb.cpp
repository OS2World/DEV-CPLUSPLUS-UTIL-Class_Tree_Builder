/*
** Module   :CTB.CPP
** Abstract :Main file of CLASS TREE BUILDER
**
** Copyright (C) Sergey I. Yevtushenko
** Log:
** Update : Sat  11-05-96
** Update : Tue  10-26-93
*/

#include <stdio.h>
#include <stdlib.h>
#include <dlc.h>
#include <dos.h>

char label[] = "Class Tree Builder  Version 2.91  Copyright (C) 1992-96 Sergey I. Yevtushenko.\n\n";
char sintax[] = "Usage: CTB [options] file[s]\n"
"\t-Ixxx  Include search path\n"
"\t-Sxxx  Set output base class\n"
"\t-oxxx  Output filename\n"
"\t-i     Ignore all includes\n"
"\t-w     Supress warnings messages\n"
"\t-s     Print pair (file,line) of class declaration\n"
"\t-c     Treat 'struct' and 'union' as 'class'\n"
"\t-a     Show all classes\n"
"\t-m     Show methods of classes\n"
"\t-n     Supress drawing tree, show only class names\n"
;

int error_count = 0;
TokenStream *IN;
char *INCLUDEpath;

char show_all          = 0;
char treat_struct      = 0;
char ignore_includes   = 0;
char skip_warnings     = 0;
char print_declarators = 0;
char print_methods     = 0;
char draw_tree         = 1;

char UnableToOpenFile[] = "Unable to open file";

int fatal(char *str)
{
    printf("FATAL::%s\nProgram aborted.\n", str);
    abort();
    return 0;
}

void *operator new(size_t size)
{
    void *ptr = malloc(size);
    if(!ptr)
        fatal("Not enough memory");
    return ptr;
}

int error2(char *str, char *str2 = "")
{
    printf("Error :%s %s.\n", str, str2);
    return 0;
}
int error(char *str, char *str2 = "")
{
	int ln = IN->line();
    printf("Error %s %d::%s %s.\n", IN->name(), ln, str, str2);
	error_count++;
	return 0;
}
int warning(char *str, char *str2 = "")
{
	if (skip_warnings)
		return 0;
	int ln = IN->line();
	printf("Warning %s %d::%s %s.\n", IN->name(), ln, str, str2);
	return 0;
}
inline token_rec & next()
{
	return IN->next();
}
inline token_rec & current()
{
	return IN->current();
}
inline is(char *str)
{
	return !strcmp(IN->current(), str);
}
inline is(char chr)
{
	return (IN->current().type == _other && IN->current().str[0] == chr) ? 1 : 0;
}
inline is_name(char *str)
{
    return (isalnum(*str) || (*str == '_'));
}
tree(char *nm, char *ext)
{
	IN = new TokenStream;
	IN->open(nm, ext);
	if (!IN->ok())
	{
		if (!*ext)
		{
            char path[_MAX_PATH];
			char *ptr = INCLUDEpath;
			char *sptr;
			do
			{
				sptr = ptr;
				ptr = strchr(ptr, ';');
				if (ptr)
					*ptr = 0;
				strcpy(path, sptr);
				if (ptr)
					*ptr = ';';
				if (path[strlen(path)] != '\\')
				{
					int i = strlen(path);
					path[i] = '\\';
					path[i + 1] = 0;
				}
				strcat(path, nm);
				IN->open(path, ext);
				if (IN->ok())
					break;
                if(ptr)
                    ptr++;
			}
			while (ptr);

			if (!IN->ok())
			{
				delete IN;
				return 1;
			}
		}
		else
		{
			delete IN;
			return 1;
		}
	}
	strlwr(IN->name());
    char short_name[_MAX_FNAME];

    if(strrchr(IN->name(),'\\'))
        strncpy(short_name, strrchr(IN->name(), '\\')+1, 14);
    else
        strncpy(short_name, IN->name() , 14);

    if (INCLUDES.look(short_name))
	{
		delete IN;
		return 0;
	}
	else
        INCLUDES.insert(short_name);
    printf("%s%s:\n", ((*ext) ? "":"  "), short_name);
    for (next(); current().type != _eof;)
	{
		if (is('#'))
		{
			next();
			if (is("include"))
			{
				next();
                char include_name[_MAX_PATH];
				if (is('<'))
				{
                    include_name[0]=0;
                    for(;;)
                    {
                        next();
                        if(!is('>'))
                            strcat(include_name, current());
                        else
                            break;
                    }
				}
                else
                    strcpy(include_name, current());
                if (ignore_includes)
                {
                    next();
                    continue;
                }
				TokenStream *SAVE = IN;
                int rc = tree(include_name, "");
				IN = SAVE;
				if (rc)
                    error2(UnableToOpenFile, include_name);
			}
			if (is("define"))
			{
                long ln = IN->line();
                next();
                DEFS.insert(current());
                while(ln == IN->line() && IN->current().type != _eof)
                    next();
                continue;
			}
		}
        if (is("class") || is("struct") || is("union"))
		{
            if(!treat_struct && is("struct") || is("union"))
            {
                next();
                continue;
            }
			next();
            while (DEFS.look(current()))
				next();
			while (is("far") || is("near"))
				next();
			if (is(';'))
				continue;
            if(current().type != _ident)
                continue;

			Name *tmp = NT.insert(current());

            strupr(short_name);
            strcpy(tmp->declaration_file,short_name);
            tmp->declaration_line = IN->line();

            next();
            if (is(';'))
				continue;
			if (is(':'))
			{
				for (;;)
				{
					next();
					while (is("public") || is("protected") || is("private") || is("virtual"))
						next();
					Name *tmp2 = NT.look(current());
					if (!tmp2)
						warning("Undefined class name", current());
					else
						tmp->base = new link(tmp2, tmp->base);
					next();
					if (!is(','))
						break;
				}
			}
//-----------------------------------
            while (!is('{') && current().type != _eof)
                next();
            char method_name[128];
            int flags = 0;
            int level = 0;
            method_name[0] = 0;
            char single = 0;
            do
            {
                if(is('{'))
                    level++;
                if(is('}'))
                    level--;
                if(current().type == _ident)
                {
                    //printf("-%s-\n", CPtr(current()));
                    if(is("static"))
                        flags |= 1;
                    if(is("virtual"))
                        flags |= 2;
                    if(single == '~' && level == 1)
                    {
                        flags |= 4;
                        method_name[0] = '~';
                        strcpy(&method_name[1], current());
                    }
                    else
                        strcpy(method_name, current());
                    next();
                    if(is('(') && level == 1)
                    {
                        if(!strcmp(tmp->str, method_name))
                            flags |= 8;
                        tmp->funcs = new method(method_name, tmp->funcs);
                        tmp->funcs->flags = flags;
                        flags = 0;
                    }
                    continue;
                }
                else
                    single = *CPtr(current());
                next();
            }
            while(level && current().type != _eof);
//-----------------------------------            
        }
		next();
	}
	delete IN;
	return 0;
}

// ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹
// €±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±€
// ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ

int main(int count, char **args)
{
	int cnt;
    char *outname = 0;
    char *obj = 0;

    if (1 == count)
	{
		printf(label);
		printf(sintax);
		return -2;
	}
	cnt = 1;
    while ('-' == args[cnt][0])
	{
		switch (args[cnt][1])
		{
			case 'S':
				obj = &args[cnt][2];
				break;
			case 'o':
				outname = &args[cnt][2];
				break;
			case 'I':
                INCLUDEpath = &args[cnt][2];
				break;
			case 'i':
				ignore_includes = 1;
                break;
            case 'a':
                show_all = 1;
                break;
            case 'c':
                treat_struct = 1;
				break;
			case 'w':
                skip_warnings = 1;
				break;
            case 'm':
                print_methods = 1;
                break;
            case 'n':
                draw_tree = 0;
                break;
            case 's':
                print_declarators = 1;
                break;
			default:
                error2("Bad option :", args[cnt]);
				return -4;
		}
		cnt++;
	}
	printf(label);
	if (count == cnt)
	{
        error2("No file names given");
		return -3;
	}
    char *env  = getenv("INCLUDE");
    if(INCLUDEpath)
    {
        char *temp = new char[strlen(INCLUDEpath) + ((env) ? strlen(env) : 0) + 5];
        strcpy(temp, INCLUDEpath);
        if(env)
        {
            strcat(temp, ";");
            strcat(temp, env);
        }
        INCLUDEpath = temp;
    }
    else
        INCLUDEpath = env;
	for (; cnt < count; cnt++)
	{
        if(strchr(args[cnt],'*') || strchr(args[cnt],'?'))
        {
            char wildarg[_MAX_PATH];
            char *tmp  = args[cnt];
            char *tmp2 = wildarg;
            char *name_pos = wildarg;
            int ext_found = 0;

            while(*tmp)
            {
                if(*tmp == '\\')
                {
                    ext_found = 0;
                    name_pos = tmp2+1;
                }
                if(*tmp == '.')
                    ext_found = 1;
                *tmp2++ = *tmp++;
            }
            if(!ext_found)
            {
                *tmp2++ = '.';
                *tmp2++ = 'h';
            }
            *tmp2 = 0;
            struct find_t one_file;
            for(int done = _dos_findfirst(wildarg, 0x21, &one_file); !done; done = _dos_findnext(&one_file))
            {
                strcpy(name_pos,one_file.name);
                if( tree(wildarg, ".h"))
                {
                    error2(UnableToOpenFile, wildarg);
                    error_count++;
                }
            }
        }
        else
        {
            if (tree(args[cnt], ".h"))
            {
                error2(UnableToOpenFile, args[cnt]);
                error_count++;
            }
        }
	}
	if (error_count)
		printf("\n*** Total %d error(s).\n", error_count);
	else
		NT.show_tree(obj, outname);

    return error_count;
}
