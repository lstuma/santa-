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

class ParseResult {
	public:
		// whether parsing was successful
		bool success;
		// if error, short description of the error (e.g. "Invalid Syntax", or "missing colon ':' ")
		// if success, short description of what was actually parsed (e.g. "if-statement")
		std::string description;
		// if error, token position at which error occured
		int pos;
		// if error, how narrowly it resembles an actual instruction (e.g. "a = b+c!!" would have a high gravity, since)
		int gravity;

		ParseResult(bool success);
		ParseResult(bool success, std::string description);
		ParseResult(bool success, std::string description, int gravity, int pos);
		// returns printable string for stdout
		std::string to_string();
		// override of << operator
		friend std::ostream& operator<<(std::ostream &os,  const ParseResult &result);
};
