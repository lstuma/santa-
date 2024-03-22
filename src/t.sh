echo "Transpiling Lexer"
lex xmas.l
echo "Compiling Lexer"
gcc lex.yy.c -o xmas_lex

echo "Starting Lexer"
#cat test.clause | ./xmas_lex

echo "Compiling compiler.cpp"
g++-12 tokenagency.cpp parseresult.cpp errorhandler.cpp astnode.cpp parser.cpp compiler.cpp -o compiler -std=c++20

echo "Starting Compiler"
cat test.clause | ./xmas_lex | ./compiler
