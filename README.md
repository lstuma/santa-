# santa()
# Development
## Requirements
- at least `g++12` or newer (will probably upgrade to `g++20` soon)
## Testing
Run the `t.sh` script under the folder _src_ to test the compiler on the _src/test.clause_ script.
All developmental scripts etc. are made for Linux, if you want to test on windows, you're on your own.
## Current state of development
- Lexer works
- Syntax Parser works (like ~90%)
## Future developments
- AST generation
-

# Language
## Tokens
```tokens
#.*\n 			/* comment */
{WHITE}+		/* whitespace */
{NUM}			TOK_NUM;
'{ALPHA}*'		TOK_STR;
"{ALPHA}*"		TOK_STR;
ribbon			TOK_DT_STR;		/* datatype text */
bells			TOK_DT_NUM;		/* datatype num */
gift			TOK_RET;		/* return */
elf				TOK_FUNC;			/* function definition */
grinch			TOK_ELSE;		/* variable definition */
santa			TOK_IF;		/* if */
say				TOK_READ;			/* read */
hear			TOK_WRITE;		/* write */
{ALPHA}+		TOK_NAME;		

\+				TOK_PLUS;
-				TOK_MINUS;
\*				TOK_MUL;
\/				TOK_DIV;
%				TOK_MOD;
=				TOK_EQUALS;

\(				TOK_OPEN_RBRACKET;
\)				TOK_CLOSE_RBRACKET;
\{				TOK_OPEN_CBRACKET;
\}				TOK_CLOSE_RBRACKET;
,				TOK_COMMA;
```
## Syntax
```EBNF
start ::= (expression|statement)*
expression ::= func_call | str_expr | math_expr | condition
condition ::= gen_val
str_expr ::= gen_str (tok_plus gen_str)*
math_expr ::= gen_num ((tok_plus|tok_minus|tok_mul|tok_div|tok_mod) gen_num)*
gen_val ::= gen_str | gen_num
gen_str ::= tok_name | tok_str
gen_num ::= tok_name | tok_num
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
func_call ::= tok_name tok_open_rbracket [tok_name (tok_comma, tok_name)*] tok_close_rbracket
func_def ::= tok_func tok_name tok_open_rbracket [expression (tok_comma, expression)*] tok_close_rbracket statement
```
## Processing
### Lexer
The output of the Lexer resembles the format of a CSV (Comma Separated Values) file:
```tokenstream
<toktype>,<lineno>,<optional_int>,<optional_str>
```

Whereas:
- *toktype:* int representation of the enum of the token type
- *lineno:* line in source code where token can be found
- *optional_int:* an integer, e.g. metainfo for tok_num
- *optional_str:* a string, e.g. metainfo for tok_str or tok_name

```tokenstream
4,1,0,'null'
11,1,0,c
17,1,0,'null'
1,1,1234,'null'
3,2,0,'null'
11,2,0,my_var
17,2,0,'null'
2,2,0,abcd
6,4,0,'null'
8,4,0,'null'
```

As for now the output stream will be piped into to the through stdin compiler:

```bash
cat test.clause | ./xmas_lex | ./compiler
```

### How do we parse the syntax?
We will parse the syntax tree using a **Recursive Descent** (top-down) **Parser**. For this to work the Context Free Grammar should *not* have:
- Left Recursion
- Non-Determinism
