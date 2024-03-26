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

Parser::Parser(TokenAgency tokenagency) {
	this->tokenagency = tokenagency;
	this->length = tokenagency.tokenstream.size();
    this->tokens = tokenagency.array();
    this->pos = 0;
    this->lookahead = 0;

    this->ast = ASTNode();
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
	return ASTNode(ast_terminal, get_token(lookahead-i));
}

ParseResult Parser::parse_operation() {
	/*
		OPERATION ::= GENERIC_VALUE (OPERATOR GENERIC_VALUE)*
	*/
	save();
	
	ParseResult r;
	if((r=match_generic_value()).success) {
		ASTNode tree = ASTNode(ast_operation);
		tree.append(r.ast);
		while((r=match_operator()).success) { 
			tree.append(r.ast);
			if(!(r=match_generic_value()).success) {
				ParseResult result = ParseResult(false, "expression expected following operator", lookahead-pos, lookahead);
				restore();
				return result;
			}
			tree.append(r.ast);
		}
		unsave();
		return ParseResult(true, "operation", 0, lookahead, tree);
	}
	ParseResult result = ParseResult(false, "expected expression (operation)", lookahead-pos, lookahead);
	restore();
	return result;
}

ParseResult Parser::parse_func_call() {
	/*
		FUNC_CALL ::= tok_name tok_open_rbracket FUNC_CALL_ARGS tok_close_rbracket
	*/
	save();
	
	ASTNode tree = ASTNode(ast_func_call);
	if( match(tok_name) && 
		match(tok_open_rbracket)) {
		tree.append(ast_tok(2));
		tree.append(ast_tok(1));
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
			tree.append(ast_tok());
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
		tree.append(ast_tok());
		ParseResult r = parse_expression();
		if(r.success)
			tree.append(r.ast);
			if(match(tok_colon)) {
				unsave();
				tree.append(ast_tok());
				return ParseResult(true, "conditional", 0, lookahead, tree);
			}
	}
	ParseResult result = ParseResult(false, "invalid if-statement", lookahead-pos, lookahead);
	restore();
	return result;
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
		tree.append(ast_tok(3));	// add tok_func
		tree.append(ast_tok(2));	// add tok_name
		tree.append(ast_tok(1));	// add tok_open_rbracket
		ParseResult r = parse_func_definition_args().success;
		if(!r.success) {
			restore();
			return r;
		}
		tree.append(r.ast);			// add func_definition_args
		if(match(tok_close_rbracket) && match(tok_colon)) {
			tree.append(ast_tok(2));	// add tok_close_rbracket
			tree.append(ast_tok(1));	// add tok_colon
			unsave();
			return ParseResult(true, "function Definition", 0, lookahead, tree);
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
			tree.append(ast_tok(2));
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
	
	if(match(tok_name) && match(tok_equals)) {
		tree.append(ast_tok(2));
		tree.append(ast_tok(1));

		ParseResult r = parse_expression();
		if(!r.success) {
			restore();
			return r;
		}
		tree.append(r.ast);

		unsave();
		return ParseResult(true, "variable definition", 0, lookahead, tree);
	}

	restore();
	return ParseResult(false, "invalid variable definition", lookahead-pos, lookahead);
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
	ParseResult results[4] = {parse_conditional(), parse_func_definition(), parse_func_call(), parse_variable_definition()};
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
	ParseResult results[] = {parse_func_call(), parse_operation()};
	// get the result wich is either true or has higher gravity
	ParseResult result = ParseResult(false, "parser failed (expression)", 0, lookahead);
	for(ParseResult r: results) {
		if(r.success) return r;
		if(r.gravity > result.gravity) result = r;
	}
 	return result;
}

ParseResult Parser::parse_line() {
	lookahead = pos;

	// get indent of line
	int indent = 0;
	while(match(tok_tab)) indent++;

	// parse line
	ParseResult r = parse_statement();
	if(!r.success) 
		return r;
	std::cout << r << "\n";
	if(!match(tok_newline)) 
		return ParseResult(false, "missing newline at end of statement", lookahead-pos, lookahead);
	return r;
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
	while(this->pos<this->length && (r=parse_line()).success) {
		pos = lookahead;
		std::cout << r << "\n";
		this->ast.append(r.ast);
	}

	if(!r.success) {
		std::cout << r << "\n";
		std::cout << errorhandler.construct_error(r.description, r.pos, r.pos) << "\n";
		return false;
	}
	return true;
}

bool Parser::semantic_analysis() {
	std::cout << "Testing AST";
	ASTNode tree = ASTNode();
	tree.append(ASTNode(ast_statement));
	tree.append(ASTNode(ast_expression));
	tree[0].append(ASTNode(ast_operation));
	Token t = Token("2,2,test,0,\"teststr\"");
	tree[0][0].append(ASTNode(ast_terminal,t));
	std::cout << "Printing AST to stdout\n";
	std::cout << tree.to_string();
	std::cout << "Removeing node from AST\n";
	tree[0].pop();
	std::cout << tree.to_string();

	std::cout << "Printing constructed AST\n";
	std::cout << ast.to_string();
	std::cout << "Semantic Analysis NOT implemented\n";
	return false;
}