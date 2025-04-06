#include "lexical_analyzer.h"

Token token_list[MaxTokensInList];
int token_list_index;

const char *token_type_to_string(TokenType type) // convert the token into string so i can print it
{
    switch (type)
    {
    case T_IDENTIFIER:
        return "T_IDENTIFIER";
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
    case T_CONSTANT_INT:
        return "T_CONSTANT_INT";
    case T_CONSTANT_REAL:
        return "T_CONSTANT_REAL";
    case T_CONSTANT_CHAR:
        return "T_CONSTANT_CHAR";
    case T_CONSTANT_STRING:
        return "T_CONSTANT_STRING";
    case T_COMMA:
        return "T_COMMA";
    case T_SEMICOLON:
        return "T_SEMICOLON";
    case T_LEFT_PAR:
        return "T_LEFT_PAR";
    case T_RIGHT_PAR:
        return "T_RIGHT_PAR";
    case T_LEFT_BRACKET:
        return "T_LEFT_BRACKET";
    case T_RIGHT_BRACKET:
        return "T_RIGHT_BRACKET";
    case T_LEFT_ACCOLADE:
        return "T_LEFT_ACCOLADE";
    case T_RIGHT_ACCOLADE:
        return "T_RIGHT_ACCOLADE";
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
    case T_NOT_EQUAL:
        return "T_NOT_EQUAL";
    case T_LESS:
        return "T_LESS";
    case T_LESS_EQUAL:
        return "T_LESS_EQUAL";
    case T_GREATER:
        return "T_GREATER";
    case T_GREATER_EQUAL:
        return "T_GREATER_EQUAL";
    case T_SPACE:
        return "T_SPACE";
    case T_COMMENT:
        return "T_COMMENT";
    case T_LINE_COMMENT:
        return "T_LINE_COMMENT";
    case T_EOF:
        return "T_EOF";
    default:
        return "UNKNOWN";
    }
}

TokenType get_keyword_type(const char *word) // here i check if the token is a keyword and i return it
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
    return T_IDENTIFIER; // daca nu e niciun keyword, atunci il returnez ca identifier si il verific mai departe in main
}

int integer(const char *str)
{
    if (str == NULL || *str == '\0')
        return 0;
    int i = 0;
    // Hexadecimal case
    if (str[i] == '0' && (str[i + 1] == 'x' || str[i + 1] == 'X')) // caut 0x sau x
    {
        i += 2;                // trec de 0x
        if (!isxdigit(str[i])) // verific ca dupa 0x sa fie macar o cifra
            return 0;
        for (; str[i] != '\0'; i++) // restul cifrelor din numar
        {
            if (!isxdigit(str[i]))
                return 0;
        }
        return 1;
    }

    // Octal de ex. 006
    if (str[i] == '0') // verific sa fie primu caracter 0
    {
        for (i++; str[i] != '\0'; i++) // restu caracterelor sa fie intre 0-7
        {
            if (str[i] < '0' || str[i] > '7')
                return 0;
        }
        return 1;
    }
    // integer
    if (str[0] < '1' || str[0] > '9') // prima cifra de la 1 la 9
        return 0;
    for (i = 1; str[i] != '\0'; i++)
    {
        if (!isdigit(str[i]))
            return 0;
    }
    return 1;
}

int real_constant(const char *str) // exponent si real
{
    int i = 0, dot = 0, exp = 0;

    if (!isdigit(str[i]) && str[i] != '.') // gol sau nu incepe cu .
        return 0;

    while (str[i] != '\0')
    {
        if (str[i] == '.')
        {
            if (dot != 0 || exp != 0)
                return 0;
            dot = 1; // daca gasesc un punct activez flagul de dot
        }
        else if (str[i] == 'e' || str[i] == 'E')
        {
            if (exp != 0 || i == 0 || !isdigit(str[i - 1]))
                return 0; // daca deja am gasit un E sau primu element este E sau caracterul de dinainte nu e cifra
            exp = 1;
            if (str[i + 1] == '+' || str[i + 1] == '-')
                i++; // dupa e trebuie sa vina + sau - de ex. E+10 e-3
            if (!isdigit(str[i + 1]))
                return 0;
        }
        else if (!isdigit(str[i]))
        {
            return 0;
        }
        i++; // urmatorul caracter din token
    }
    return 1;
}

void add_token(TokenType type, const char *value)
{
    if (token_list_index >= MaxTokensInList)
    {
        printf("Token list is full.\n");
        return;
    }
    token_list[token_list_index].type = type;                                     // ii dau tipul de token
    strncpy(token_list[token_list_index].value.string_value, value, MaxTokenLen); // copiez in array valoarea de la token
    token_list[token_list_index].value.string_value[MaxTokenLen - 1] = '\0';      // il inchid cu null ca sa nu puna caractere random in restul pozitiilor libere
    token_list_index++;
}

void Token_Formation(const char *formation)
{
    TokenType type = get_keyword_type(formation);

    if (type != T_IDENTIFIER)
    {
        add_token(type, formation);
    }
    else if (integer(formation))
    {
        add_token(T_CONSTANT_INT, formation);
    }
    else if (real_constant(formation))
    {
        add_token(T_CONSTANT_REAL, formation);
    }
    else
    {
        add_token(T_IDENTIFIER, formation);
    }
}

