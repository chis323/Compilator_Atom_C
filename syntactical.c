#include "lexical_analyzer.h"

int current_token_index = 0;
Token *current_token;
int verbose = 0; // if 0 => less messages, if 1 => more debug messages

// the function prototypes are needed because some of the functions may be used before they are defined
void advance();
void match(TokenType expected_type);
void skip_whitespace_and_comments();
void parse_program();
void parse_unit();
void parse_struct_declaration();
void parse_variable_declaration();
void parse_typebase_specifier();
void parse_function_declaration();
void parse_funcArg();
void parse_stm();
void parse_whileFor_statement();
void parse_ifElse_statement();
void parse_returnBreak_statement();
void parse_stmCompound();
void parse_expression();
void parse_expression_statement();
void parse_assignment_expression();
void parse_logical_or_expression();
void parse_logical_and_expression();
void parse_equality_expression();
void parse_relational_expression();
void parse_additive_expression();
void parse_multiplicative_expression();
void parse_unary_expression();
void parse_postfix_expression();
void parse_primary_expression();

void advance() // Moves the parser to the next token
{
    if (current_token_index + 1 < token_list_index)
    {
        current_token_index++;
        current_token = &token_list[current_token_index];
    }
    else
    {
        current_token = NULL; // Marking the end of input
    }
}

void match(TokenType expected_type) // checks if the current token matches what we expect
{
    if (current_token->type == expected_type)
    {
        printf("Matched %s: %s\n", token_type_to_string(current_token->type), current_token->value);
        advance();
    }
    else
    {
        printf("Syntax error at token %d: Expected %s but found %s (%s)\n",
               current_token_index,
               token_type_to_string(expected_type),
               token_type_to_string(current_token->type),
               current_token->value);
        exit(1);
    }
}

void skip_whitespace_and_comments() // to avoid parsing what is not needed
{
    while (current_token != NULL &&
           (current_token->type == T_SPACE ||
            current_token->type == T_COMMENT ||
            current_token->type == T_LINE_COMMENT))
    {
        printf("Skipping %s\n", token_type_to_string(current_token->type));
        advance();
    }
}

// The entry point of the syntactical analyzer
void parse_program()
{
    printf("\n Starting program parsing\n");
    skip_whitespace_and_comments();

    while (current_token->type != T_EOF)
    {
        parse_unit();
        skip_whitespace_and_comments();
    }
    printf("\nProgram parsed successfully!\n");
}

void parse_unit()
{
    printf("Parsing unit\n");

    // Continue parsing until EOF
    while (current_token->type != T_EOF)
    {
        skip_whitespace_and_comments();

        // Error recovery: skip until a valid declaration token
        while (current_token->type != T_EOF &&
               !(current_token->type == T_INT ||
                 current_token->type == T_CHAR ||
                 current_token->type == T_VOID ||
                 current_token->type == T_DOUBLE ||
                 current_token->type == T_STRUCT))
        {
            printf("Skip unexpected token: %s (%s)\n",
                   token_type_to_string(current_token->type),
                   current_token->value);
            advance();
            skip_whitespace_and_comments();
        }

        if (current_token->type == T_EOF)
            break;

        // Struct definition or struct-based declaration
        if (current_token->type == T_STRUCT)
        {
            int i = current_token_index + 1;

            // Skip whitespace/comments
            while (i < token_list_index &&
                   (token_list[i].type == T_SPACE ||
                    token_list[i].type == T_COMMENT ||
                    token_list[i].type == T_LINE_COMMENT))
                i++;

            if (i < token_list_index)
            {
                if (token_list[i].type == T_IDENTIFIER)
                {
                    int id_index = i;
                    i++;

                    // Skip whitespace/comments
                    while (i < token_list_index &&
                           (token_list[i].type == T_SPACE ||
                            token_list[i].type == T_COMMENT ||
                            token_list[i].type == T_LINE_COMMENT))
                        i++;

                    if (i < token_list_index &&
                        token_list[i].type == T_LEFT_ACCOLADE)
                    {
                        // Struct definition: struct Name { ... };
                        parse_struct_declaration();
                        continue;
                    }
                    else
                    {
                        // Struct-based variable declaration
                        parse_variable_declaration();
                        continue;
                    }
                }
                else if (token_list[i].type == T_LEFT_ACCOLADE)
                {
                    // Anonymous struct
                    parse_struct_declaration();
                    continue;
                }
                else
                {
                    printf("Syntax error: expected identifier or '{' after 'struct'\n");
                    return;
                }
            }
        }

        // Function declaration
        parse_function_declaration();
    }
}

