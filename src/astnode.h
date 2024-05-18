#ifndef astnode_h
#define astnode_h

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

#include "tokenagency.h"

/*
	// at beginning n starts at 0
	INSTRUCTION ::=  tok_tab{n} STATEMENT tok_newline

	EXPRESSION ::= OPERATION | FUNC_CALL
	OPERATION ::= GENERIC_VALUE (OPERATOR GENERIC_VALUE)*
	OPERATOR ::= tok_plus | tok_minus | tok_mul | tok_div | tok_mod
	GENERIC_VALUE ::= tok_name | tok_num | FUNC_CALL
	FUNC_CALL ::= tok_name tok_open_rbracket FUNC_CALL_ARGS tok_close_rbracket
	FUNC_CALL_ARGS ::= EXPRESSION (tok_comma EXPRESSION)*
	
	STATEMENT ::= CONDITIONAL | FUNC_DEFINITION | VARIABLE_DEFINITION | FUNC_CALL
	VARIABLE_DEFINITION ::= tok_name tok_equals EXPRESSION
	CONDITIONAL ::= tok_if EXPRESSION tok_colon tok_newline BLOCK
	FUNC_DEFINITION ::= tok_func tok_name tok_open_rbracket FUNC_DEF_ARGS tok_close_rbracket tok_colon tok_newline BLOCK
	FUNC_DEF_ARGS ::= tok_name (tok_comma tok_name)*

	BLOCK ::= (tok_tab{n+1} STATEMENT tok_newline)* tok_tab{n+1} STATEMENT
*/

enum ast_type {
	ast_undefined = 0,

	ast_statement = 1,
	ast_expression = 2,

	ast_operation = 3,
	ast_operator = 4,
	ast_generic_value = 5,
	ast_func_call = 6,
	ast_func_call_args = 7,
	ast_pointer_expr = 16,

	ast_variable_definition = 8,
	ast_conditional = 9,
	ast_func_definition = 10,
	ast_func_def_args = 11,

	ast_terminal = 12,
	ast_identifier = 14,
	ast_value = 15,
	ast_ret = 17,

	ast_block = 13,
};

class ASTNode {
  public:
    // type of node
    ast_type type = ast_undefined;
    // (optional) token that corresponds to the node
    Token token;
    // parent node
    ASTNode* parent = nullptr;
    // childreen of the node
    std::list<ASTNode> children;

    ASTNode();
    ASTNode(ast_type type);
    ASTNode(ast_type type, Token token);
    ASTNode(ast_type type, Token token, ASTNode* parent);
    ASTNode(ast_type type, Token token, std::list<ASTNode> children);
    
    // get child at position i
  	ASTNode get(int i);
    // overrides for node[i] operator to allow to get child at pos i using [] operator
    ASTNode& operator[](std::size_t i);
    // returns length of children list
    int childcount();
    // add child
    ASTNode append(ASTNode child);
    // insert child at index i
    ASTNode insert(int i, ASTNode child);
    // remove child from end
		ASTNode pop();
		// remove child at index i
		ASTNode pop(int i);

		// string representation of ast tree
		std::string to_string();
		std::string to_string(int depth);
};

#endif