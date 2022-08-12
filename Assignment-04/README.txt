Written by Saaketh Gunti, 190101080.
Programming Assignment 4
CS348

Commands to run the program:
	bash run.sh
After execution is over, to delete all the object files use the command:
	bash clean.sh


File and there roles:
1> lex.l 			: lexical analyser
2> main.cpp			: driving function
3> input.txt		: File from which the code takes input.
4> correct_input.txt: This is an input file with no semantic and syntax errors (to check the correctness of the program).
						To execute with this input, change the input file in line 28 of submission.cpp
5> yacc.y			: BNF rules are defined here
				  	  Takes care of the syntax and semantic errors
6> run.sh			: code to compile and run the files
7> clean.sh			: code to delete all the object files created while compilation

Types of error handled:
1> Syntax errors 	: foreign character,
					  statements that do not follow the grammar,
					  variable declarations with reserved keywords(e.g. FOR, TO, BEGIN, etc.)
2> Semantic errors	: variables not declared but used,
					  variables having different types in expressions and assignments,
					  duplicate declarations
					  

