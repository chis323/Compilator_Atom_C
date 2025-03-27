#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#define MAX_FORMATION_LENGTH 100
#define NUM_KEYWORDS 11
#define MAX_LIST_SIZE 1000

union Value
{
    char string_value[MAX_FORMATION_LENGTH];
    char char_value;
};

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

typedef struct
{
    TokenType type;
    union Value value;
} Token;

Token token_list[MAX_LIST_SIZE];
int token_list_index = 0;

const char *key_word_array[NUM_KEYWORDS] = {
    "break", "char", "double", "else", "for", "if",
    "int", "return", "struct", "void", "while"};

const char *token_type_to_string(TokenType type)
{
    switch (type)
    {
    case T_ID:
        return "T_ID";
    case T_BREAK:
        return "T_BREAK";
    case T_CHAR:
        return "T_CHAR";
    case T_DOUBLE:
        return "T_DOUBLE";
    case T_ELSE:
        return "T_ELSE";
    case T_FOR:
        return "T_FOR";
    case T_IF:
        return "T_IF";
    case T_INT:
        return "T_INT";
    case T_RETURN:
        return "T_RETURN";
    case T_STRUCT:
        return "T_STRUCT";
    case T_VOID:
        return "T_VOID";
    case T_WHILE:
        return "T_WHILE";
    case T_CONSTINT:
        return "T_CONSTINT";
    case T_CONSTREAL:
        return "T_CONSTREAL";
    case T_CONSTCHAR:
        return "T_CONSTCHAR";
    case T_CONSTSTRING:
        return "T_CONSTSTRING";
    case T_COMMA:
        return "T_COMMA";
    case T_SEMICOLON:
        return "T_SEMICOLON";
    case T_LPAR:
        return "T_LPAR";
    case T_RPAR:
        return "T_RPAR";
    case T_LBRACKET:
        return "T_LBRACKET";
    case T_RBRACKET:
        return "T_RBRACKET";
    case T_LACC:
        return "T_LACC";
    case T_RACC:
        return "T_RACC";
    case T_ADD:
        return "T_ADD";
    case T_SUB:
        return "T_SUB";
    case T_MULT:
        return "T_MULT";
    case T_DIV:
        return "T_DIV";
    case T_DOT:
        return "T_DOT";
    case T_AND:
        return "T_AND";
    case T_OR:
        return "T_OR";
    case T_NOT:
        return "T_NOT";
    case T_ASSIGN:
        return "T_ASSIGN";
    case T_EQUAL:
        return "T_EQUAL";
    case T_NOTEQUAL:
        return "T_NOTEQUAL";
    case T_LESS:
        return "T_LESS";
    case T_LESSEQUAL:
        return "T_LESSEQUAL";
    case T_GREATER:
        return "T_GREATER";
    case T_GREATEREQUAL:
        return "T_GREATEREQUAL";
    case T_SPACE:
        return "T_SPACE";
    case T_COMMENT:
        return "T_COMMENT";
    case T_LINECOMMENT:
        return "T_LINECOMMENT";
    case T_END:
        return "T_END";
    default:
        return "UNKNOWN";
    }
}

TokenType get_keyword_type(const char *word)
{
    if (strcmp(word, "break") == 0)
        return T_BREAK;
    if (strcmp(word, "char") == 0)
        return T_CHAR;
    if (strcmp(word, "double") == 0)
        return T_DOUBLE;
    if (strcmp(word, "else") == 0)
        return T_ELSE;
    if (strcmp(word, "for") == 0)
        return T_FOR;
    if (strcmp(word, "if") == 0)
        return T_IF;
    if (strcmp(word, "int") == 0)
        return T_INT;
    if (strcmp(word, "return") == 0)
        return T_RETURN;
    if (strcmp(word, "struct") == 0)
        return T_STRUCT;
    if (strcmp(word, "void") == 0)
        return T_VOID;
    if (strcmp(word, "while") == 0)
        return T_WHILE;
    return T_ID;
}

int integer(const char *str)
{
    for (int i = 0; str[i] != '\0'; i++)
    {
        if (!isdigit(str[i]))
        {
            return 0;
        }
    }
    return 1;
}

int is_float_constant(const char *str) // 
{
    int i = 0, dot_seen = 0, e_seen = 0;

    if (!isdigit(str[i]) && str[i] != '.')
        return 0;

    while (str[i] != '\0')
    {
        if (str[i] == '.')
        {
            if (dot_seen || e_seen)
                return 0;
            dot_seen = 1;
        }
        else if (str[i] == 'e' || str[i] == 'E')
        {
            if (e_seen || i == 0 || !isdigit(str[i - 1]))
                return 0;
            e_seen = 1;
            if (str[i + 1] == '+' || str[i + 1] == '-')
                i++;
            if (!isdigit(str[i + 1]))
                return 0;
        }
        else if (!isdigit(str[i]))
        {
            return 0;
        }
        i++;
    }
    return 1;
}

void add_token(TokenType type, const char *value)
{
    if (token_list_index >= MAX_LIST_SIZE)
    {
        printf("Token list is full.\n");
        return;
    }
    token_list[token_list_index].type = type;
    strncpy(token_list[token_list_index].value.string_value, value, MAX_FORMATION_LENGTH);
    token_list[token_list_index].value.string_value[MAX_FORMATION_LENGTH - 1] = '\0';
    token_list_index++;
}

