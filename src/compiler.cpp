
#include <iostream>
#include <cassert>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <string_view>
#include <list>
#include <array>
#include <any>
#include <map>

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
    public: int lineno;
    public: Token(toktype r, int n, string s) {
          type = r;
          num = n;
          str = s;
      }
      public: Token(string s) {
          type = toktype(atoi(s.substr(0, s.find(",")).c_str()));
          s.erase(0, s.find(",")+1);
          lineno = atoi(s.substr(0, s.find(",")).c_str());
          s.erase(0, s.find(",")+1);
          num = atoi(s.substr(0, s.find(",")).c_str());
          s.erase(0, s.find(",")+1);
          if(type == tok_str) str = s.substr(1, s.size()-2);
          else str = s;
      }
};

// tokenstream form lexer
list<Token> tokenstream;
class TokenAgency {
  public: static Token get_token(int i) {
        auto it = tokenstream.begin();
        advance(it, i);
        return *it;
    }
    // returns list of tokens
  public: static list<Token> fetch_tokens() {
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
};


class ASTNode {
    public: Token* t = nullptr;
    public: ASTNode* left = nullptr;
    public: ASTNode* right = nullptr;
    public: ASTNode() {
      }
    public: ASTNode(Token* _t) {
          t = _t;
      }
    public: ASTNode(Token* _t, Token* _l, Token* _r) {
          t = _t;
          ASTNode _left = ASTNode(_l);
          left = &_left;

          ASTNode _right = ASTNode(_r);
          right = &_right;
      }
};

class SyntaxParser {

    toktype* tokens;   // array with
    int length;
    public: int pos;            // position of parser
    public: int lookahead;      // position of lookahead pointer

    public: SyntaxParser(toktype* _tokens, int _length) {
          tokens = _tokens;
          length = _length;
          pos = 0;
          lookahead = 0;
      }

    toktype get_token(int _pos) {
        // returns token at position if existent otherwise -1;
        if(_pos >= length) return tok_eof;
        return tokens[_pos];
    }
    toktype get_token(toktype* _tokens, int _pos, int _length) {
        // returns token at position if existent otherwise -1;
        if(_pos >= _length) return tok_eof;
        return _tokens[_pos];
    }

    toktype* get_tokens(int start_pos, int end_pos) {
        // generates new array on heap that holds all the tokens from start_pos to end_pos
        toktype* _tokens = (toktype*) malloc(sizeof(toktype)*(end_pos-start_pos));
        for(int i = start_pos; i < end_pos; i++)
            _tokens[i] = get_token(i);
        return _tokens;
    }

    public: bool match(toktype* m, int length) {
          // matches the next few lookahead tokens against the provided tokens
          int _lookahead = lookahead;
          for(int i = 0; i < length; i++) {
              if(!match(m[i])) {
                  lookahead = _lookahead;
                  return false;
              }
          }
          return true;
      }

    public: bool match(toktype m) {
          // matches the next lookahead token against the provided token
          if(get_token(lookahead) == m) {
              lookahead++;
              return true;
          }
          return false;
      }

    public: bool parse() {
          // parses all the tokens and checks for faulty syntax
          while(pos < length) {
              if(match_expression()) pos = lookahead;
              else if(match_statement()) pos = lookahead;
              else {
                  printf("\e[1;91mSyntax Error\e[0m in line \e[1;33m%d\e[0m: Invalid syntax!\n", TokenAgency::get_token(pos).lineno);
                  return false;
              }
              if(debug) cout << "POS: " << pos << " / " << length << "\r";
          }
          return true;
      }

    /*
        Expressions

      EBNF:, )
        expression ::= func_call | str_expr | math_expr | condition
        condition ::= gen_str | gen_num
        str_expr ::= gen_str | gen_str tok_plus str_expr
        math_expr ::= gen_num | gen_num (tok_plus|tok_minus|tok_mul|tok_div|tok_mod) math_expr
        gen_str ::= tok_name | tok_str
        gen_num ::= tok_name | tok_num
        func_call ::= tok_name tok_open_rbracket [tok_name (tok_comma, tok_name)*] tok_close_rbracket
    */
    bool match_expression() {
        return match_math_expr()
        || match_str_expr()
        || match_condition_expr()
        || match_gen_num_expr()
        || match_gen_str_expr()
        || match_func_call_expr();
    }

    bool match_math_expr() {
        int _lookahead = lookahead;
        if(match(tok_num)) {
            // if no '+' '-' '*' '/' or '%' match it's just a number otherwise look if there is another math expression following
            if(!(match(tok_plus) || match(tok_minus) || match(tok_mul) || match(tok_div) || match(tok_mod))) return true;
            else if(match_math_expr()) return true;
        }
        lookahead = _lookahead;
        return false;
    }