void parse_struct_declaration()
{
    printf(" Parsing struct declaration\n");
    Type *t = getType(current_token->type);
    match(T_STRUCT);
    skip_whitespace_and_comments();
    char *name = current_token->value.string_value;
    match(T_IDENTIFIER);
    addVar(name, 0, t);
    skip_whitespace_and_comments();
    match(T_LEFT_ACCOLADE);
    symbols[symbolCount - 1].cls = CLS_STRUCT;
    crtStruct = &symbols[symbolCount - 1];
    skip_whitespace_and_comments();
    crtStruct->members = calloc(MAX_ARGS, sizeof(Symbol));

    if (current_token->type != T_RIGHT_ACCOLADE)
    {
        parse_variable_declaration();
        skip_whitespace_and_comments();
    }
    match(T_RIGHT_ACCOLADE);
    match(T_SEMICOLON);
    crtStruct = NULL;
}

void parse_typebase_specifier()
{
    printf("  Parsing type specifier\n");
    switch (current_token->type)
    {
    case T_IDENTIFIER:
    case T_INT:
    case T_CHAR:
    case T_DOUBLE:
    case T_VOID:
        match(current_token->type);
        break;
    case T_STRUCT:
        match(T_STRUCT);
        if (current_token->type == T_IDENTIFIER)
        {
            match(T_IDENTIFIER); // struct tag
        }
        break;
    default:
        printf("Syntax error: Expected typebase specifier but found %s\n", token_type_to_string(current_token->type));
        exit(1);
    }
}

void parse_variable_declaration()
{
    printf("  Parsing variable declaration\n");
    Type *t = getType(current_token->type);
    parse_typebase_specifier();
    skip_whitespace_and_comments();

    int structIndex = 0;

    do
    {
        char *name = current_token->value.string_value;
        match(T_IDENTIFIER);
        if(crtStruct != NULL)
        {
            addVar(name, &structIndex, t);
        }
        else
        {
            addVar(name, 0, t);
        }
        skip_whitespace_and_comments();

        // array declaration with complex expressions like[20 / 4 + 5]
        while (current_token->type == T_LEFT_BRACKET)
        {
            match(T_LEFT_BRACKET);
            parse_expression();
            match(T_RIGHT_BRACKET);
            skip_whitespace_and_comments();
        }

        // initialization
        if (current_token->type == T_ASSIGN)
        {
            match(T_ASSIGN);
            parse_expression();
        }

        skip_whitespace_and_comments();

    } while (current_token->type == T_COMMA && (match(T_COMMA), skip_whitespace_and_comments(), 1));

    match(T_SEMICOLON);
}

void parse_function_declaration()
{
    printf("  Parsing function declaration\n");
    Type *t = getType(current_token->type);
    parse_typebase_specifier();
    skip_whitespace_and_comments();
    if (current_token->type == T_MULT) // array parameter
    {
        match(T_MULT);
    }
    char *name = current_token->value.string_value;
    match(T_IDENTIFIER);
    addVar(name, 0, t);
    symbols[symbolCount - 1].cls = CLS_FUNC;
    skip_whitespace_and_comments();
    match(T_LEFT_PAR);
    crtFunc = &symbols[symbolCount - 1];
    if (current_token->type != T_RIGHT_PAR)
    {
        parse_funcArg();
    }
    match(T_RIGHT_PAR);
    parse_stmCompound();
    crtFunc = NULL;
}

