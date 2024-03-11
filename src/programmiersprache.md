- funktionen
- input, output
- operation ($+$, $-$, $*$, $\div$, $\%$)
- variablen
	- basic variablentypen: _int_, _\[null temrinated\] string_
		=> *int* is called bells
		=> *string* is called ribbon

christmassy words:
- elf
- gift
- cane
- snow
- pole
- bells
- reindeer
- santa
- snowman
- hohoho

funktionendefinition:
`elf <name> <parameter>:`

hello world:
```santa
hohoho
```

if-else:
```
santa n==1 gift 1
grinch gift n*factorial(n-1)
```



Example
```(Clause
elf factor)ial n, p {
	say p
	santa {n==1 gift 1}
	grinch {gift n*(factorial n-1,p)}
}

bells num = 5
factorial num, 'computing'

ribbon var = ''
say 'Ho Ho Ho!'
hear var
```

EBNF
```ebnf
CODE ::= EXPRESSION OPT_WHITESPACE '\n' OPT_WHITESPACE CODE|OPT_WHITESPACE
EXPRESSION ::= FUNCTION_CALL|FUNCTION_DEFINITION|VARIABLE_DEFINITION|GIFT|IF_STATEMENT|COMMENT

COMMENT ::= '#' NOT_NL '\n'
OPT_WHITESPACE ::= WHITESPACE | epsilon
WHITESPACE ::= '\n' WHITESPACE | '\n' | ' ' | ' ' WHITESPACE
IF_STATEMENT ::= 'santa' FUNCTION_CALL FUNCTION_CALL ELSE_STATEMENT
ELSE_STATEMENT ::= 'grinch' FUNCTION_CALL|epsilon
FUNCTION_CALL :== NAME PARAMS
FUNCTION_DEFINITION ::= 'elf' NAME PARAMS '{' OPT_WHITESPACE CODE '}'
PARAMS ::= NAME | NAME, PRAMS
VARIABLE_DEFINITION ::= DATATYPE NAME | 'ribbon' NAME '=' TEXT_VALUE | 'bells' NAME '=' NUM_VALUE | NAME '=' VALUE
GIFT ::= 'gift' VALUE | 'gift' NAME
DATATYPE = 'ribbon' | 'bells'

ALPHA_OR_UNDERSCORE = 'a'|...|'z'|'A'|...|'Z'|'_'
NUM = '0'|...|'9'
NAME = ALPHA_OR_UNDERSCORE | ALPHA_OR_UNDERSCORE NAME

VALUE = NUM_VALUE | TEXT_VALUE
NUM_VALUE = NUM NUM_VALUE | epsilon
TEXT_VALUE = '\'' CHAR TEXT_VALUE '\'' | epsilon

```
