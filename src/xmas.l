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

		TOK_TAB,
		TOK_NEWLINE,
		TOK_COLON,

		TOK_COMMENT,
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

%%

#.*\n 			return TOK_COMMENT; /* comment */
[ ]+			/* whitespace */
{NUM}			return TOK_NUM;
'{ALPHA}*'		return TOK_STR;
\"{ALPHA}*\"	return TOK_STR;
ribbon			return TOK_DT_STR;		/* datatype text */
bells			return TOK_DT_NUM;		/* datatype num */
return			return TOK_RET;		/* return */
elf				return TOK_FUNC;			/* function definition */
else			return TOK_ELSE;		/* variable definition */
if				return TOK_IF;		/* if */
say				return TOK_READ;			/* read */
hear			return TOK_WRITE;		/* write */
{ALPHA}+		return TOK_NAME;		

\+				return TOK_PLUS;
-				return TOK_MINUS;
\*				return TOK_MUL;
\/				return TOK_DIV;
%				return TOK_MOD;
=				return TOK_EQUALS;
:				return TOK_COLON;

\(				return TOK_OPEN_RBRACKET;
\)				return TOK_CLOSE_RBRACKET;
\{				return TOK_OPEN_CBRACKET;
\}				return TOK_CLOSE_RBRACKET;
,				return TOK_COMMA;

[\t ]*\n+		return TOK_NEWLINE;
\t				return TOK_TAB;


.				return ERR;

%%

int main() {
	enum toktype r;
	while(r=yylex()) {
		switch(r) {
			case TOK_NUM: printf("%d,%d,%s,%d,\'null\'\n", r, yylineno, yytext, atoi(yytext)); break;
			case TOK_STR: printf("%d,%d,%s,0,%s\n", r, yylineno, yytext, yytext); break;
			case TOK_NAME: printf("%d,%d,%s,0,%s\n", r, yylineno, yytext, yytext); break;

			case TOK_NEWLINE: printf("%d,%d,,0,\\n\n", r, yylineno-1); break;

			// if line is a comment, we'll need to replace the newline character at the end:
			case TOK_COMMENT:
				int i = 0; while(yytext[i]) i++;
				yytext[i-1] = '\0';
				printf("%d,%d,%s,0,\'null\'\n", r, yylineno-1, yytext);
				// fake TOK_NEWLINE
				printf("%d,%d,,0,\\n\n", 24, yylineno-1);
				break;

			case ERR: printf("-1,%d,%s,0,\"Unexpected \"\e[1;90m%s\e[1;94m\" found\n", yylineno, yytext, yytext); break;
			case TOK_EOF: printf("EOF\n"); break;
			default: printf("%d,%d,%s,0,\'null\'\n", r, yylineno, yytext); break;
		}
	}
	printf("EOF\n");
}
