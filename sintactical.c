#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexical_analyzer.h"


int current_token_index = 0;
Token *current_token;


void match_t(TokenType expect_t)
{
    if (current_token->type == expect_t)
    {
        printf("match_ted Syntax\n");
        advance();
    }
    else
    {
        printf("Syntax error\n");
    }
}


void skip_spaces()
{
    while (current_token != NULL && (current_token->type == T_SPACE || current_token->type == T_COMMENT || current_token->type == T_LINE_COMMENT))
    {
        printf("Skip Space\n");
        advance();
    }
}


void advance()
{
    if (current_token_index + 1 < token_list_index) // inca nu am ajuns la capatul listei
    {
        current_token_index++;
        current_token = &token_list[current_token_index]; //ii dau valoare din array de pe pozitia curenta
    }
    else
    {
        current_token = NULL;
    }
}

void parse_type_specifier()
{
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
        printf("Syntax error\n");
    }
}

//###########################################

void declFunc()
{
    match_t(T_IDENTIFIER); // nume functie
    skip_spaces();
    match_t(T_LEFT_PAR); // (

    if (current_token->type != T_RIGHT_PAR)
    {
        //()parametru daca nu se inchide paranteza
    }

    match_t(T_RIGHT_PAR);  //paranteza de final
}

void declvar()
{
    parse_type_specifier(); // int
    skip_spaces();

    match_t(T_IDENTIFIER); // nume var

    // Array
    if (current_token->type == T_LEFT_BRACKET)
    {
        match_t(T_LEFT_BRACKET);     //[
        match_t(T_CONSTANT_INT);      // numar
        match_t(T_RIGHT_BRACKET);     // ]
    }

    // init
    if (current_token->type == T_ASSIGN)
    {
        match_t(T_ASSIGN);        //=
        //trec mai departe (de terminat)
    }

    while (current_token->type == T_COMMA)
    {
        match_t(T_COMMA); // ,
        skip_spaces();
        match_t(T_IDENTIFIER);    //int

        if (current_token->type == T_LEFT_BRACKET)
        {
            match_t(T_LEFT_BRACKET);      //[ numar ]
            match_t(T_CONSTANT_INT);
            match_t(T_RIGHT_BRACKET);
        }

        if (current_token->type == T_ASSIGN)        // =
        {
            match_t(T_ASSIGN);
            //trec mai departe (de terminat)
        }
    }

    match_t(T_SEMICOLON); // ;
}


int main()
{
    main_secondary();
}
