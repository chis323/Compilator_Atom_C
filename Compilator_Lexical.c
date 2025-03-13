#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#define MAX_TOKEN_LENGTH 100

typedef enum {
    START, IDENTIFIER, NUMBER, OPERATOR, SYMBOL, STRING, COMMENT, ERROR, END
} State;

typedef enum {
    TOKEN_IDENTIFIER, TOKEN_NUMBER_ZEC, TOKEN_STRING, TOKEN_OPERATOR, TOKEN_SYMBOL, TOKEN_KEYWORD, TOKEN_ERROR, TOKEN_EOF
} TokenType;

const char *keywords[] = {"if", "else", "while", "return", "int", "char", "void"};
const int num_keywords = sizeof(keywords) / sizeof(keywords[0]);

typedef struct {
    TokenType type;
    char value[MAX_TOKEN_LENGTH];
    int id;
} Token;

Token *tokens = NULL;
int token_count = 0;

int is_keyword(const char *word) {
    for (int i = 0; i < num_keywords; i++) {
        if (strcmp(word, keywords[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

void add_token(Token token) {
    token.id = token_count;
    tokens = realloc(tokens, (token_count + 1) * sizeof(Token));
    if (!tokens) {
        perror("Memory allocation error");
        exit(1);
    }
    tokens[token_count++] = token;
}

Token get_next_token(FILE *file) {
    Token token;
    State state = START;
    char buffer[MAX_TOKEN_LENGTH] = "";
    int index = 0;
    int c;
    
    while ((c = fgetc(file)) != EOF) {
        switch (state) {
            case START:
                if (isspace(c)) continue;
                if (isalpha(c) || c == '_') {
                    state = IDENTIFIER;
                } else if (isdigit(c)) {
                    state = NUMBER;
                } else if (strchr("+-*/=<>!&|", c)) {
                    state = OPERATOR;
                } else if (strchr("(){}[],;", c)) {
                    state = SYMBOL;
                } else {
                    state = ERROR;
                }
                buffer[index++] = c;
                break;
            
            case IDENTIFIER:
                if (isalnum(c) || c == '_') {
                    buffer[index++] = c;
                } else {
                    ungetc(c, file);
                    buffer[index] = '\0';
                    token.type = is_keyword(buffer) ? TOKEN_KEYWORD : TOKEN_IDENTIFIER;
                    strcpy(token.value, buffer);
                    return token;
                }
                break;
            
            case NUMBER:
                if (isdigit(c)) {
                    buffer[index++] = c;
                } else {
                    ungetc(c, file);
                    buffer[index] = '\0';
                    token.type = TOKEN_NUMBER_ZEC;
                    strcpy(token.value, buffer);
                    return token;
                }
                break;
            
            case OPERATOR:
                if (strchr("=|&", c)) {
                    buffer[index++] = c;
                } else {
                    ungetc(c, file);
                }
                buffer[index] = '\0';
                token.type = TOKEN_OPERATOR;
                strcpy(token.value, buffer);
                return token;
                
            case SYMBOL:
                buffer[index] = '\0';
                token.type = TOKEN_SYMBOL;
                strcpy(token.value, buffer);
                return token;
                
            case ERROR:
                buffer[index] = '\0';
                token.type = TOKEN_ERROR;
                strcpy(token.value, buffer);
                return token;
        }
    }
    
    token.type = TOKEN_EOF;
    strcpy(token.value, "EOF");
    return token;
}

void print_tokens() {
    for (int i = 0; i < token_count; i++) {
        printf("ID: %d, Type: %d, Value: %s\n", i, tokens[i].type, tokens[i].value);
    }
}

int main() {
    FILE *file = fopen("0.c", "r");
    if (!file) {
        perror("Error opening file");
        return 1;
    }
    
    Token token;
    do {
        token = get_next_token(file);
        add_token(token);
    } while (token.type != TOKEN_EOF);
    
    fclose(file);
    print_tokens();
    free(tokens);
    return 0;
}
