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

class Logger {
	public:
		static int debug_level;

		static void error(std::string msg);
		static void warning(std::string msg);
		static void info(std::string msg);
		static void verbose(std::string msg);
		static void debug(std::string msg);
}