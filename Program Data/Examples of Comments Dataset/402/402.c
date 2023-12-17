/*
This function is responsible for initializing a JSON lexer, setting its initial state, and allocating resources for token storage.

Here's the detail:
- The function takes a pointer to a JSONLexer structure (lexer) and a JSONLexerEmitter function pointer (func) as input parameters.
- It assigns the JSONLexerEmitter function pointer (func) to the emit field of the JSONLexer structure (lexer).
- It sets the initial state of the lexer to IN_START, indicating the starting state of the lexer's state machine.
- The function allocates a new qstring (presumably a dynamic string) using qstring_new and assigns it to the token field of the lexer, providing storage for the lexer to build tokens during the lexing process.
- It initializes the x and y coordinates of the lexer to 0, possibly indicating the starting position in the input source.

Needs to notice:
- The function assumes that the input parameters are valid and does not perform parameter validation checks. It should be used with caution to ensure that the provided parameters are valid and within the expected range.
- The allocation of resources, such as memory for the token field, implies that proper resource management and deallocation should be considered when the JSONLexer is no longer in use to prevent memory leaks.
- The specific details of the JSONLexer structure, the qstring type, and the JSONLexerEmitter function type are not provided in the given code snippet, and their behavior and usage should be understood from the broader context of the codebase.
*/
void json_lexer_init(JSONLexer *lexer, JSONLexerEmitter func)

{

    lexer->emit = func;

    lexer->state = IN_START;

    lexer->token = qstring_new();

    lexer->x = lexer->y = 0;

}
