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
#include <any>
#include <map>

#include "parser.h"
#include "color.h"

Parser::Parser(TokenAgency tokenagency) {
	this->tokenagency = tokenagency;
	this->length = tokenagency.tokenstream.size();
    this->tokens = tokenagency.array();
    this->pos = 0;
    this->lookahead = 0;

    this->ast = ASTNode(ast_block);
    this->current_node = this->ast;
    
    this->errorhandler = ErrorHandler(tokenagency);
}


void Parser::save() {
	saved_lookahead.push(lookahead);
}

void Parser::restore() {
	int top = saved_lookahead.top();
	saved_lookahead.pop();
	lookahead = top;
}

int Parser:: unsave() {
	int top = saved_lookahead.top();
	saved_lookahead.pop();
	return top;
}

toktype Parser::get_tok(int pos) {
    // returns toktype at position if existent otherwise tok_eof
    if(pos >= length) return tok_eof;
    return tokens[pos];
}

Token Parser::get_token(int pos) {
	// returns pointer to Token at position if existent otherwise Token with eof toktype
	if(pos >= length) return Token(tok_eof);
	return tokenagency.get_token(pos);
}

bool Parser::match(toktype* tok, int length) {
  	// matches the next few lookahead tokens against the provided tokens
  	save();
  	for(int i = 0; i < length; i++) {
      	if(!match(tok[i])) {
          	restore();
          	return false;
      	}
  	}
  	unsave();
  	return true;
}

bool Parser::match(toktype tok) {
  	// matches the next lookahead token against the provided token
  	if(get_tok(lookahead) == tok) {
      	lookahead++;
      	return true;
	}
	return false;
}

ASTNode Parser::ast_tok() {
	// retrieves token at lookahead-1 and returns an ASTNode obj representing the terminal
	return ast_tok(1);
}
ASTNode Parser::ast_tok(int i) {
	// retrieves token at lookahead-i and returns an ASTNode obj representing the terminal
	Token t = get_token(lookahead-i);
	// first check which type of ast_type should be assigned to node
	ast_type n_type;
	switch(t.type) {
		case tok_name:
			n_type = ast_identifier;
			break;
		case tok_plus:
		case tok_minus:
		case tok_mod:
		case tok_div:
		case tok_mul:
			n_type = ast_operator;
			break;
		case tok_ret:
			n_type = ast_ret;
			break;
		case tok_num:
		case tok_str:
			n_type = ast_value;
			break;
		default:
			n_type = ast_terminal;
			break;
	}
	return ASTNode(n_type, t);
}

ParseResult Parser::parse_operation() {
	/*
		OPERATION ::= GENERIC_VALUE (OPERATOR GENERIC_VALUE)*

		ASTNode::children = {<operator>, <expr1>, <expr2>}
	*/
	save();

	ParseResult r;
	ASTNode tree = ASTNode(ast_operation);
	
	// <value>
	if((r=match_generic_value()).success) {
		tree.append(r.ast);
		unsave();
		return ParseResult(true, "operation", 0, lookahead, tree);
	}

	// <operator> <expr1> <expr2>
	if((r=match_operator()).success) {
		tree.append(r.ast);
		if(!(r=parse_expression()).success) { 
			ParseResult result = ParseResult(false, "expression expected following operator", lookahead-pos, lookahead);
			restore();
			return result;
		}
		tree.append(r.ast);
		if(!(r=parse_expression()).success) { 
			ParseResult result = ParseResult(false, "expressionexpected following expression", lookahead-pos, lookahead);
			restore();
			return result;
		}
		tree.append(r.ast);
		unsave();
		return ParseResult(true, "operation", 0, lookahead, tree);
	}
	ParseResult result = ParseResult(false, "expected expression (operation)", lookahead-pos, lookahead);
	restore();
	return result;
}

