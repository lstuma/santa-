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

class FuncSignature {
	public:
		std::string identifier;
		int argc;

		// string representation of scope
		std::string to_string();

		FuncSignature();
		FuncSignature(std::string identifier, int argc);
};

class Scope {
	private:
		std::list<std::list<std::string>> var_scope;
		std::list<std::list<FuncSignature>> func_scope;

	public:
		bool var_in_scope(std::string identifier);
		bool func_in_scope(std::string identifier, int argc);

		void add_var(std::string identifier);
		void add_func(std::string identifier, int argc);

		// removes last added scope
		void pop();
		// adds a new subscope on top
		void push();

		// string representation of scope
		std::string to_string();

		Scope();
};
