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

class ErrorHandler {
	private:
		// token manager
		TokenAgency tokenagency;
	public:
		ErrorHandler();
		ErrorHandler(TokenAgency tokenagency);
		// reconstructs a portion of the source code and marks tokens from position start to position end in the color specified
		std::string reconstruct_source(int start, int end, std::string color);
		// constructs string representation of error from description and start+end position of tokens
		std::string construct_error(std::string description, int start, int end);
		// calls stringify_error but with implicit error type of 0 (error)
		std::string stringify_error(std::string description, std::string body, int linestart, int lineend);
		// returns string representation of the error ready to be happily (ʕಡᴥಡʔ) printed to stdout (errortype -> 0:error, 1:warning, ...)
		std::string stringify_error(std::string description, std::string body, int linestart, int lineend, int errortype);
};