ParseResult Parser::parse_pointer_expr() {
	/*
		POINTER_EXPR ::= TOK_MUL TOK_NAME

		ASTNode::children = {<ast_name>}
	*/
	save();
	
	ParseResult r;
	if(match(tok_mul)) {
		ASTNode tree = ASTNode(ast_pointer_expr);
		if(match(tok_name)) {
			tree.append(ast_tok());
			unsave();
			return ParseResult(true, "pointer expression", 0, lookahead, tree);
		}
	}
	ParseResult result = ParseResult(false, "incomplete pointer expression", lookahead-pos, lookahead);
	restore();
	return result;
}

ParseResult Parser::parse_func_call() {
	/*
		FUNC_CALL ::= tok_name tok_open_rbracket FUNC_CALL_ARGS tok_close_rbracket

		ASTNode::children = {<tok_name> <func_args>}
	*/
	save();
	
	ASTNode tree = ASTNode(ast_func_call);
	if( match(tok_name) && 
		match(tok_open_rbracket)) {
		tree.append(ast_tok(2));
		ParseResult r = parse_func_call_args();
		if(!r.success) {
			restore();
			return r;
		}
		if(match(tok_close_rbracket)) {
			tree.append(r.ast);
			unsave();
			return ParseResult(true, "function Call", 0, lookahead, tree);
		}
	}
	
	ParseResult result = ParseResult(false, "invalid function call", lookahead-pos, lookahead);
	restore();
	return result;
}

ParseResult Parser::parse_func_call_args() {
	/*
		FUNC_CALL ::= tok_name tok_open_rbracket FUNC_CALL_ARGS tok_close_rbracket
	*/
	save();

	ASTNode tree = ASTNode(ast_func_call_args);

	ParseResult r;
	if((r=parse_expression()).success) {
		tree.append(r.ast);
		while(match(tok_comma)) {
			if(!(r=parse_expression()).success) {
				ParseResult result = ParseResult(false, "stray comma", lookahead-pos, lookahead-1);
				restore();
				return result;
			}
			tree.append(r.ast);
		}
	}

	unsave();
	return ParseResult(true, "function call arguments", 0, lookahead, tree);
}

ParseResult Parser::match_generic_value() {
	/*
		GENERIC_VALUE ::= tok_name | tok_num
	*/
	if(match(tok_name) || match(tok_num)) return ParseResult(true, "generic value", ast_tok());
	return ParseResult(false, "no generic value", 0, lookahead);
}

ParseResult Parser::match_operator() {
	/*
		OPERATOR ::= tok_plus | tok_minus | tok_mul | tok_div | tok_mod
	*/
	if(match(tok_plus) || match(tok_minus) || match(tok_mul) || match(tok_div) || match(tok_mod)) return ParseResult(true, "operator", ast_tok());
	return ParseResult(false, "no opeartor", 0, lookahead);
}

ParseResult Parser::parse_conditional() {
	/*
		CONDITIONAL ::= tok_if EXPRESSION tok_colon
	*/
	save();
	
	if(match(tok_if))
	{
		ASTNode tree = ASTNode(ast_conditional);
		ParseResult r = parse_expression();
		if(!r.success) {
			restore();
			return r;
		}
		tree.append(r.ast);
		if(match(tok_colon)) {
			// check following block
			if((r=parse_block()).success) {
				tree.append(r.ast);
				unsave();
				return ParseResult(true, "conditional", 0, lookahead, tree);
			} else {
				restore();
				return r;
			}
		}
	}
	ParseResult result = ParseResult(false, "invalid if-statement", lookahead-pos, lookahead);
	restore();
	return result;
}

ParseResult Parser::parse_ret() {
	save();

	ParseResult r;
	if(match(tok_ret)) {
		ASTNode tree = ast_tok();
		if(!(r=parse_expression()).success) {
			restore();
			return r;
		}
		tree.append(r.ast);
		return ParseResult(true, "return statement", 0, lookahead, tree);
	}
	return ParseResult(false, "invalid return statement");
}

