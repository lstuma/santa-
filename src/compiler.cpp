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
        
    // now parse syntax
    Parser parser = Parser(tokenagency);
    bool valid_code = parser.parse();
    cout << (valid_code?Color::bhgreen+"[+]"+Color::reset+" Syntax is correct :)\n":Color::bhred+"[-]"+Color::reset+" Syntax is incorrect :(\n");

    return 0;
}