void parse_funcArg()
{
    printf("    Parsing parameter list\n");
    Type *t = getType(current_token->type);
    parse_typebase_specifier();
    skip_whitespace_and_comments();
    char *name = current_token->value.string_value;
    match(T_IDENTIFIER);
    crtFunc->args = calloc(MAX_ARGS, sizeof(Symbol));
    printf("NUME FUCT %s \n", crtFunc->name);
    int argIndex = 0;
    Symbol *arg = addSymbol(crtFunc->args, &argIndex, name, CLS_VAR);
    if (current_token->type == T_LEFT_BRACKET) // array parameter
    {
        match(T_LEFT_BRACKET);
        match(T_RIGHT_BRACKET);
    }
    while (current_token->type == T_COMMA)
    {
        match(T_COMMA);
        Type *t = getType(current_token->type);
        parse_typebase_specifier();
        char *name = current_token->value.string_value;
        match(T_IDENTIFIER);
        Symbol *arg = addSymbol(crtFunc->args, &argIndex, name, CLS_VAR);
        if (current_token->type == T_LEFT_BRACKET)
        {
            match(T_LEFT_BRACKET);
            match(T_RIGHT_BRACKET);
        }
    }
}

void parse_stmCompound()
{
    printf("  Parsing compound statement\n");
    skip_whitespace_and_comments();
    match(T_LEFT_ACCOLADE);
    skip_whitespace_and_comments();

    while (current_token->type != T_RIGHT_ACCOLADE && current_token->type != T_EOF)
    {
        parse_stm();
        skip_whitespace_and_comments();
    }
    match(T_RIGHT_ACCOLADE);
}

void parse_stm()
{
    printf("    Parsing statement\n");
    switch (current_token->type)
    {
    case T_LEFT_ACCOLADE:
        parse_stmCompound();
        break;
    case T_IF:
        parse_ifElse_statement();
        break;
    case T_FOR:
    case T_WHILE:
        parse_whileFor_statement();
        break;
    case T_RETURN:
    case T_BREAK:
        parse_returnBreak_statement();
        break;
    case T_SEMICOLON:
        parse_expression_statement(); // Empty statement
        break;
    case T_INT:
    case T_CHAR:
    case T_DOUBLE:
    case T_VOID:
    case T_STRUCT:
        parse_variable_declaration();
        break;
    default:
        parse_expression_statement(); // Default to expression statement
        break;
    }
}

void parse_expression_statement() // for parsing a full expr with ";" at the end
{
    printf("      Parsing expression statement\n");
    if (current_token->type != T_SEMICOLON)
    {
        parse_expression();
    }
    match(T_SEMICOLON);
}

void parse_ifElse_statement() // part of stm
{
    printf("      Parsing if/else statement\n");
    match(T_IF);
    match(T_LEFT_PAR);
    parse_expression();
    match(T_RIGHT_PAR);
    parse_stm();
    skip_whitespace_and_comments();
    if (current_token->type == T_ELSE)
    {
        match(T_ELSE);
        parse_stm();
    }
}

void parse_whileFor_statement() // part of stm
{
    printf("      Parsing while/for statement\n");
    if (current_token->type == T_WHILE)
    {
        match(T_WHILE);
        match(T_LEFT_PAR);
        parse_expression();
        match(T_RIGHT_PAR);
        parse_stm();
    }
    else
    {
        match(T_FOR);
        match(T_LEFT_PAR);

        if (current_token->type != T_SEMICOLON)
        {
            parse_expression();
        }
        match(T_SEMICOLON);
        if (current_token->type != T_SEMICOLON)
        {
            parse_expression();
        }
        match(T_SEMICOLON);
        if (current_token->type != T_RIGHT_PAR)
        {
            parse_expression();
        }
        match(T_RIGHT_PAR);
        parse_stm();
    }
}

void parse_returnBreak_statement() // part of stm
{
    if (verbose == 1)
        printf("      Parsing return/break statement\n");
    if (current_token->type == T_RETURN)
    {
        match(T_RETURN);
        if (current_token->type != T_SEMICOLON)
        {
            parse_expression();
        }
        match(T_SEMICOLON);
    }
    else
    {
        match(T_BREAK);
        match(T_SEMICOLON);
    }
}

void parse_expression() // for parsing just expressions, without ";" at the end
{
    printf("        Parsing expression\n");
    parse_assignment_expression();

    while (current_token->type == T_COMMA)
    {
        match(T_COMMA);
        parse_assignment_expression();
    }
}

