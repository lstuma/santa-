echo "Transpiling Lexer"
lex xmas.l
echo "Compiling Lexer"
gcc lex.yy.c -o xmas_lex

echo "Starting Lexer"
#cat test.clause | ./xmas_lex

echo "Syntax-checking compiler.cpp"
g++-12 tokenagency.cpp parseresult.cpp scope.cpp errorhandler.cpp astnode.cpp parser.cpp codegen/generator.cpp compiler.cpp -o compiler -std=c++20 -fsyntax-only

if [ $? -eq 0 ]
then
	
	echo "Compiling compiler.cpp"
	g++-12 tokenagency.cpp parseresult.cpp scope.cpp errorhandler.cpp astnode.cpp parser.cpp codegen/generator.cpp compiler.cpp -o compiler -std=c++20 

	echo "Starting Compiler"
	cat test.clause | ./xmas_lex | ./compiler

	echo "Running program"
	./hello_i386_32
else
	echo "Syntax checking failed with code $?"
fi