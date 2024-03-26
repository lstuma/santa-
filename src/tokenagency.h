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

enum toktype {
    tok_err=-1,
    tok_eof=0,
    tok_num=1,
    tok_str=2,
    tok_dt_str=3,
    tok_dt_num=4,
    tok_ret=5,
    tok_func=6,
    tok_else=7,
    tok_if=8,
    tok_read=9,
    tok_write=10,
    tok_name=11,
    tok_plus=12,
    tok_minus=13,
    tok_mul=14,
    tok_div=15,
    tok_mod=16,
    tok_equals=17,
    tok_open_rbracket=18,
    tok_close_rbracket=19,
    tok_open_cbracket=20,
    tok_close_cbracket=21,
    tok_comma=22,
    tok_tab=23,
    tok_newline=24,
    tok_colon=25,
    tok_comment=26,
};

class Token {
    public: 
      toktype type;
      int num;
      std::string str;
      std::string raw;
      int lineno;
      
      Token();
      Token(toktype r, int n, std::string s);
      Token(toktype r, int lineno);
      Token(toktype r);
      Token(std::string s);
      std::string to_string();
};


class TokenAgency {
  public: 
    std::list<Token> tokenstream;
  
    TokenAgency();
    Token get_token(int i);
    toktype* array();
    std::list<Token> fetch_tokens();
};