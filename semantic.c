#include <stdio.h>
#include "lexical_analyzer.h"
#define MAX_SYM 1000

Symbol symbols[MAX_SYM];
int symbolCount = 0;
int crtDepth = 0;
Symbol* crtFunc = NULL;
Symbol* crtStruct = NULL;

void initSymbol(Symbol *s)
{
    s->name = NULL;
    s->cls = 0;
    s->mem = 0;

    s->type.typeBase = 0;
    s->type.s = NULL;
    s->type.nElements = -1;

    s->depth = 0;

    // Clear args/members union
    for (int i = 0; i < 20; i++)
    {
        s->args[i].name = NULL;
        s->args[i].cls = 0;
        s->args[i].mem = 0;
        s->args[i].type.typeBase = 0;
        s->args[i].type.s = NULL;
        s->args[i].type.nElements = -1;
        s->args[i].depth = 0;
    }
}

Symbol *addSymbol(Symbol *symbols,const char *name, int cls)
{
    if (symbolCount >= MAX_SYM)
        printf("symbol table full");

    Symbol *s = &symbols[symbolCount++];
    if (s->name != NULL)
        initSymbol(s);

    s->name = name;
    s->cls = cls;
    return s;
}

Symbol *findSymbol(Symbol *symbols, const char *name)
{
    // Search in reverse order to find the most recent declaration (like in nested scopes)
    for (int i = symbolCount - 1; i >= 0; i--)
    {
        if (strcmp(symbols[i].name, name) == 0 && symbols[i].depth <= crtDepth)
        {
            return &symbols[i];
        }
    }
    return NULL; // not found
}

int findSymbolIndex(const char *name)
{
    for (int i = symbolCount - 1; i >= 0; i--)
    {
        if (strcmp(symbols[i].name, name) == 0 && symbols[i].depth <= crtDepth)
        {
            return i;
        }
    }
    return -1; // not found
}

void deleteSymbolsAfter(int index)
{
    if (index < 0 || index > symbolCount)
        printf("invalid index to deleteSymbolsAfter");

    symbolCount = index;
}

void addVar(char name[], Type *t)
{
    Symbol *s;
    if (crtStruct)
    {
        if (findSymbol(crtStruct->members, name))
            printf("symbol redefinition: %s", name);
        s = addSymbol(crtStruct->members, name, CLS_VAR);
    }
    else if (crtFunc)
    {
        s = findSymbol(symbols, name);
        if (s && s->depth == crtDepth)
            printf( "symbol redefinition: %s", name);
        s = addSymbol(symbols, name, CLS_VAR);
        s->mem = MEM_LOCAL;
    }
    else
    {
        if (findSymbol(symbols, name))
            printf( "symbol redefinition: %s", name);
        s = addSymbol(symbols, name, CLS_VAR);
        s->mem = MEM_GLOBAL;
    }
    s->type = *t;
}

Type* getType(TokenType type){
    Type* t = malloc(sizeof(Type));
    switch(type){
        case T_INT: {
           t->typeBase = TB_INT;
           break;
        }
       case T_DOUBLE: {
           t->typeBase = TB_DOUBLE;
           break;
        }
        case T_CHAR: {
           t->typeBase = TB_CHAR;
           break;
        }
       default: {
            free(t); 
            return NULL;
       }
    }
    t->s = NULL;
    t->nElements = -1;
    return t;
}
