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

#include "color.h"
#include "tokenagency.h"
#include "errorhandler.h"

ErrorHandler::ErrorHandler() {
    
}

ErrorHandler::ErrorHandler(TokenAgency tokenagency) {
	this->tokenagency = tokenagency;
}

std::string ErrorHandler::reconstruct_source(int start, int end, std::string color) {
	// get first token that is on line of start token
	// and last token that is on line ofend token
    int rstart = start;
    int rend = end;
    while(tokenagency.get_token(--rstart).lineno == tokenagency.get_token(start).lineno && tokenagency.get_token(rstart).type != tok_eof);
    while(tokenagency.get_token(++rend).lineno == tokenagency.get_token(end).lineno && tokenagency.get_token(rend).type != tok_eof);
    int current_lineno = tokenagency.get_token(rstart).lineno;

    // construct string
    std::string s = "";
    int rowno = 0;
    for(int i = rstart; i < rend; i++) {
        Token t = tokenagency.get_token(i);
        rowno++;
       	if(t.lineno > current_lineno) {
            while(current_lineno < t.lineno) {
            	rowno = 0;
                std::stringstream lineno_format;
                lineno_format << std::setw(3) << std::setfill(' ') << ++current_lineno;
                s += Color::hwhite + "\n " + lineno_format.str() + Color::reset + " | ";
            }
        }

        // mark with color specified
        if(i <= end && i >= start) s+=color;

        switch(t.type) {
            case tok_newline: 
            	// if the current lookahead is stuck on a newline the instruction is missing something at the end
                if(i <= end && i >= start) s += Color::redhb + "_" + Color::reset + " ";
                break;
            case tok_tab: 
                s += Color::hblack + "___" + Color::reset;
                break;
            case tok_open_rbracket:
            case tok_close_rbracket:
            case tok_name:
            case tok_colon:
            case tok_plus:
            case tok_minus:
            case tok_div:
            case tok_mod:
            case tok_mul:
            case tok_num:
            	s += t.raw;
            	break;
            case tok_comma:
            	s += ",";
            	break;
            case tok_equals:
            	s += " " + t.raw + " ";
            	break;
            default: 
                s += t.raw + " ";
                break;
        }

        // remove any colors
        s += Color::reset;
    }
    return s;
}

std::string ErrorHandler::construct_error(std::string description, int start, int end) {
	return stringify_error(description, reconstruct_source(start, end, Color::bhred), tokenagency.get_token(start).lineno, tokenagency.get_token(end).lineno);
}

std::string ErrorHandler::stringify_error(std::string description, std::string body, int linestart, int lineend) {
	return ErrorHandler::stringify_error(description, body, linestart, lineend, 0);
}

std::string ErrorHandler::stringify_error(std::string description, std::string body, int linestart, int lineend, int errortype) {
	std::string s = Color::bhblue + "santa" + Color::reset + ":";
	s += Color::bhblue + std::to_string(linestart) + Color::reset + ":" + Color::bhblue + std::to_string(lineend) + Color::reset + ": ";
	switch(errortype) {
		case 0:
			s += Color::bhred + "error";
			break;
		case 1:
			s += Color::byellow + "warning";
			break;
	}
	s += Color::reset + ": " + Color::bhblue + description + Color::reset;
	s += body;
	return s;
}