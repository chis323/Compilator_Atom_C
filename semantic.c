#include <stdio.h>
#include "lexical_analyzer.h"

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

    s->args = NULL;
}

Symbol *addSymbol(Symbol *symbols, int* count, const char *name, int cls)
{
    if (*count >= MAX_SYM)
        printf("symbol table full");

    Symbol *s = &symbols[(*count)++];
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
        s = addSymbol(crtStruct->members, &symbolCount, name, CLS_VAR);
    }
    else if (crtFunc)
    {
        s = findSymbol(symbols, name);
        if (s && s->depth == crtDepth)
            printf( "symbol redefinition: %s", name);
        s = addSymbol(symbols, &symbolCount, name, CLS_VAR);
        s->mem = MEM_LOCAL;
    }
    else
    {
        if (findSymbol(symbols, name))
            printf( "symbol redefinition: %s", name);
        s = addSymbol(symbols, &symbolCount, name, CLS_VAR);
        s->mem = MEM_GLOBAL;
    }
    s->type = *t;
}

Type* getType(TokenType type)
{
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
        case T_VOID: {
            t->typeBase = TB_VOID;
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

const char* clsToString(int cls) 
{
    switch(cls) {
        case CLS_VAR: return "VAR";
        case CLS_FUNC: return "FUNC";
        case CLS_EXTFUNC: return "EXTFUNC";
        case CLS_STRUCT: return "STRUCT";
        default: return "UNKNOWN";
    }
}

const char* memToString(int mem) 
{
    switch(mem) {
        case MEM_GLOBAL: return "GLOBAL";
        case MEM_ARG: return "ARG";
        case MEM_LOCAL: return "LOCAL";
        default: return "UNKNOWN";
    }
}

const char* typeBaseToString(int typeBase) 
{
    switch(typeBase) {
        case TB_INT: return "INT";
        case TB_DOUBLE: return "DOUBLE";
        case TB_CHAR: return "CHAR";
        case TB_STRUCT: return "STRUCT";
        case TB_VOID: return "VOID";
        default: return "UNKNOWN";
    }
}

void printType(Type t) 
{
    printf("%s", typeBaseToString(t.typeBase));
    if (t.typeBase == TB_STRUCT && t.s != NULL) {
        printf("(%s)", t.s->name);
    }
    if (t.nElements >= 0) {
        printf("[%d]", t.nElements);
    } else if (t.nElements == 0) {
        printf("[]");
    }
}

void printSymbol(const Symbol* sym) 
{
    printf("Name: %-10s | Class: %-7s | Mem: %-6s | Type: ",
           sym->name, clsToString(sym->cls), memToString(sym->mem));
    printType(sym->type);
    printf(" | Depth: %d\n", sym->depth);

    // Print arguments if it's a function
    if (sym->cls == CLS_FUNC && sym->args != NULL) {
        printf("  Function Arguments:\n");
        for (int i = 0; sym->args[i].name != NULL; i++) {  // Assuming NULL-terminated
            printf("    Arg %d: ", i);
            printSymbol(&sym->args[i]);
        }
    }

    // Print members if it's a struct
    if (sym->cls == CLS_STRUCT && sym->members != NULL) {
        printf("  Struct Members:\n");
        for (int i = 0; sym->members[i].name != NULL; i++) {  // Assuming NULL-terminated
            printf("    Member %d: ", i);
            printSymbol(&sym->members[i]);
        }
    }
}

void printSymbolTable() 
{
    printf("\n=== SYMBOL TABLE (%d entries) ===\n", symbolCount);
    for (int i = 0; i < symbolCount; i++) {
        printf("[%d] ", i);
        printSymbol(&symbols[i]);
        printf("\n");
    }
    printf("=== END OF SYMBOL TABLE ===\n\n");
}