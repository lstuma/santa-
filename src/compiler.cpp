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
    cout << (valid_syntax?Color::bhgreen+"[+]"+Color::reset+" Syntactic Analysis turned out correct :)\n":Color::bhred+"[-]"+Color::reset+" Syntactic Analysis turned out incorrect :(\n");
    if(!valid_syntax) return -1;

    // semantic analysis
    bool valid_semantic = parser.semantic_analysis();
    cout << (valid_semantic?Color::bhgreen+"[+]"+Color::reset+" Semantic Analysis turned out correct :)\n":Color::bhred+"[-]"+Color::reset+" Semantic Analysis turned out incorrect :(\n");
    if(!valid_semantic) return -1;

    return 0;
}