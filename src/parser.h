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
#include "astnode.h"
#include "errorhandler.h"

class Parser {
	private:
		// current pos in tokenstream
		int pos;
		// lookahead position
		int lookahead;
		// saved lookaheads for restoring them if anything fails to match
		std::stack<int> saved_lookahead;
		// length of array of tokens
		int length;
		// array of tokens
		toktype* tokens;
		// AST
		ASTNode ast;
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

		// parse a single line/instruction
		ParseResult parse_line();
		// parse a single statement
		ParseResult parse_statement();
		// parse a single expression
		ParseResult parse_expression();

			// matching functions for expressions
			ParseResult parse_operation();
			ParseResult parse_func_call();
			ParseResult parse_func_call_args();
			bool match_generic_value();
			bool match_operator();

			// matching functions for statements
			ParseResult parse_conditional();
			ParseResult parse_variable_definition();
			ParseResult parse_func_definition();
			ParseResult parse_func_definition_args();

	public:
		Parser(TokenAgency tokenagency);
		bool parse();
};
