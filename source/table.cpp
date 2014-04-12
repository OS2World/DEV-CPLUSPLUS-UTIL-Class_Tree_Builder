/*
** Module   :TABLE.CPP
** Abstract :Name table implementation
**
** Copyright (C) Sergey I. Yevtushenko
** Log:
** Last update Sat  11-05-96
*/

#include <table.h>
#include <dlc.h>
#include <stdlib.h>
#include <stdio.h>
#define INFO_PLACE  50
extern char print_declarators;
extern char print_methods;
extern char show_all;
extern char draw_tree;

Table::Table(int sz)
{
	if (sz <= 0)
	{
		if (sz)
			sz = -sz;
		else
			sz = 29;
	}
	tbl = new Name *[sz];
	size = sz;
	for (int i = 0; i < sz; i++)
		tbl[i] = NULL;
}
Table::~Table()
{
	for (int i = 0; i < size; i++)
	{
		Name *nx;
		for (Name * n = tbl[i]; n; n = nx)
		{
			nx = n->next;
			delete n;
		}
	}
	delete tbl;
}

Name *Table::look(char *p, int ins)
{
	int ii = 0;
	char *pp = p;
	while (*pp)
		ii = ii << 1 ^ *pp++;
	if (ii < 0)
		ii = -ii;
	ii %= size;
	for (Name * n = tbl[ii]; n; n = n->next)
		if (!strcmp(p, n->str))
			return n;
	if (ins)
	{
		Name *nn = new Name(p, tbl[ii]);
		tbl[ii] = nn;
		return nn;
	}
	return NULL;
}
Name::Name(char *p, Name * n)
{
	next = n;
	str = new char[strlen(p) + 1];
	strcpy(str, p);
	base = NULL;
	childs = NULL;
	used = 0;
    funcs = 0;
    declaration_file[0] = 0;
    declaration_line = 0;
}

Name::~Name()
{
	delete str;
    for(method* tmp = 0; funcs; funcs = tmp)
    {
        tmp = funcs->next;
        delete funcs;
    }
}

link *lmerge (link *p, link *q)
{
    link *r;
    link head(0, 0);

    for ( r = &head; p && q; )
    {
        if ( strcmp(p->name->str, q->name->str) < 0 )
        {
            r = r->next = p;
            p = p->next;
        }
        else
        {
            r = r->next = q;
            q = q->next;
        }
    }
    r->next = (p ? p : q);
    return head.next;
}

link* Sort(link *p)
{
    link *q, *r;

    if(p)
    {
        q = p;
        for ( r = q->next; r && (r = r->next) != NULL; r = r->next )
            q = q->next;
        r = q->next;
        q->next = NULL;

        if ( r )
            p = lmerge(Sort(r), Sort(p));
    }
    return p;
}
Table::Prepare(Name * nm)
{
	if (nm)
	{
		if (nm->base)
        {
			nm->used = 1;
            nm->base = Sort(nm->base);
        }
		for (link * lnk = nm->base; lnk; lnk = lnk->next)
		{
			if (!lnk->name->childs)
			{
				lnk->name->childs = new link(nm);
			}
			else
			{
                for (link * lnk2 = lnk->name->childs; lnk2; lnk2 = lnk2->next)
				{
					if (lnk2->name == nm)
						break;
					else
						lnk2->name->used = 1;
				}
				if (!lnk2)
					lnk->name->childs = new link(nm, lnk->name->childs);
			}
		}
	}
	return 0;
}

static int level = 1;
static char *bar;
static char *outbuffer;

