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

Token::Token(toktype r, int n, std::string s) {
  this->type = r;
  this->lineno = 0;
  this->num = n;
  this->str = s;
}

Token::Token(toktype r) {
  this->type = r;
  this->lineno = 0;
  this->num = 0;
  this->str = "";
}

Token::Token(toktype r, int lineno) {
  this->type = r;
  this->lineno = lineno;
  this->num = 0;
  this->str = "";
}

Token::Token(std::string s) {
  this->type = toktype(atoi(s.substr(0, s.find(",")).c_str()));
  s.erase(0, s.find(",")+1);
  this->lineno = atoi(s.substr(0, s.find(",")).c_str());
  s.erase(0, s.find(",")+1);
  this->raw = s.substr(0, s.find(","));
  s.erase(0, s.find(",")+1);
  this->num = atoi(s.substr(0, s.find(",")).c_str());
  s.erase(0, s.find(",")+1);
  if(type == tok_str) str = s.substr(1, s.size()-2);
  else str = s;
}

TokenAgency::TokenAgency() {
    tokenstream = std::list<Token>();
}

std::string Token::to_string() {
    return "{type:" + Color::bblue + std::to_string(this->type) + Color::reset +
    ", lineno:" + Color::bblue + std::to_string(this->lineno) + Color::reset + 
    ", raw:" + Color::bblue + this->raw + Color::reset + 
    ", num:" + Color::bblue + std::to_string(this->num) + Color::reset +  
    ", str:" + Color::bblue + this->str + Color::reset + "}";
}

// returns list of tokens
std::list<Token> TokenAgency::fetch_tokens() {
    std::list<Token> l = {};

    // token stream will come through stdin
    while(true) {
        // read token from input stream
        std::string s;
        getline(std::cin, s);
        Token t = Token(s);
        // if err is passed, print it to stdout
        if(t.type == tok_err) {
            std::cout << t.str << "\n";
        }
        // if no new token is passed, return list of tokens
        if(t.type == tok_eof) {
            tokenstream = l;
            return l;
        }
        // otherwise
        l.push_back(t);
    }
}

Token TokenAgency::get_token(int i) {
    if(i < 0) return Token(tok_eof, 0);
    else if(i >= tokenstream.size()) return Token(tok_eof, get_token(tokenstream.size()-1).lineno);
    auto it = tokenstream.begin();
    advance(it, i);
    return *it;
}

// returns an array of toktypes for the given tokenlist
toktype* TokenAgency::array() {
    toktype* tokens = (toktype*) malloc(tokenstream.size() * sizeof(toktype));
    std::list<Token>::iterator it = tokenstream.begin();
    for (int i = 0; i < tokenstream.size(); i++) {
        tokens[i] = (*it).type;
        it++;
    }
    return tokens;
}
