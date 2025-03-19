#include <stdio.h>
#include <stdarg.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#define TOKEN_LENGTH 100

void err(const char *fmt, ...);

#define SAFEALLOC(var, Type)                          \
    if ((var = (Type *)malloc(sizeof(Type))) == NULL) \
        err("not enough memory");

/*typedef enum
{
    IDENTIFIER,
    KEYWORDS,
    CONSTANTS,
    DELIMITERS,
    OPERATORS
} State;*/



typedef enum
{
    T_ID,
    T_BREAK,
    T_CHAR,
    T_DOUBLE,
    T_ELSE,
    T_FOR,
    T_IF,
    T_INT,
    T_RETURN,
    T_STRUCT,
    T_VOID,
    T_WHILE,
    T_CONSTINT,
    T_CONSTREAL,
    T_CONSTCHAR,
    T_CONSTSTRING,
    T_COMMA,
    T_SEMICOLON,
    T_LPAR,
    T_RPAR,
    T_LBRACKET,
    T_RBRACKET,
    T_LACC,
    T_RACC,
    T_ADD,
    T_SUB,
    T_MULT,
    T_DIV,
    T_DOT,
    T_AND,
    T_OR,
    T_NOT,
    T_ASSIGN,
    T_EQUAL,
    T_NOTEQUAL,
    T_LESS,
    T_LESSEQUAL,
    T_GREATER,
    T_GREATEREQUAL,
    T_SPACE,
    T_COMMENT,
    T_LINECOMMENT,
    T_END
} TokenType;


char* convert(TokenType token)
{
    char* s = malloc(20);
    if (!s) err("Not enough memory!");
    memset(s, 0, 20);
    switch (token)
    {
        case T_ID: strcpy(s, "T_ID"); break;
        case T_BREAK: strcpy(s, "T_BREAK"); break;
        case T_CHAR: strcpy(s, "T_CHAR"); break;
        case T_DOUBLE: strcpy(s, "T_DOUBLE"); break;
        case T_ELSE: strcpy(s, "T_ELSE"); break;
        case T_FOR: strcpy(s, "T_FOR"); break;
        case T_IF: strcpy(s, "T_IF"); break;
        case T_INT: strcpy(s, "T_INT"); break;
        case T_RETURN: strcpy(s, "T_RETURN"); break;
        case T_STRUCT: strcpy(s, "T_STRUCT"); break;
        case T_VOID: strcpy(s, "T_VOID"); break;
        case T_WHILE: strcpy(s, "T_WHILE"); break;
        case T_CONSTINT: strcpy(s, "T_CONSTINT"); break;
        case T_CONSTREAL: strcpy(s, "T_CONSTREAL"); break;
        case T_CONSTCHAR: strcpy(s, "T_CONSTCHAR"); break;
        case T_CONSTSTRING: strcpy(s, "T_CONSTSTRING"); break;
        case T_COMMA: strcpy(s, "T_COMMA"); break;
        case T_SEMICOLON: strcpy(s, "T_SEMICOLON"); break;
        case T_LPAR: strcpy(s, "T_LPAR"); break;
        case T_RPAR: strcpy(s, "T_RPAR"); break;
        case T_LBRACKET: strcpy(s, "T_LBRACKET"); break;
        case T_RBRACKET: strcpy(s, "T_RBRACKET"); break;
        case T_LACC: strcpy(s, "T_LACC"); break;
        case T_RACC: strcpy(s, "T_RACC"); break;
        case T_ADD: strcpy(s, "T_ADD"); break;
        case T_SUB: strcpy(s, "T_SUB"); break;
        case T_MULT: strcpy(s, "T_MULT"); break;
        case T_DIV: strcpy(s, "T_DIV"); break;
        case T_DOT: strcpy(s, "T_DOT"); break;
        case T_AND: strcpy(s, "T_AND"); break;
        case T_OR: strcpy(s, "T_OR"); break;
        case T_NOT: strcpy(s, "T_NOT"); break;
        case T_ASSIGN: strcpy(s, "T_ASSIGN"); break;
        case T_EQUAL: strcpy(s, "T_EQUAL"); break;
        case T_NOTEQUAL: strcpy(s, "T_NOTEQUAL"); break;
        case T_LESS: strcpy(s, "T_LESS"); break;
        case T_LESSEQUAL: strcpy(s, "T_LESSEQUAL"); break;
        case T_GREATER: strcpy(s, "T_GREATER"); break;
        case T_GREATEREQUAL: strcpy(s, "T_GREATEREQUAL"); break;
        case T_SPACE: strcpy(s, "T_SPACE"); break;
        case T_COMMENT: strcpy(s, "T_COMMENT"); break;
        case T_LINECOMMENT: strcpy(s, "T_LINECOMMENT"); break;
        case T_END: strcpy(s, "T_END"); break;
        default: strcpy(s, "error"); break;
    }
    return s;
}


typedef struct _Token
{
    int code;
    union
    {
        char *text;
        long int i;
        double r;
    } datavalue;
    int line;
    struct _Token *next;
} Token;

Token *tokens = NULL;
Token *lastToken = NULL;
int line = 1;

const char *tokenTypeNames[] = {
    "IDENTIFIER", "KEYWORDS", "CONSTANTS", "DELIMITERS", "OPERATORS", "COMMENT", "START"};

Token *addTk(int code)
{
    Token *tk;
    SAFEALLOC(tk, Token);
    tk->code = code;
    tk->line = line;
    tk->next = NULL;
    if (lastToken)
    {
        lastToken->next = tk;
    }
    else
    {
        tokens = tk;
    }
    lastToken = tk;
    return tk;
}