void Table::show_tree(Name * nm)
{
    if(nm->childs)
        nm->childs = Sort(nm->childs);
    for (link * lnk = nm->childs; lnk; lnk = lnk->next)
	{
		bar[level * 2] = (lnk->next) ? 'Ã' : 'À';
		bar[level * 2 + 1] = 0;
        int pos = sprintf(outbuffer, "%sÄ%s", bar, lnk->name->str);
        if (lnk->name->base && lnk->name->base->next)
        {
            pos += sprintf(&outbuffer[pos], "(");                      ///////////////////////////
            for (link * ptr = lnk->name->base; ptr; ptr = ptr->next)
            {
                if(ptr->name == nm)
                    continue;

                pos += sprintf(&outbuffer[pos], ptr->name->str);
                if (ptr->next && ptr->next->name != nm)
                    pos += sprintf(&outbuffer[pos], ",");
            }
            pos += sprintf(&outbuffer[pos], ")");
        }
        if(print_declarators)
        {
            while(pos < INFO_PLACE)
                outbuffer[pos++] = ' ';
            pos += sprintf(&outbuffer[pos], " -[%-12s (%4ld)]",lnk->name->declaration_file,lnk->name->declaration_line);
        }
        sprintf(&outbuffer[pos], "\r\n");

		if (OUT.ok())
            OUT.put(outbuffer);
		else
            printf(outbuffer);

		if (lnk->name->childs)
			bar[level * 2] = (lnk->next) ? '³' : ' ';
		bar[level * 2 + 1] = ' ';
		level++;
		show_tree(lnk->name);
		level--;
	}
}
void Table::show_tree(char *obj, char *file)
{
    int pos = 0;
    bar       = new char[1024];
    outbuffer = new char[1024];
	bar[0] = ' ';
	bar[1] = ' ';
	bar[2] = ' ';
	for (int i = 0; i < size; i++)
	{
		for (Name * nx = tbl[i]; nx; nx = nx->next)
			Prepare(nx);
	}
	if (file)
	{
		OUT.open(file, ".def", WR);
	}
	if (obj)
	{
		Name *o = look(obj);
		if (o)
		{
            pos = sprintf(outbuffer, "Class hierarchy from base class %s\r\n", o->str);
            if (OUT.ok())
                OUT.put(outbuffer);
			else
                printf(outbuffer);

            pos = sprintf(outbuffer, " %s", o->str);
            if(print_declarators)
            {
                while(pos < INFO_PLACE)
                    outbuffer[pos++] = ' ';
                pos += sprintf(&outbuffer[pos], " -[%-12s (%4ld)]",o->declaration_file,o->declaration_line);
            }
            sprintf(&outbuffer[pos], "\r\n");
            if (OUT.ok())
                OUT.put(outbuffer);
			else
                printf(outbuffer);
            show_tree(o);
			delete bar;
			return;
		}
		else
			printf("Error: Class '%s' not found.\n", obj);
	}

    sprintf(outbuffer, "\r\nClass hierarchy\r\n");

    if (OUT.ok())
        OUT.put(outbuffer);
    else
        printf(outbuffer);

    for (i = 0; i < size; i++)
	{
		for (Name * nx = tbl[i]; nx; nx = nx->next)
		{
            if (!nx->used)
			{
                if(show_all || nx->childs)
                {
                    pos = sprintf(outbuffer, " %s", nx->str);
                    if(print_declarators)
                    {
                        while(pos < INFO_PLACE)
                            outbuffer[pos++] = ' ';
                        pos += sprintf(&outbuffer[pos], " -[%-12s (%4ld)]",nx->declaration_file,nx->declaration_line);
                    }
                    sprintf(&outbuffer[pos], "\n\r");
                    if (OUT.ok())
                        OUT.put(outbuffer);
                    else
                        printf(outbuffer);

                    if(nx->funcs && print_methods)
                    {
                        if (OUT.ok())
                            OUT.put("\tMethods:\n\r");
                        else
                            printf("\tMethods:\n");
                        for(method *tmp=nx->funcs; tmp; tmp = tmp->next)
                        {
                            if (OUT.ok())
                            {
                                OUT.put("\t\t");
                                OUT.put(tmp->name);
                            }
                            else
                                printf("\t\t%s", tmp->name);

                            if(tmp->flags)
                            {
                                if(OUT.ok())
                                {
                                    OUT.put(" - ");
                                    if(tmp->flags & 1)
                                        OUT.put("static ");
                                    if(tmp->flags & 2)
                                        OUT.put("virtual ");
                                    if(tmp->flags & 4)
                                        OUT.put("destuctor ");
                                    if(tmp->flags & 8)
                                        OUT.put("constructor");
                                    OUT.put("\n\r");
                                }
                                else
                                    printf(" - %s%s%s%s\n",
                                    (tmp->flags & 1) ? "static " :"",
                                    (tmp->flags & 2) ? "virtual ":"",
                                    (tmp->flags & 4) ? "destuctor ":"",
                                    (tmp->flags & 8) ? "constructor":""
                                    );
                            }
                            else
                            {
                                if (OUT.ok())
                                    OUT.put("\n\r");
                                else
                                    printf("\n");
                            }
                        }
                        if (OUT.ok())
                            OUT.put("\n\r");
                        else
                            printf("\n");
                        if(draw_tree)
                            if (OUT.ok())
                                OUT.put(outbuffer);
                            else
                                printf(outbuffer);
                    }
                    if(draw_tree)
                        show_tree(nx);
                }
			}
		}
	}
	delete bar;
}

method::method(char *txt, method * nxt)
{
    name = new char[strlen(txt) + 1];
    strcpy(name, txt);
    next = nxt;
}

