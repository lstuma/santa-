/* flex file for xmas v0.1 */

%{
	#include <stdio.h>
	#include <string.h>

	enum toktype {
		ERR=-1,
		TOK_EOF=0,

		TOK_NUM=1,
		TOK_STR,
		TOK_DT_STR,
		TOK_DT_NUM,
		TOK_RET,
		TOK_FUNC,
		TOK_ELSE,
		TOK_IF,
		TOK_READ,
		TOK_WRITE,
		TOK_NAME,

		TOK_PLUS,
		TOK_MINUS,
		TOK_MUL,
		TOK_DIV,
		TOK_MOD,
		TOK_EQUALS,
		
		TOK_OPEN_RBRACKET,
		TOK_CLOSE_RBRACKET,
		TOK_OPEN_CBRACKET,
		TOK_CLOSE_CBRACKET,
		TOK_COMMA,
	};
%}

%option noyywrap
%option yylineno

DIGIT	[0-9]
NUM {DIGIT}+


LOWERCASE [a-z]
UPPERCASE [A-Z]
PUNCTUATION [.,;!?]
ALPHA	[a-zA-Z_]
WHITE	[ \t\n]

%%

#.*\n 			/* comment */
{WHITE}+		/* whitespace */
{NUM}			return TOK_NUM;
'{ALPHA}*'		return TOK_STR;
"{ALPHA}*"		return TOK_STR;
ribbon			return TOK_DT_STR;		/* datatype text */
bells			return TOK_DT_NUM;		/* datatype num */
gift			return TOK_RET;		/* return */
elf				return TOK_FUNC;			/* function definition */
grinch			return TOK_ELSE;		/* variable definition */
santa			return TOK_IF;		/* if */
say				return TOK_READ;			/* read */
hear			return TOK_WRITE;		/* write */
{ALPHA}+		return TOK_NAME;		

\+				return TOK_PLUS;
-				return TOK_MINUS;
\*				return TOK_MUL;
\/				return TOK_DIV;
%				return TOK_MOD;
=				return TOK_EQUALS;

\(				return TOK_OPEN_RBRACKET;
\)				return TOK_CLOSE_RBRACKET;
\{				return TOK_OPEN_CBRACKET;
\}				return TOK_CLOSE_RBRACKET;
,				return TOK_COMMA;

.				return ERR;

%%

int main() {
	enum toktype r;
	while(r=yylex()) {
		switch(r) {
			case TOK_NUM: printf("%d,%d,\'null\'\n", r, atoi(yytext)); break;
			case TOK_STR: printf("%d,0,%s\n", r, yytext); break;
			case TOK_NAME: printf("%d,0,%s\n", r, yytext); break;

			case ERR: printf("-1,0,\"\e[1;91mLexical Error\e[0m in line \e[1;33m%d\e[0m: Unexpected \e[0m\"\e[1;94m%s\e[0m\" found!", yylineno, yytext); break;
			case TOK_EOF: printf("EOF\n"); break;
			default: printf("%d,0,\'null\'\n", r); break;
		}
	}
	printf("EOF\n");
}