ParseResult Parser::parse_func_definition() {
	/*
		FUNC_DEFINITION ::= tok_func tok_name tok_open_rbracket FUNC_DEF_ARGS tok_close_rbracket
	*/
	save();

	if( match(tok_func) && 
		match(tok_name) && 
		match(tok_open_rbracket)) {
		ASTNode tree = ASTNode(ast_func_definition);
		tree.append(ast_tok(2));	// add tok_name
		ParseResult r = parse_func_definition_args();
		if(!r.success) {
			restore();
			return r;
		}

		tree.append(r.ast);			// add func_definition_args
		if(match(tok_close_rbracket) && match(tok_colon)) {
			// check that there is a following block
			if((r=parse_block()).success) {
				tree.append(r.ast);
				unsave();
				return ParseResult(true, "function Definition", 0, lookahead, tree);
			} else {
				restore();
				return r;
			}
		}
	}
	ParseResult result = ParseResult(false, "invalid function definition", lookahead-pos, lookahead);
	restore();
	return result;
}	

ParseResult Parser::parse_func_definition_args() {
	/*
		FUNC_DEF_ARGS ::= tok_name (tok_comma tok_name)*
	*/
	save();
	ASTNode tree = ASTNode(ast_func_def_args);
	
	if(match(tok_name)) {
		tree.append(ast_tok());
		while(match(tok_comma)) {
			if(!match(tok_name)) {
				ParseResult result = ParseResult(false, "stray comma", lookahead-pos, lookahead-1);
				restore();
				return result;
			}
			tree.append(ast_tok(1));
		}
	}
	
	unsave();
	return ParseResult(true, "function Definition Args", 0, lookahead, tree);
}

ParseResult Parser::parse_variable_definition() {
	/*
		VARIABLE_DEFINITION ::= tok_name tok_equals EXPRESSION
	*/
	save();

	ASTNode tree = ASTNode(ast_variable_definition);
	// check whether assigning to a pointer address or to a variable
	if(match(tok_mul) && match(tok_name) && match(tok_equals)) {
		ASTNode pointer_expr_ast = ASTNode(ast_pointer_expr);
		pointer_expr_ast.append(ast_tok(2));
		tree.append(pointer_expr_ast);
	} else if(match(tok_name) && match(tok_equals)) {
		tree.append(ast_tok(2));
	} else {
		restore();
		return ParseResult(false, "invalid variable definition", lookahead-pos, lookahead);
	}

	// parse expression which is assigned to variable
	ParseResult r = parse_expression();
	if(!r.success) {
		restore();
		return r;
	}
	tree.append(r.ast);

	unsave();
	return ParseResult(true, "variable definition", 0, lookahead, tree);
}

ParseResult Parser::parse_statement() {
	/*
		STATEMENT ::= CONDITIONAL | FUNC_DEFINITION | VARIABLE_DEFINITION | FUNC_CALL
		VARIABLE_DEFINITION ::= tok_name tok_equals EXPRESSION
		CONDITIONAL ::= tok_if EXPRESSION tok_colon
		FUNC_DEFINITION ::= tok_func tok_name tok_open_rbracket FUNC_DEF_ARGS tok_close_rbracket
		FUNC_DEF_ARGS ::= tok_name (tok_comma tok_name)*
		FUNC_CALL ::= tok_name tok_open_rbracket FUNC_CALL_ARGS tok_close_rbracket
		FUNC_CALL_ARGS ::= EXPRESSION (tok_comma EXPRESSION)*
	*/
	ParseResult results[5] = {parse_conditional(), parse_func_definition(), parse_func_call(), parse_variable_definition(), parse_ret()};
	// get the result wich is either true or has higher gravity
	ParseResult result = ParseResult(false, "parser failed (statement)", 0, lookahead);
	for(ParseResult r: results) {
		if(r.success) return r;
		if(r.gravity > result.gravity) result = r;
	}
 	return result;
}

ParseResult Parser::parse_expression() {
	/*
		EXPRESSION ::= OPERATION | FUNC_CALL
		OPERATION ::= GENERIC_VALUE (OPERATOR GENERIC_VALUE)*
		OPERATOR ::= tok_plus | tok_minus | tok_mul | tok_div | tok_mod
		GENERIC_VALUE ::= tok_name | tok_num | FUNC_CALL
		FUNC_CALL ::= tok_name tok_open_rbracket FUNC_CALL_ARGS tok_close_rbracket
		FUNC_CALL_ARGS ::= EXPRESSION (tok_comma EXPRESSION)*
	*/
	ParseResult results[] = {parse_func_call(), parse_operation(), parse_pointer_expr()};
	// get the result wich is either true or has higher gravity
	ParseResult result = ParseResult(false, "parser failed (expression)", 0, lookahead);
	for(ParseResult r: results) {
		if(r.success) return r;
		if(r.gravity > result.gravity) result = r;
	}
 	return result;
}