    bool match_str_expr() {
        int _lookahead = lookahead;
        if(match_gen_str_expr()) {
            // if no '+' follows it's just a string otherwise check for another string expression following
            if(!match(tok_plus)) return true;
            else if (match_str_expr()) return true;
        }
        lookahead = _lookahead;
        return false;
    }

    bool match_condition_expr() {
        return match_gen_str_expr() || match_gen_num_expr();
    }

    bool match_gen_num_expr() {
        return match(tok_name) || match(tok_num);
    }

    bool match_gen_str_expr() {
        return match(tok_name) || match(tok_str);
    }

    bool match_func_call_expr() {
        return false;
    }

    /*
        Statements

        EBNF:
        statement ::= open_cbracket (statement)* close_cbrackets | var_decl | var_init | var_def | fun_call | func_def | conditional
        conditional ::= tok_if condition statement [tok_else statement]
        var_decl ::= num_decl | str_decl
        var_init ::= num_init | str_init
        var_def ::= num_def | str_def
        num_decl ::= tok_dt_num tok_name
        num_init ::= tok_dt_num tok_name tok_equals expression
        num_def ::= tok_name tok_equals expression
        str_decl ::= tok_dt_str tok_name
        str_init ::= tok_dt_str tok_name tok_equals expression
        str_def ::= tok_name tok_equals expression
        func_def ::= tok_func tok_name tok_open_rbracket [expression (tok_comma, expression)*] tok_close_rbracket statement
    */
    bool match_statement() {
        return match_conditional()
        || match_num_init()
        || match_num_decl()
        || match_str_init()
        || match_str_decl()
        || match_var_def()
        || match_func_def();
    }

    bool match_conditional() {
        int _lookahead = lookahead;
        if(match(tok_if) && match_condition_expr() && match_statement()) {
            // if we don't match 'else' or match 'else' <statement> return true
            if(!match(tok_else) || match_statement()) return true;
        }
        lookahead = _lookahead;
        return false;
    }

    bool match_num_decl() {
        return match(new toktype[] {tok_dt_num, tok_name}, 2);
    }

    bool match_num_init() {
        int _lookahead = lookahead;
        if(match(new toktype[] {tok_dt_num, tok_name, tok_equals}, 3) && match_expression()) return true;
        lookahead = _lookahead;
        return false;
    }

    bool match_str_decl() {
        return match(new toktype[] {tok_dt_str, tok_name}, 2);
    }

    bool match_str_init() {
        int _lookahead = lookahead;
        if(match(new toktype[] {tok_dt_str, tok_name, tok_equals}, 3) && match_expression()) return true;
        lookahead = _lookahead;
        return false;
    }

    bool match_var_def() {
        int _lookahead = lookahead;
        if(match(new toktype[] {tok_name, tok_equals}, 2) && match_expression()) return true;
        lookahead = _lookahead;
        return false;
    }

    bool match_func_def() {
        int _lookahead = lookahead;
        if(match(new toktype[] {tok_func, tok_name, tok_open_rbracket}, 3)) {
            // if next token is ')' there are no arguments, otherwise parse for args; followed by a statement
            if((match(tok_close_rbracket) || match_func_def_args() && match(tok_close_rbracket)) && match_statement()) return true;
        }
        lookahead = _lookahead;
        return false;
    }

    bool match_func_def_args() {
        // does not restore lookahead if fail
        bool m = match(tok_name);
        while(match(tok_comma)) m = match(tok_name);
        return m;
    }

    public: ASTNode generate_ast(list<Token> l) {
          // generate an AST from the chronological list of tokens
          return ASTNode();
      }
};


int main() {
    if(debug) printf("[i] fetching and processing tokens...\n");
    tokenstream = TokenAgency::fetch_tokens();
    
    if(debug) {
        printf("[+] received tokens: ");
        for (auto v : tokenstream) cout << v.type << "," << v.lineno << "," << v.num << "," << v.str << " ";
        printf("\n");
    }
    
    // transform list of tokens (containing information such as variable names etc.)  into array of just tokens (not containing any metainfo)
    if(debug) printf("[i] creating array of tokens for syntax parsing\n[i] tokens = [ ");
    toktype tokens[tokenstream.size()];
    list<Token>::iterator it = tokenstream.begin();
    for (int i = 0; i < tokenstream.size(); i++) {
        tokens[i] = (*it).type;
        if(debug) cout << tokens[i] << " ";
        it++;
    }
    if(debug) printf("]\n");

    // now parse syntax
    SyntaxParser parser = SyntaxParser(tokens, sizeof(tokens)/sizeof(tokens[0]));
    bool valid_code = parser.parse();

    cout << (valid_code?"[+] Syntax is correct :)\n":"[-] Syntax is incorrect :(\n");

    return 0;
}