void process_formation(const char *formation)
{
    TokenType type = get_keyword_type(formation);
    if (type != T_ID)
    {
        add_token(type, formation);
    }
    else if (integer(formation))
    {
        add_token(T_CONSTINT, formation);
    }
    else if (is_float_constant(formation))
    {
        add_token(T_CONSTREAL, formation);
    }
    else
    {
        add_token(T_ID, formation);
    }
}

void process_delimiter(char chr)
{
    switch (chr)
    {
    case ',':
        add_token(T_COMMA, ",");
        break;
    case ';':
        add_token(T_SEMICOLON, ";");
        break;
    case '(':
        add_token(T_LPAR, "(");
        break;
    case ')':
        add_token(T_RPAR, ")");
        break;
    case '[':
        add_token(T_LBRACKET, "[");
        break;
    case ']':
        add_token(T_RBRACKET, "]");
        break;
    case '{':
        add_token(T_LACC, "{");
        break;
    case '}':
        add_token(T_RACC, "}");
        break;
    }
}

void process_operator(char curr, char next, FILE *file)
{
    switch (curr)
    {
    case '+':
        add_token(T_ADD, "+");
        break;
    case '-':
        add_token(T_SUB, "-");
        break;
    case '*':
        add_token(T_MULT, "*");
        break;
    case '/':
        if (next == '/')
        {
            char comment[MAX_FORMATION_LENGTH];
            int i = 0;
            while ((comment[i] = fgetc(file)) != EOF && comment[i] != '\n')
            {
                i++;
            }
            comment[i] = '\0';
            add_token(T_LINECOMMENT, comment);
        }
        else if (next == '*')
        {
            char comment[MAX_FORMATION_LENGTH];
            int i = 0;
            while ((comment[i] = fgetc(file)) != EOF)
            {
                if (comment[i] == '*' && (comment[i + 1] = fgetc(file)) == '/')
                {
                    comment[i + 2] = '\0';
                    break;
                }
                i++;
            }
            add_token(T_COMMENT, comment);
        }
        else
        {
            add_token(T_DIV, "/");
        }
        break;
    case '.':
        add_token(T_DOT, ".");
        break;
    case '!':
        add_token(next == '=' ? T_NOTEQUAL : T_NOT, next == '=' ? "!=" : "!");
        break;
    case '=':
        add_token(next == '=' ? T_EQUAL : T_ASSIGN, next == '=' ? "==" : "=");
        break;
    case '<':
        add_token(next == '=' ? T_LESSEQUAL : T_LESS, next == '=' ? "<=" : "<");
        break;
    case '>':
        add_token(next == '=' ? T_GREATEREQUAL : T_GREATER, next == '=' ? ">=" : ">");
        break;
    case '&':
        if (next == '&')
            add_token(T_AND, "&&");
        break;
    case '|':
        if (next == '|')
            add_token(T_OR, "||");
        break;
    }
    if (next == '=' || (curr == '&' && next == '&') || (curr == '|' && next == '|'))
    {
        fgetc(file);
    }
}

void process_char(char curr, FILE *file, char *formation, int *index, int *is_formation, int *in_string)
{
    char next = fgetc(file);
    ungetc(next, file);

    if (isspace(curr))
    {
        if (*is_formation)
        {
            formation[*index] = '\0';
            process_formation(formation);
            *is_formation = 0;
            *index = 0;
        }
        if (curr == '\n' || curr == '\r' || curr == '\t')
        {
            add_token(T_SPACE, " ");
        }
    }
    else if (*in_string)
    {
        if (curr == '"')
        {
            formation[*index] = '\0';
            add_token(T_CONSTSTRING, formation);
            *in_string = 0;
            *index = 0;
        }
        else
        {
            formation[(*index)++] = curr;
        }
    }
    else if (curr == '"')
    {
        *in_string = 1;
        *index = 0;
    }
    else if (curr == '\'')
    {
        char char_literal = fgetc(file);
        if (char_literal != EOF && fgetc(file) == '\'')
        {
            char char_value[2] = {char_literal, '\0'};
            add_token(T_CONSTCHAR, char_value);
        }
        else
        {
            add_token(T_END, "Invalid character literal");
        }
    }
    else if (isalnum(curr) || curr == '.' || curr == '_')
    {
        if (!(*is_formation))
        {
            *index = 0;
        }
        *is_formation = 1;
        if (*index < MAX_FORMATION_LENGTH - 1)
        {
            formation[(*index)++] = curr;
        }
    }
    else
    {
        if (*is_formation)
        {
            formation[*index] = '\0';
            process_formation(formation);
            *is_formation = 0;
        }
        process_delimiter(curr);
        process_operator(curr, next, file);
    }
}

int main()
{
    char filename[] = "0.c";

    FILE *compiling_file = fopen(filename, "r");
    if (compiling_file == NULL)
    {
        printf("Error opening file: %s\n", filename);
        return 1;
    }

    char formation[MAX_FORMATION_LENGTH];
    int index = 0;
    char curr;
    int is_formation = 0;
    int in_string = 0;

    while ((curr = fgetc(compiling_file)) != EOF)
    {
        process_char(curr, compiling_file, formation, &index, &is_formation, &in_string);
    }

    if (is_formation)
    {
        formation[index] = '\0';
        process_formation(formation);
    }

    fclose(compiling_file);

    for (int i = 0; i < token_list_index; i++)
    {
        Token token = token_list[i];
        printf("Token: %s       | Value: %s\n", token_type_to_string(token.type), token.value.string_value);
    }

    return 0;
}
