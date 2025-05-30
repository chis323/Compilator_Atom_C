#include "lexical_analyzer.h"
int main()
{
    tokenize("5.c");
    for (int i = 0; i < token_list_index; i++)
    {
        Token token = token_list[i];
        printf("Token: %s       Value: %s\n", token_type_to_string(token.type), token.value.string_value);
    }
    
    syntactic_main();
    printf("----------------------SYNTACTIC COMPLETE------------------------\n\n");

    printSymbolTable();

    return 0;
}