void parse_assignment_expression()
{
    if (verbose == 1)
        printf("          Parsing assignment expression\n");

    // first parsing the left side (could be an array access)
    parse_logical_or_expression();

    if (current_token->type == T_ASSIGN) // checking for assignment operator
    {
        match(T_ASSIGN);
        parse_assignment_expression(); // parsing the right side
    }
}

void parse_logical_or_expression()
{
    if (verbose == 1)
        printf("            Parsing logical OR expression\n");
    parse_logical_and_expression();

    while (current_token->type == T_OR)
    {
        match(T_OR);
        parse_logical_and_expression();
    }
}

void parse_logical_and_expression()
{
    if (verbose == 1)
        printf("            Parsing logical AND expression\n");
    parse_equality_expression();

    while (current_token->type == T_AND)
    {
        match(T_AND);
        parse_equality_expression();
    }
}

void parse_equality_expression()
{
    if (verbose == 1)
        printf("              Parsing equality expression\n");
    parse_relational_expression();

    while (current_token->type == T_EQUAL || current_token->type == T_NOT_EQUAL)
    {
        match(current_token->type);
        parse_relational_expression();
    }
}

void parse_relational_expression()
{
    if (verbose == 1)
        printf("              Parsing relational expression\n");
    parse_additive_expression();

    while (current_token->type == T_LESS || current_token->type == T_GREATER ||
           current_token->type == T_LESS_EQUAL || current_token->type == T_GREATER_EQUAL)
    {
        match(current_token->type);
        parse_additive_expression();
    }
}

void parse_additive_expression()
{
    if (verbose == 1)
        printf("                Parsing additive expression\n");
    parse_multiplicative_expression();

    while (current_token->type == T_ADD || current_token->type == T_SUB)
    {
        match(current_token->type);
        parse_multiplicative_expression();
    }
}

void parse_multiplicative_expression()
{
    if (verbose == 1)
        printf("                Parsing multiplicative expression\n");
    parse_unary_expression();

    while (current_token->type == T_MULT || current_token->type == T_DIV)
    {
        match(current_token->type);
        parse_unary_expression();
    }
}

void parse_unary_expression()
{
    if (verbose == 1)
        printf("                  Parsing unary expression\n");
    if (current_token->type == T_NOT || current_token->type == T_SUB)
    {
        match(current_token->type);
        parse_unary_expression();
    }
    else
    {
        parse_postfix_expression();
    }
}

void parse_postfix_expression()
{
    if (verbose == 1)
        printf("                  Parsing postfix expression\n");
    parse_primary_expression();

    while (1)
    {
        if (current_token->type == T_LEFT_BRACKET)
        {
            // array access: exprPostfix [ expr ]
            match(T_LEFT_BRACKET);
            parse_expression();
            match(T_RIGHT_BRACKET);
        }
        else if (current_token->type == T_LEFT_PAR)
        {
            // function call: exprPostfix ( args )
            match(T_LEFT_PAR);
            if (current_token->type != T_RIGHT_PAR)
            {
                parse_expression();
                while (current_token->type == T_COMMA)
                {
                    match(T_COMMA);
                    parse_expression();
                }
            }
            match(T_RIGHT_PAR);
        }
        else if (current_token->type == T_DOT)
        {
            // struct member access: exprPostfix . ID
            match(T_DOT);
            match(T_IDENTIFIER);
        }
        else
        {
            break;
        }
    }
}

void parse_primary_expression()
{
    if (verbose == 1)
        printf("                    Parsing primary expression\n");
    skip_whitespace_and_comments();

    switch (current_token->type)
    {
    case T_IDENTIFIER:
        match(T_IDENTIFIER);
        break;
    case T_CONSTANT_INT:
    case T_CONSTANT_REAL:
    case T_CONSTANT_CHAR:
    case T_CONSTANT_STRING:
        match(current_token->type);
        break;
    case T_LEFT_PAR:
        match(T_LEFT_PAR);
        parse_expression();
        match(T_RIGHT_PAR);
        break;
    default:
        printf("Syntax error: Expected primary expression but found %s\n", token_type_to_string(current_token->type));
        exit(1);
    }
}

int syntactic_main()
{
    tokenize("9.c");
    current_token_index = 0;
    current_token = &token_list[current_token_index]; // current_token is a pointer that always points to the current token being processed by the parser
    parse_program();

    return 0;
}
