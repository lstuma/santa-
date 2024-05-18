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
#include "parser.h"
#include "codegen/generator.h"

bool debug = true;

using namespace std;

// handles tokens
TokenAgency tokenagency;

int main() {
    // get tokens from lexer
    if(debug) printf("[i] fetching and processing tokens...\n");
    
    // get tokens from stdin
    tokenagency = TokenAgency();
    tokenagency.fetch_tokens();

    if(debug) {
        printf("[+] received tokens: \n");
        for (Token t: tokenagency.tokenstream) cout << t.to_string() << "\n";
        printf("\n");
    }

    // syntactic analysis (+checking for previous lexer errors)
    Parser parser = Parser(tokenagency);
    bool valid_syntax = parser.syntactic_analysis();
    if(!valid_syntax) {
        std::cout << Color::bhblue << "santa" << Color::reset << ":" << " syntactic analysis " << Color::bhred << "FAILED\n" << Color::reset;
        return -1;
    }
    std::cout << Color::bhblue << "santa" << Color::reset << ":" << " syntactic analysis " << Color::bhgreen << "SUCCEDED\n" << Color::reset;

    // semantic analysis
    bool valid_semantic = parser.semantic_analysis();
    if(!valid_semantic) {
        std::cout << Color::bhblue << "santa" << Color::reset << ":" << " semantic analysis " << Color::bhred << "FAILED\n" << Color::reset;
        return -1;
    }
    std::cout << Color::bhblue << "santa" << Color::reset << ":" << " semantic analysis " << Color::bhgreen << "SUCCEDED\n" << Color::reset;

    Generator generator = Generator(parser.ast);
    bool generation_success = generator.generate();
    if(!generation_success) {
        std::cout << Color::bhblue << "santa" << Color::reset << ":" << " code generation " << Color::bhred << "FAILED\n" << Color::reset;
        return -1;
    }
    std::cout << Color::bhblue << "santa" << Color::reset << ":" << " code generation " << Color::bhgreen << "SUCCEDED\n" << Color::reset;

    return 0;
}