ParseResult Parser::parse_line() {
	// parse line
	ParseResult r = parse_statement();
	if(!r.success) 
		return r;
	std::cout << r << "\n";
	if(!match(tok_newline)) {
		std::cout << "unsuccessful parsing in line [" << lookahead << "|" << pos << "]\n";
		return ParseResult(false, "missing newline at end of statement", lookahead-pos, lookahead);
	}
	return r;
}

ParseResult Parser::parse_block() {
	save();

	ASTNode tree = ASTNode(ast_block);
	int block_indent = this->indent+1;
	
	while(true) {
		// eat up newlines and comments
		while(match(tok_newline) || match(tok_comment));
		// get indent of line
		int indent = 0;
		while(match(tok_tab)) indent++;

		if(indent > block_indent) {
			// indent is higher thant expected (invalid)
			restore();
			return ParseResult(false, "wrong indent (should be " + std::to_string(block_indent) + " but is " + std::to_string(indent) + ")", lookahead-pos, lookahead);
		} else if(indent < block_indent) {
			// block ends
			unsave();
			lookahead--;
			return ParseResult(true, "block", 0, lookahead, tree);
		}

		// parse the next instruction and add to block
		ParseResult r = parse_line();
		if(!r.success) {
			restore();
			return r;
		}
		tree.append(r.ast);
	}
}

bool Parser::syntactic_analysis() {
	// check for lexer errors
	bool lex_ok = true;
	for(int i = 0; i < this->length; i++) {
		Token t = this->tokenagency.get_token(i); 
		if(t.type==tok_err) {
			std::cout << errorhandler.construct_error(t.str, i, i) << "\n";
			lex_ok = false;
		}
	}
	if(!lex_ok) return false;

	ParseResult r = ParseResult(false, "parser failed (all)");
	while(this->pos<this->length) {
		// eat up newlines and comments
		while(match(tok_newline) || match(tok_comment));
		// get indent of line
		this->indent = 0;
		while(match(tok_tab)) this->indent++;

		if(this->indent != 0) {
			std::cout << "la " << lookahead << " pos " << pos << "\n";
			std::cout << errorhandler.construct_error("wrong indent (should be 0 but is " + std::to_string(indent) + ")", lookahead-1, lookahead-1) << "\n";
			return false;
		}

		if((r=parse_line()).success) {
			pos = lookahead;
			std::cout << r << "\n";
			this->ast.append(r.ast);
		} else {
			std::cout << r << "\n";
			std::cout << errorhandler.construct_error(r.description, r.pos, r.pos) << "\n";
			return false;	
		}
	}
	return true;
}

