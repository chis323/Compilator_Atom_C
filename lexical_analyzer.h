// lexical_analyzer.h

#ifndef LEXICAL_ANALYZER_H
#define LEXICAL_ANALYZER_H

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#define MaxTokenLen 100
#define MaxTokensInList 1000

typedef enum {
    T_IDENTIFIER,
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
    T_CONSTANT_INT,
    T_CONSTANT_REAL,
    T_CONSTANT_CHAR,
    T_CONSTANT_STRING,
    T_COMMA,
    T_SEMICOLON,
    T_LEFT_PAR,
    T_RIGHT_PAR,
    T_LEFT_BRACKET,
    T_RIGHT_BRACKET,
    T_LEFT_ACCOLADE,
    T_RIGHT_ACCOLADE,
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
    T_NOT_EQUAL,
    T_LESS,
    T_LESS_EQUAL,
    T_GREATER,
    T_GREATER_EQUAL,
    T_SPACE,
    T_COMMENT,
    T_LINE_COMMENT,
    T_EOF
} TokenType;

union Value {
    char string_value[MaxTokenLen];
    char char_value;
};

typedef struct {
    TokenType type;
    union Value value;
} Token;

extern Token token_list[MaxTokensInList];
extern int token_list_index;

const char *token_type_to_string(TokenType type);
TokenType get_keyword_type(const char *word);
int integer(const char *str);
int real_constant(const char *str);
void add_token(TokenType type, const char *value);
void Token_Formation(const char *formation);
void token_delimiter(char chr);
void token_operators(char curr, char next, FILE *file);
void process_char(char curr, FILE *file, char *formation, int *index, int *is_formation, int *in_string);
void tokenize(const char *filename);
Token *get_tokens();

#endif
