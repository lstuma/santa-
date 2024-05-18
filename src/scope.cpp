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

#include "scope.h"
#include "color.h"

std::string Scope::to_string() {
	std::string s =  "{";
	int sub = 0;
	// variables
	for(auto l: this->var_scope) {
		for(auto identifier: l) {
			std::stringstream num_format; std::stringstream sub_format;
			sub_format << std::setw(2) << std::setfill(' ') << sub;
			s += Color::bhcyan + sub_format.str() + Color::reset + ":(" + Color::bhblue + identifier + Color::reset + "), ";
		}
		sub++;
	}
	// functions
	sub = 0;
	for(auto l: this->func_scope) {
		for(FuncSignature signature: l) {
			std::stringstream num_format; std::stringstream sub_format;
			sub_format << std::setw(2) << std::setfill(' ') << sub;
			s += Color::bhmagenta + sub_format.str() + Color::reset + ":" + signature.to_string() + Color::reset + ", ";
		}
		sub++;
	}
	s += "}";
	return s;
}

bool Scope::var_in_scope(std::string identifier) {
	for(std::list<std::string> l: this->var_scope)
		for(std::string scope_identifier: l)
			if(identifier == scope_identifier) return true;
	return false;
}
bool Scope::func_in_scope(std::string identifier, int argc) {
	for(std::list<FuncSignature> l: this->func_scope)		
		for(FuncSignature signature: l) {
			if(identifier == signature.identifier && argc == signature.argc) return true;
		}
	return false;
}

void Scope::add_var(std::string identifier) {
	this->var_scope.back().push_back(identifier);
}
void Scope::add_func(std::string identifier, int argc) {
	FuncSignature signature = FuncSignature(identifier, argc);
	this->func_scope.back().push_back(signature);
}

void Scope::pop() {
	this->var_scope.pop_back();
	this->func_scope.pop_back();
}

void Scope::push() {
	this->var_scope.push_back(std::list<std::string>());
	this->func_scope.push_back(std::list<FuncSignature>());
}

Scope::Scope() {
	this->var_scope = std::list<std::list<std::string>>();
	this->func_scope = std::list<std::list<FuncSignature>>();
}

FuncSignature::FuncSignature(std::string identifier, int argc) {
	this->identifier  = identifier;
	this->argc = argc;
}

FuncSignature::FuncSignature() {
	this->identifier = "undefined";
	this->argc = 0;
}

std::string FuncSignature::to_string() {
	// stringify
	std::string s = "(" + Color::bhblue + this->identifier + Color::reset + ", argc=" + Color::bhblue + std::to_string(this->argc) + Color::reset + ")";
	return s;
}