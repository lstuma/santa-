echo "-={Syntax Analysis}=-"
echo "Transpiling Lexer"
lex xmas.l
echo "Compiling Lexer"
gcc lex.yy.c -o xmas_lex

ehco "-={Semantic Analysis}=-"
echo "Transpiling Parser"
yacc xmas.y -o yac.yy.c
echo "Compiling Parser"
gcc yac.yy.c -o xmas_yacc

echo "-={Tests}=-"
echo "Test 1: Testing Syntax Analysis:"
cat test.clause | ./xmas_lex