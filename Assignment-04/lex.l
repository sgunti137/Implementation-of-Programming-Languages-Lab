/* 
  Author: Saaketh Gunti, 190101080.
  Programming Assignment 04, CS348
*/

%{

#include <bits/stdc++.h>
#include "yacc.tab.h" /* This file contains token definitions and union structure */

using namespace std;

int lc = 0; /* Line number of the input */
extern int errorCount; /* Number of errors encountered so far */
void errorHandler(void); /* Functions to handle errors */

%}

%option noyywrap

%%

PROGRAM { return define_PROGRAM;}
VAR { return define_VAR;}
BEGIN { return define_BEGIN;}
END { return define_END;}
"END." { return define_END_DOT;}
INTEGER { return define_INT_TYPE;}
REAL { return define_REAL_TYPE;}
FOR { return define_FOR;}
READ { return define_READ;}
WRITE { return define_WRITE;}
TO { return define_TO;}
DO { return define_DO;}
";" { return define_SEMICOLON;}
":" { return define_COLON;}
"," { return define_COMMA;}
":=" { return define_ASSIGN;}
"+" { return define_PLUS;}
"-" { return define_MINUS;}
"*" { return define_MULT;}
DIV { return define_DIV;}
"(" { return define_OPEN_BRACKET;}
")" { return define_CLOSE_BRACKET;}

[A-Z]([A-Z0-9])* {
  yylval.string_val = (char *)malloc(strlen(yytext) + 1);
  strcpy(yylval.string_val, yytext);
  return define_ID;
}

[0-9]+ {
  yylval.int_val = atoi(yytext);
  return define_INT;
}

[0-9]+"."[0-9]+ {
  yylval.double_val = atof(yytext);
  return define_REAL;
}

[\t] {}
" " {}
"\n" {lc++;}
. {errorHandler(); return -1;}
%%

void errorHandler() {
  char* newError;
	newError = (char *)malloc(sizeof(char)* 256);
  newError[0] = '\0';
  sprintf(newError, "'%c' (0%o) : illegal character at line %d", yytext[0], yytext[0], (lc + 1));
  string errstr(newError);
  errorCount++;
  if(errorCount == 1) {
    cout << endl << "--ERRORS--" << endl;
  }
  cout << errstr << endl;
  return;
}