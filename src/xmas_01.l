/* flex file for xmas v0.1 */

%{
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

		TOK_PLUS,
		TOK_MINUS,
		TOK_MUL,
		TOK_DIV,
		TOK_MOD
	};
%}

%option noyywrap

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

\+				return TOK_PLUS;
-				return TOK_MINUS;
\*				return TOK_MUL;
\/				return TOK_DIV;
%				return TOK_MOD;

.				return ERR;

%%

int main() {
	enum toktype r;
	while(r=yylex()) {
		switch(r) {
			case TOK_NUM: printf("n=%d\n", atoi(yytext)); break;
			case TOK_STR: printf("s=%s\n", yytext); break;

			case ERR: 
			case TOK_EOF: printf("err %d\n", r); break;
			default: printf("-- %d --\n", r); break;
		}
	}
}