void freeTokens()
{
    Token *current = tokens;
    while (current)
    {
        Token *next = current->next;
        if (current->code == T_ID || current->code == T_CONSTSTRING)
        {
            free(current->datavalue.text);
        }
        free(current);
        current = next;
    }
    tokens = lastToken = NULL;
}


void err(const char *fmt, ...)
{
    va_list va;
    va_start(va, fmt);
    fprintf(stderr, "error: ");
    vfprintf(stderr, fmt, va);
    fputc('\n', stderr);
    va_end(va);
    exit(-1);
}

char *createString(const char *start, const char *end)
{
    int length = end - start;
    char *newStr = (char *)malloc(length + 1);
    if (!newStr)
        err("Not enough memory!");
    strncpy(newStr, start, length);
    newStr[length] = '\0';
    return newStr;
}

void print_tokens()
{
    Token *current = tokens;
    while (current)
    {
        printf("Token Type: %s, Value: %s\n", convert(current->code), current->datavalue.text ? current->datavalue.text : "");
        current = current->next;
    }
}

void tkerr(const Token *tk,const char *fmt,...)
{
va_list va;
va_start(va,fmt);
fprintf(stderr,"error in line %d: ",tk->line);
vfprintf(stderr,fmt,va);
fputc('\n',stderr);
va_end(va);
exit(-1);
}

const char *pCrtCh;

Token* getNextToken()
{
    int state = 0, nCh;
    char ch;
    const char *pStartCh;
    Token *tk;
    while (1)
    {
        ch = *pCrtCh;
        switch (state)
        {
        case 0: // transitions test for state 0
            if (isalpha(ch) || ch == '_')
            {
                pStartCh = pCrtCh; // memorizes the beginning of the ID
                pCrtCh++;          // consume the character
                state = 1;         // set the new state
            }
            else if (ch == '=')
            {
                pCrtCh++;
                state = 3;
            }
            else if (ch == ' ' || ch == '\r' || ch == '\t')
            {
                pCrtCh++; // consume the character and remains in state 0
            }
            else if (ch == '\n')
            { // handled separately in order to update the current line
                line++;
                pCrtCh++;
            }
            else if (ch == 0)
            { // the end of the input string
                addTk(T_END);
                return T_END;
            }
            else if(isdigit(ch))
            {
                if(isdigit(ch) && ch!=0)
                {
                    pCrtCh++;
                    state=6;
                }
                else if(ch == 0)
                {
                    pCrtCh++;
                    state=7;
                }
            }
            else
                tkerr(addTk(T_END), "invalid character");

            break;
            
        case 1:
            if (isalnum(ch) || ch == '_')
                pCrtCh++;
            else
                state = 2;
            break;
        case 2:
            nCh = pCrtCh - pStartCh; // the id length
            // keywords tests
            if (nCh == 5 && !memcmp(pStartCh, "break", 5))
                tk = addTk(T_BREAK);
            else if (nCh == 4 && !memcmp(pStartCh, "char", 4))
                tk = addTk(T_CHAR);
            else if (nCh == 6 && !memcmp(pStartCh, "double", 6))
                tk = addTk(T_DOUBLE);
            else if (nCh == 4 && !memcmp(pStartCh, "else", 4))
                tk = addTk(T_ELSE);
            else if (nCh == 3 && !memcmp(pStartCh, "for", 3))
                tk = addTk(T_FOR);
            else if (nCh == 2 && !memcmp(pStartCh, "if", 2))
                tk = addTk(T_IF);
            else if (nCh == 3 && !memcmp(pStartCh, "int", 3))
                tk = addTk(T_INT); 
            else if (nCh == 6 && !memcmp(pStartCh, "return", 6))
                tk = addTk(T_RETURN);
            else if (nCh == 6 && !memcmp(pStartCh, "struct", 6))
                tk = addTk(T_STRUCT);
            else if (nCh == 4 && !memcmp(pStartCh, "void", 4))
                tk = addTk(T_VOID);
            else if (nCh == 5 && !memcmp(pStartCh, "while", 5))
                tk = addTk(T_WHILE); // keywords done
            else
            { // if no keyword, then it is an ID
                tk = addTk(T_ID);
                tk->datavalue.text = createString(pStartCh, pCrtCh);
            }
            return tk;  // identifiers done
        case 3:
            if (ch == '=')
            {
                pCrtCh++;
                state = 4;
            }
            else
                state = 5;
            break;
        case 4:
            addTk(T_EQUAL);
            return T_EQUAL;
        case 5:
            addTk(T_ASSIGN);
            return T_ASSIGN;
        case 6:
            if(isdigit(ch)) pCrtCh++;
            else
                state = 8;
        case 7:
            if(isdigit(ch) && ch<=7) pCrtCh++;
            else if(ch=='x')
            {
                pCrtCh++;
                state=9;
            }
            else
                state=8;
        case 8:
            addTk(T_CONSTINT);
            return T_CONSTINT;
        case 9:
            if ((ch >= '0' && ch <= '9') || (ch >= 'a' && ch <= 'f') || (ch >= 'A' && ch <= 'F'))
            {
                pCrtCh++;
                state = 10;
            }
        case 10:
            if ((ch >= '0' && ch <= '9') || (ch >= 'a' && ch <= 'f') || (ch >= 'A' && ch <= 'F')) pCrtCh++;
            else
            state=8;
        }
    }
}

int main()
{
    FILE *file = fopen("0.c", "r");
    if (!file)
    {
        err("Could not open file");
        return -1;
    }

    char buffer[256];
    while (fscanf(file, "%255s", buffer) == 1)
    {
        Token *token = addTk(T_ID);
        token->datavalue.text = strdup(buffer);
    }

    fclose(file);

    print_tokens();
    freeTokens();

    return 0;
}
