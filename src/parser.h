#include <iomanip>
#include <iostream>
#include <cassert>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <string_view>
#include <sstream>
#include <list>
#include <array>
#include <stack>

#include "parseresult.h"
#include "errorhandler.h"
#include "scope.h"

class Parser {
	private:
		// current pos in tokenstream
		int pos;
		// lookahead position
		int lookahead;
		// indent of the current line
		int indent;
		// saved lookaheads for restoring them if anything fails to match
		std::stack<int> saved_lookahead;
		// length of array of tokens
		int length;
		// array of tokens
		toktype* tokens;
		// the current ast node that is being parsed (semantics)
		ASTNode current_node;
		// token manager
		TokenAgency tokenagency;
		// error handler
		ErrorHandler errorhandler;

		// save the current lookahead and allow restoring it later on
		void save();
		// restore the lookahead to the last saved value
		void restore();
		// remove the last saved lookahead value (returns the value)
		int unsave();

		// returns toktype at position if existent otherwise tok_eof
        toktype get_tok(int pos);
        // returns pointer to Token at position if existent otherwise Token with toktype eof
        Token get_token(int pos);

		// match a token at position (linepos|lookahead)
		bool match(toktype tok);
		// match an array of tokens at position (linepos|lookahead),(linepos|lookahead+1)...
		bool match(toktype* tok, int length);

		// retrieves token at lookahead-i and returns an ASTNode obj representing the terminal
		ASTNode ast_tok(int i);
		// retrieves token at lookahead-1 and returns an ASTNode obj representing the terminal
		ASTNode ast_tok();
	
		// parse a single line/instruction
		ParseResult parse_line();
		// parse a single statement
		ParseResult parse_statement();
		// parse a single expression
		ParseResult parse_expression();

			// parsing an entire block
			ParseResult parse_block();

			// matching functions for expressions
			ParseResult parse_operation();
			ParseResult parse_func_call();
			ParseResult parse_func_call_args();
			ParseResult parse_pointer_expr();
			ParseResult match_generic_value();
			ParseResult match_operator();

			// matching functions for statements
			ParseResult parse_conditional();
			ParseResult parse_variable_definition();
			ParseResult parse_func_definition();
			ParseResult parse_func_definition_args();
			ParseResult parse_ret();

		// semantic analysis of a node in the ast tree
		ParseResult parse_node(ASTNode node, Scope* scope);
		ParseResult parse_static_expression(ASTNode node);
		ParseResult parse_globals();
		
	public:
		// AST
		ASTNode ast;
		
		Parser(TokenAgency tokenagency);
		// parses tokenstream for syntax (checks for lexer errors first)
		// if successful, the Parser->ast variable should be populated
		bool syntactic_analysis();
		// parses the ast and checks for semantic errors (and indents)
		bool semantic_analysis();
};