void token_delimiter(char chr)
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
        add_token(T_LEFT_PAR, "(");
        break;
    case ')':
        add_token(T_RIGHT_PAR, ")");
        break;
    case '[':
        add_token(T_LEFT_BRACKET, "[");
        break;
    case ']':
        add_token(T_RIGHT_BRACKET, "]");
        break;
    case '{':
        add_token(T_LEFT_ACCOLADE, "{");
        break;
    case '}':
        add_token(T_RIGHT_ACCOLADE, "}");
        break;
    }
}

void token_operators(char curr, char next, FILE *file)
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
            char comment[MaxTokenLen];
            int i = 0;
            fgetc(file); // consume the first '/' from '//' pair
            while ((comment[i] = fgetc(file)) != EOF && comment[i] != '\n')
            {
                i++;
            }
            comment[i] = '\0';
            add_token(T_LINE_COMMENT, comment);
        }
        else if (next == '*') // Multi-line comment: /* ... */
        {
            char comment[MaxTokenLen];
            int i = 0;

            // Start collecting AFTER the initial `/*`
            while (1)
            {
                char c = fgetc(file);
                if (c == EOF)
                    break;

                if (c == '*')
                {
                    char nextChar = fgetc(file);
                    if (nextChar == '/')
                    {
                        // End of comment
                        break;
                    }
                    else
                    {
                        // Not end, include `*` and continue
                        comment[i++] = '*';
                        if (nextChar != EOF)
                            ungetc(nextChar, file);
                    }
                }
                else
                {
                    comment[i++] = c;
                }

                if (i >= MaxTokenLen - 1)
                    break;
            }
            comment[i] = '\0'; // terminate string
            add_token(T_COMMENT, comment);
        }

        else
        {
            add_token(T_DIV, "/"); // altfel e tot operator divizor
        }
        break;
    case '!':
        if (next == '=')
            add_token(T_NOT_EQUAL, "!=");
        else
            add_token(T_NOT, "!");
        break;

    case '=':
        if (next == '=')
            add_token(T_EQUAL, "==");
        else
            add_token(T_ASSIGN, "=");
        break;

    case '<':
        if (next == '=')
            add_token(T_LESS_EQUAL, "<=");
        else
            add_token(T_LESS, "<");
        break;

    case '>':
        if (next == '=')
            add_token(T_GREATER_EQUAL, ">=");
        else
            add_token(T_GREATER, ">");
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
        fgetc(file); // la cazurile de doua caractere dau skip la al doilea
    }
}

void process_char(char curr, FILE *file, char *formation, int *index, int *is_formation, int *in_string)
{
    char next = fgetc(file); // next e urmatorul caracter
    ungetc(next, file);      // pun caracterul inapoi pt cazul in care vreau sa il mai citesc
    if (isspace(curr))
    { // despartite prin spatiu
        if (*is_formation != 0)
        {
            formation[*index] = '\0'; // resetez si adaug in array
            Token_Formation(formation);
            *is_formation = 0;
            *index = 0;
        }
        if (curr == '\n')
        {
            add_token(T_SPACE, " ");
        }
        return;
    }
    // cazul de string
    if (*in_string)
    {
        if (curr == '"') // daca deja suntem intr-un string atunci am gasit ghilimele de inchidere, adaugam token de string si resetam
        {
            formation[*index] = '\0';
            add_token(T_CONSTANT_STRING, formation);
            *in_string = 0;
            *index = 0;
        }
        else
        {
            formation[(*index)++] = curr;
        }
        return;
    }

    if (curr == '"') // ghilimele de inceput de string
    {
        *in_string = 1;
        *index = 0;
        return;
    }
    // cazul  'A'
    if (curr == '\'')
    {
        char char_literal = fgetc(file);
        if (char_literal != EOF && fgetc(file) == '\'')
        {
            char char_value[2] = {char_literal, '\0'}; // citesc urmatorul caracter si trebuie sa fie tot '
            add_token(T_CONSTANT_CHAR, char_value);
        }
        else
            return;
    }
    //_123 or .asdanfj or 213.23.45 errors
    if (isalnum(curr) || curr == '.' || curr == '_')
    {
        if (!(*is_formation))
        {
            *index = 0;
        }
        *is_formation = 1;
        if (*index < MaxTokenLen - 1)
        {
            formation[(*index)++] = curr;
        }
        return;
    }

    if (*is_formation)
    {
        formation[*index] = '\0';   // scap de caracterele random
        Token_Formation(formation); // adaug in array
        *is_formation = 0;          // resetez
    }

    token_delimiter(curr);
    token_operators(curr, next, file); // apelez functiile de delimitator sau operator
}

Token *get_tokens()
{
    return token_list;
}

void tokenize(const char *filename)
{
    FILE *compiling_file = fopen(filename, "r");
    if (compiling_file == NULL)
    {
        printf("Error opening file: %s\n", filename);
        return;
    }

    char formation[MaxTokenLen];
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
        Token_Formation(formation);
    }
    add_token(T_EOF, "EOF");
    fclose(compiling_file);
}

int main_secondary()
{
    tokenize("9.c");
    for (int i = 0; i < token_list_index; i++)
    {
        Token token = token_list[i];
        printf("Token: %s       Value: %s\n", token_type_to_string(token.type), token.value.string_value);
    }
    return 0;
}
