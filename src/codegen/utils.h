#ifndef utils_h
#define utils_h

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <array>
#include <list>
#include <iomanip>
#include <elfio/elfio.hpp>

namespace utils {
	// transforms a c-string into another string representing the first string in hex format (used for testing)
	std::string char_arr_to_hex(const char* arr, int length) {
		std::ostringstream oss;
		for (int i = 0; i < length; i++) {
			if(i%16 == 0)
				oss << "\n[0x" << std::hex << std::setw(8) << std::setfill('0') << i << "] ";
			oss << "0x" << std::hex << std::setw(2) << std::setfill('0') << (int)arr[i] << " ";
		}

		// high values for some reason return "ffffffX", replace "ffffff" with ""
		std::string hex = oss.str();
		size_t pos = 0;
	    while((pos = hex.find("ffffff", pos)) != std::string::npos)
	         hex.replace(pos, 6, "");

		return hex;
	};

	std::string str_to_hex(std::string str) {
		return char_arr_to_hex(str.c_str(), str.length());
	};
}

#endif