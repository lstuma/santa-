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
#include "parseresult.h"

ParseResult::ParseResult() {
	this->success = false;
	this->description = "undefined";
	this->gravity = 0;
	this->pos = 0;
}

ParseResult::ParseResult(bool success) {
	this->success = success;
	this->description = "undefined";
	this->gravity = 0;
	this->pos = 0;
}

ParseResult::ParseResult(bool success, std::string description) {
	this->success = success;
	this->description = description;
	this->gravity = 0;
	this->pos = 0;
}

ParseResult::ParseResult(bool success, std::string description, ASTNode ast) {
	this->success = success;
	this->description = description;
	this->gravity = 0;
	this->pos = 0;
	this->ast=ast;
}

ParseResult::ParseResult(bool success, std::string description, int gravity, int pos) {
	this->success = success;
	this->description = description;
	this->gravity = gravity;
	this->pos = pos;
}

ParseResult::ParseResult(bool success, std::string description, int gravity, int pos, ASTNode ast) {
	this->success=success;
	this->description=description;
	this->gravity=gravity;
	this->pos=pos;
	this->ast=ast;
}


std::string ParseResult::to_string() {
    return "{success:" + Color::bblue + (success?"true":"false") + Color::reset +
	", description:" + Color::bblue + description + Color::reset + 
	", gravity:" + Color::bblue + std::to_string(gravity) + Color::reset + 
	", pos:" + Color::bblue + std::to_string(pos) + Color::reset +  "}";
}

std::ostream& operator<<(std::ostream &os, const ParseResult &result) {
	ParseResult p = result;
	return os << p.to_string();
}
