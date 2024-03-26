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

#include "astnode.h"

class ParseResult {
	public:
		// whether parsing was successful
		bool success;
		// if error, short description of the error (e.g. "Invalid Syntax", or "missing colon ':' ")
		// if success, short description of what was actually parsed (e.g. "if-statement")
		std::string description;
		// if success, construct ast tree representing parsed section
		ASTNode ast;
		// if error, token position at which error occured
		int pos;
		// if error, how narrowly it resembles an actual instruction (e.g. "a = b+c!!" would have a high gravity, since)
		int gravity;

		ParseResult();
		ParseResult(bool success);
		ParseResult(bool success, std::string description);
		ParseResult(bool success, std::string description, ASTNode ast);
		ParseResult(bool success, std::string description, int gravity, int pos);
		ParseResult(bool success, std::string description, int gravity, int pos, ASTNode ast);
		// returns printable string for stdout
		std::string to_string();
		// override of << operator
		friend std::ostream& operator<<(std::ostream &os,  const ParseResult &result);
};
