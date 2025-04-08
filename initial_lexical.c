#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexical_analyzer.h"

// Global variables for the parser
int current_token_index = 0;
Token *current_token;

// Function prototypes
void advance();
void match(TokenType expected_type);
void skip_whitespace_and_comments();
void parse_program();
void parse_declaration();
void parse_type_specifier();
void parse_variable_declaration();
void parse_function_declaration();
void parse_parameter_list();
void parse_compound_statement();
void parse_statement();
void parse_expression_statement();
void parse_selection_statement();
void parse_iteration_statement();
void parse_jump_statement();
void parse_expression();
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

void advance()
{
    if (current_token_index + 1 < token_list_index)
    {
        current_token_index++;
        current_token = &token_list[current_token_index];
    }
    else
    {
        current_token = NULL; // Mark end of input
    }
}

void match(TokenType expected_type)
{
    if (current_token->type == expected_type)
    {
        printf("Matched %s: %s\n",
               token_type_to_string(current_token->type),
               current_token->value.string_value);
        advance();
    }
    else
    {
        printf("Syntax error at token %d: Expected %s but found %s (%s)\n",
               current_token_index,
               token_type_to_string(expected_type),
               token_type_to_string(current_token->type),
               current_token->value.string_value);
        exit(1);
    }
}

void skip_whitespace_and_comments()
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

void parse_program()
{
    printf("==== Starting to parse program ====\n");
    skip_whitespace_and_comments();

    while (current_token->type != T_EOF)
    {
        parse_declaration();
        skip_whitespace_and_comments();
    }
    printf("==== Program parsed successfully ====\n");
}

void parse_declaration()
{
    printf("-- Parsing declaration --\n");

    // Error recovery: skip until we find a type specifier
    while (current_token->type != T_EOF &&
           !(current_token->type == T_INT ||
             current_token->type == T_CHAR ||
             current_token->type == T_VOID ||
             current_token->type == T_DOUBLE ||
             current_token->type == T_STRUCT))
    {
        printf("Skipping unexpected token: %s (%s)\n",
               token_type_to_string(current_token->type),
               current_token->value.string_value);
        advance();
        skip_whitespace_and_comments();
    }

    if (current_token->type == T_EOF)
        return;

    parse_type_specifier();
    skip_whitespace_and_comments();

    // Peek ahead to see if this is a function declaration
    Token *lookahead = &token_list[current_token_index + 1];
    while (lookahead->type == T_SPACE || lookahead->type == T_COMMENT || lookahead->type == T_LINE_COMMENT)
    {
        lookahead = &token_list[++current_token_index + 1];
    }

    if (lookahead->type == T_LEFT_PAR)
    {
        parse_function_declaration();
    }
    else
    {
        parse_variable_declaration();
    }
}

void parse_type_specifier()
{
    printf("  Parsing type specifier\n");
    switch (current_token->type)
    {
    case T_INT:
    case T_CHAR:
    case T_DOUBLE:
    case T_VOID:
    case T_STRUCT:
        match(current_token->type);
        break;
    default:
        printf("Syntax error: Expected type specifier but found %s\n",
               token_type_to_string(current_token->type));
        exit(1);
    }
}

void parse_variable_declaration()
{
    printf("  Parsing variable declaration\n");
    parse_type_specifier();
    skip_whitespace_and_comments();

    // First variable
    match(T_IDENTIFIER);

    // Array declaration
    if (current_token->type == T_LEFT_BRACKET)
    {
        match(T_LEFT_BRACKET);
        match(T_CONSTANT_INT);
        match(T_RIGHT_BRACKET);
    }

    // Initialization
    if (current_token->type == T_ASSIGN)
    {
        match(T_ASSIGN);
        parse_expression();
    }

    // Additional variables
    while (current_token->type == T_COMMA)
    {
        match(T_COMMA);
        skip_whitespace_and_comments();
        match(T_IDENTIFIER);

        if (current_token->type == T_LEFT_BRACKET)
        {
            match(T_LEFT_BRACKET);
            match(T_CONSTANT_INT);
            match(T_RIGHT_BRACKET);
        }

        if (current_token->type == T_ASSIGN)
        {
            match(T_ASSIGN);
            parse_expression();
        }
    }

    match(T_SEMICOLON);
}