ParseResult Parser::parse_node(ASTNode node, Scope* scope) {
	// since syntax is already parsed, we only need to check semantics (undefined variables/functions)
	ParseResult r;
	int argc; std::string identifier;
	std::cout << (*scope).to_string() << "\n";
	switch(node.type) {
		case ast_undefined:
			return ParseResult(false, "invalid ast node (undefined)");
		
		case ast_variable_definition:
			if(node[0].type == ast_pointer_expr) {
				// if identifier in pointer expr doesn't exist, that is kinda bad
				r = parse_node(node[0][0], scope);
				if(!r.success || !(r=parse_node(node[1], scope)).success)
					return r;
			}
			else {
				identifier = node[0].token.raw;
				if(!(r=parse_node(node[1], scope)).success)
					return r;
				(*scope).add_var(identifier);
			}
			return ParseResult(true);
		
		case ast_identifier:
			identifier = node.token.raw;
			if(!(*scope).var_in_scope(identifier))
				return ParseResult(false, "Variable " + identifier + " undefined", 1, tokenagency.get_token_pos(node.token));
			return ParseResult(true);
		
		case ast_func_call:
			// get amount of args and identifier
			argc = node[1].childcount();
			identifier = node[0].token.raw;
			if(!(*scope).func_in_scope(identifier, argc)) {
				// get string representation of args
				std::string args;
				for(int i = 0; i < node[1].childcount(); i++) args += "arg" + std::to_string(i) + ", ";
				args = args.substr(0, args.length()-2);
				// return error (undefined function)
				return ParseResult(false, "Call to undefined function " + identifier + "(" + args + ")", 1, tokenagency.get_token_pos(node[0].token));
			}
			// parse args in func call
			if(!(r=parse_node(node[1], scope)).success) return r;
			return ParseResult(true);
		
		case ast_func_definition:
			// get amount of args and identifier
			argc = node[1].childcount();
			identifier = node[0].token.raw;
			// check if function signature already exists
			if((*scope).func_in_scope(identifier, argc)) {
				// get string representation of args
				std::string args;
				for(ASTNode child: node[1].children) args += child.token.raw + ", ";
				args = args.substr(0, args.length()-2);
				// return error (previously defined function)
				return ParseResult(false, "Function " + identifier + " (argc=" + std::to_string(argc) + ") already exists", 1, tokenagency.get_token_pos(node[0].token));
			}
			// add func args to scope
			(*scope).push();
			for(ASTNode child: node[1].children) (*scope).add_var(child.token.raw);
			// parse block
			if(!(r=parse_node(node[2], scope)).success) return r;
			(*scope).pop();
			// add func signature to scope
			(*scope).add_func(identifier, argc);
			return ParseResult(true);

		case ast_func_def_args:
			// special case since func_def_args will contain undefined variables (duh!)
			return ParseResult(true);

		case ast_block:
			// parse every child node
			(*scope).push();
			for(ASTNode child: node.children) {
				if(!(r=parse_node(child, scope)).success) return r;
			}
			(*scope).pop();
			return ParseResult(true);

		default:
			// parse every child node but do NOT ADD an extra sub-scope
			for(ASTNode child: node.children) {
				if(!(r=parse_node(child, scope)).success) return r;
			}
			return ParseResult(true);
	}
	return ParseResult(false, "invalid ast node (out of bounds)");
}

ParseResult Parser::parse_static_expression(ASTNode node) {
	if(node.type == ast_func_call) {
			return ParseResult(false, "cannot assign dynamic value to global variable", 1, tokenagency.get_token_pos(node[0].token));
	}
	else {
		ParseResult r;
		for(ASTNode _node: node.children) {
			if(!(r=parse_static_expression(_node)).success) return r;
		}
		return ParseResult(true, "valid static expression");
	}
}

ParseResult Parser::parse_globals() {
	/*
		- parses area of global write/read (i.e. where globla variables are assigned)
		- parses assignment of global variables (static expressions) which cannot
		  be assigned dynamic values (values that need to be computed at runtime, i.e. function calls)
	*/
	for(ASTNode node: this->ast.children) {
		if(node.type == ast_variable_definition) {
			ParseResult r = parse_static_expression(node[1]);
			if(!r.success) return r;
		} else if(node.type == ast_ret) return ParseResult(false, "return statement outside of function", 1, tokenagency.get_token_pos(node.token));
	}
	return ParseResult(true, "valid global variable definition");
}

bool Parser::semantic_analysis() {
	std::cout << Color::bhyellow << "ast constructed from src\n" << Color::reset;
	std::cout << ast.to_string();

	ParseResult r = this->parse_globals();
	if(!r.success) {
		std::cout << errorhandler.construct_error(r.description, r.pos, r.pos) << "\n";
		return false;
	}

	Scope scope = Scope();
	// go through AST in preorder and check each node for validity
	r = parse_node(this->ast, &scope);
	
	// check result
	if(r.success) return true;

	std::cout << errorhandler.construct_error(r.description, r.pos, r.pos) << "\n";
	return false;
}