#ifndef color_h
#define color_h

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

class Color {
	public:
		// regular
		constexpr static std::string black = "\e[0;30m";
		constexpr static std::string red = "\e[0;31m";
		constexpr static std::string green = "\e[0;32m";
		constexpr static std::string yellow = "\e[0;33m";
		constexpr static std::string blue = "\e[0;34m";
		constexpr static std::string magenta = "\e[0;35m";
		constexpr static std::string cyan = "\e[0;36m";
		constexpr static std::string white = "\e[0;37m";

		// regular bold
		constexpr static std::string bblack = "\e[1;30m";
		constexpr static std::string bred = "\e[1;31m";
		constexpr static std::string bgreen = "\e[1;32m";
		constexpr static std::string byellow = "\e[1;33m";
		constexpr static std::string bblue = "\e[1;34m";
		constexpr static std::string bmagenta = "\e[1;35m";
		constexpr static std::string bcyan = "\e[1;36m";
		constexpr static std::string bwhite = "\e[1;37m";

		// regular underline
		constexpr static std::string ublack = "\e[4;30m";
		constexpr static std::string ured = "\e[4;31m";
		constexpr static std::string ugreen = "\e[4;32m";
		constexpr static std::string uyellow = "\e[4;33m";
		constexpr static std::string ublue = "\e[4;34m";
		constexpr static std::string umagenta = "\e[4;35m";
		constexpr static std::string ucyan = "\e[4;36m";
		constexpr static std::string uwhite = "\e[4;37m";

		// regular bg
		constexpr static std::string blackb = "\e[40m";
		constexpr static std::string redb = "\e[41m";
		constexpr static std::string greenb = "\e[42m";
		constexpr static std::string yellowb = "\e[43m";
		constexpr static std::string blueb = "\e[44m";
		constexpr static std::string magentab = "\e[45m";
		constexpr static std::string cyanb = "\e[46m";
		constexpr static std::string whiteb = "\e[47m";

		// high intensty bg
		constexpr static std::string blackhb = "\e[0;100m";
		constexpr static std::string redhb = "\e[0;101m";
		constexpr static std::string greenhb = "\e[0;102m";
		constexpr static std::string yellowhb = "\e[0;103m";
		constexpr static std::string bluehb = "\e[0;104m";
		constexpr static std::string magentahb = "\e[0;105m";
		constexpr static std::string cyanhb = "\e[0;106m";
		constexpr static std::string whitehb = "\e[0;107m";

		// high intensty
		constexpr static std::string hblack = "\e[0;90m";
		constexpr static std::string hred = "\e[0;91m";
		constexpr static std::string hgreen = "\e[0;92m";
		constexpr static std::string hyellow = "\e[0;93m";
		constexpr static std::string hblue = "\e[0;94m";
		constexpr static std::string hmagenta = "\e[0;95m";
		constexpr static std::string hcyan = "\e[0;96m";
		constexpr static std::string hwhite = "\e[0;97m";

		// bold high intensity
		constexpr static std::string bhblack = "\e[1;90m";
		constexpr static std::string bhred = "\e[1;91m";
		constexpr static std::string bhgreen = "\e[1;92m";
		constexpr static std::string bhyellow = "\e[1;93m";
		constexpr static std::string bhblue = "\e[1;94m";
		constexpr static std::string bhmagenta = "\e[1;95m";
		constexpr static std::string bhcyan = "\e[1;96m";
		constexpr static std::string bhwhite = "\e[1;97m";

		// reset
		constexpr static std::string reset = "\e[0m";
};

#endif