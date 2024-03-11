
#include <iostream>
#include <cassert>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <string_view>
#include <list>
#include <array>

bool debug = true;

using namespace std;

enum toktype {
    tok_err=-1,
    tok_eof=0,
    tok_num,
    tok_str,
    tok_dt_str,
    tok_dt_num,
    tok_ret,
    tok_func,
    tok_else,
    tok_if,
    tok_read,
    tok_write,
    tok_name,
    tok_plus,
    tok_minus,
    tok_mul,
    tok_div,
    tok_mod,
    tok_equals,
    tok_open_rbracket,
    tok_close_rbracket,
    tok_open_cbracket,
    tok_close_cbracket,
    tok_comma,
};

class Token {
    public: toktype type;
    public: int num;
    public: string str;
    public: Token(toktype r, int n, string s) {
          type = r;
          num = n;
          str = s;
      }
      public: Token(string s) {
          type = toktype(atoi(s.substr(0, s.find(",")).c_str()));
          s.erase(0, s.find(",")+1);
          num = atoi(s.substr(0, s.find(",")).c_str());
          s.erase(0, s.find(",")+1);
          if(type == tok_str) str = s.substr(1, s.size()-2);
          else str = s;
      }
};

// returns list of tokens
list<Token> fetch_tokens() {
    list<Token> l = {};
    
    // token stream will come through stdin
    
    while(true) {
        // read token from input stream
        string s;
        getline(cin, s);
        Token t = Token(s);
        // if err is passed, print it to stdout
        if(t.type == tok_err) {
            cout << t.str << "\n";
        }
        // if no new token is passed, return list of tokens
        if(t.type == tok_eof) {
            return l;
        }
        // otherwise
        l.push_back(t);
    }
}

class ASTNode {
    public: Token t;
    public: ASTNode left;
    public: ASTNode right;
    public: ASTNode(Token _t, ASTNode _left, ASTNode _right)
    {
          t = _t;
          left = _left;
          right = _right;
      }
    public: ASTNode(Token _t, Token _left, Token _right)
      {
          t = _t;
          left = ASTNode(_left);
          right = ASTNode(_right);
      }
    public: ASTNode(Token _t)
      {
          t = _t;
      }
};

class SyntaxParser {
    public: ASTNode generate_ast(list<Token> l) {
          // generate an AST from the chronological list of tokens
          return ASTNode(Token(2,3,"test"));
      }
};


int main() {
    if(debug) printf("[i] fetching and processing tokens...\n");
    list<Token> l = fetch_tokens();
    
    if(debug) {
        printf("[+] received tokens:");
        for (auto v : l) cout << v.type << "," << v.num << "," << v.str << " ";
        printf("\n");
    }
    
    return 0;
}