void parse_function_declaration()
{
    printf("  Parsing function declaration\n");
    match(T_IDENTIFIER);
    skip_whitespace_and_comments(); // Add this line to skip any whitespace/comments
    match(T_LEFT_PAR);

    if (current_token->type != T_RIGHT_PAR)
    {
        parse_parameter_list();
    }

    match(T_RIGHT_PAR);
    parse_compound_statement();
}

void parse_parameter_list()
{
    printf("    Parsing parameter list\n");
    parse_type_specifier();
    match(T_IDENTIFIER);

    // Array parameter
    if (current_token->type == T_LEFT_BRACKET)
    {
        match(T_LEFT_BRACKET);
        match(T_RIGHT_BRACKET);
    }

    while (current_token->type == T_COMMA)
    {
        match(T_COMMA);
        parse_type_specifier();
        match(T_IDENTIFIER);

        if (current_token->type == T_LEFT_BRACKET)
        {
            match(T_LEFT_BRACKET);
            match(T_RIGHT_BRACKET);
        }
    }
}

void parse_compound_statement()
{
    printf("  Parsing compound statement\n");
    skip_whitespace_and_comments(); // Add this to skip any whitespace before '{'
    match(T_LEFT_ACCOLADE);
    skip_whitespace_and_comments();

    while (current_token->type != T_RIGHT_ACCOLADE && current_token->type != T_EOF)
    {
        parse_statement();
        skip_whitespace_and_comments();
    }

    match(T_RIGHT_ACCOLADE);
}

void parse_statement()
{
    printf("    Parsing statement\n");
    switch (current_token->type)
    {
    case T_LEFT_ACCOLADE:
        parse_compound_statement();
        break;
    case T_IF:
        parse_selection_statement();
        break;
    case T_FOR:
    case T_WHILE:
        parse_iteration_statement();
        break;
    case T_RETURN:
    case T_BREAK:
        parse_jump_statement();
        break;
    case T_SEMICOLON:
        parse_expression_statement(); // Empty statement
        break;
    case T_INT:
    case T_CHAR:
    case T_DOUBLE:
    case T_VOID:
    case T_STRUCT: // Handle variable declarations
        parse_variable_declaration();
        break;
    default:
        parse_expression_statement();
    }
}

void parse_expression_statement()
{
    printf("      Parsing expression statement\n");
    if (current_token->type != T_SEMICOLON)
    {
        parse_expression();
    }
    match(T_SEMICOLON);
}

void parse_selection_statement()
{
    printf("      Parsing selection statement\n");
    match(T_IF);
    match(T_LEFT_PAR);
    parse_expression();
    match(T_RIGHT_PAR);
    parse_statement();

    if (current_token->type == T_ELSE)
    {
        match(T_ELSE);
        parse_statement();
    }
}

void parse_iteration_statement()
{
    printf("      Parsing iteration statement\n");
    if (current_token->type == T_WHILE)
    {
        match(T_WHILE);
        match(T_LEFT_PAR);
        parse_expression();
        match(T_RIGHT_PAR);
        parse_statement();
    }
    else
    { // FOR
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
        parse_statement();
    }
}

void parse_jump_statement()
{
    printf("      Parsing jump statement\n");
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
    { // BREAK
        match(T_BREAK);
        match(T_SEMICOLON);
    }
}

void parse_expression()
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
    printf("          Parsing assignment expression\n");

    // First parse the left-hand side (could be an array access)
    parse_logical_or_expression();

    // Check for assignment operator
    if (current_token->type == T_ASSIGN)
    {
        match(T_ASSIGN);
        parse_assignment_expression(); // Parse the right-hand side
    }
}

void parse_logical_or_expression()
{
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
    printf("                  Parsing postfix expression\n");
    parse_primary_expression();

    while (1)
    {
        if (current_token->type == T_LEFT_BRACKET)
        {
            // Array access
            match(T_LEFT_BRACKET);
            parse_expression();
            match(T_RIGHT_BRACKET);

            // After array access, we might have an assignment
            if (current_token->type == T_ASSIGN)
            {
                return; // Let assignment expression handle it
            }
        }
        else if (current_token->type == T_LEFT_PAR)
        {
            // Function call
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
        else
        {
            break;
        }
    }
}

void parse_primary_expression()
{
    printf("                    Parsing primary expression\n");
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
        printf("Syntax error: Expected primary expression but found %s\n",
               token_type_to_string(current_token->type));
        exit(1);
    }
}

int main()
{
    main_secondary();
    current_token_index = 0;
    current_token = &token_list[current_token_index];
    parse_program();

    return 0;
}
