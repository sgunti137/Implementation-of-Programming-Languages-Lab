flex lex.l
bison -d yacc.y
g++ -std=c++11 -c lex.yy.c
g++ -std=c++11 -c yacc.tab.c
g++ -std=c++11 -c submission.cpp -o main.o
g++ -std=c++11 lex.yy.o yacc.tab.o main.o -o main
./main