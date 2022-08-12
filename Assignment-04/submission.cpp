/* 
  Author: Saaketh Gunti, 190101080.
  Programming Assignment 04, CS348
*/

#include <bits/stdc++.h>
using namespace std;

extern FILE* yyin; // input file to lexer
extern int yyparse(void);
vector<string> KeyWords; // vector to store the reserved key words of the grammar
int errorCount; // number of errors encountered so far

// Function to write all the keywords into KeyWords vector
void UpdateKeyWords() {
    KeyWords.push_back("TO");
    KeyWords.push_back("DO");
    KeyWords.push_back("DIV");
    KeyWords.push_back("PROGRAM");
    KeyWords.push_back("VAR");
    KeyWords.push_back("BEGIN");
    KeyWords.push_back("END");
    KeyWords.push_back("END.");
    KeyWords.push_back("INTEGER");
    KeyWords.push_back("REAL");
    KeyWords.push_back("FOR");
    KeyWords.push_back("READ");
    KeyWords.push_back("WRITE");
    return;
}

int main() {

    // opening the input file
    FILE* input;
    input = fopen("input.txt", "r");
    
    // assign the lexer input to input file pointer
    yyin = input;

    cout << "Compilation Started" << endl;

    UpdateKeyWords();

    errorCount = 0;

    bool ok = (yyparse() == 0);

    if(ok == 1) {
        // all the statements in the input file follows the grammar
        cout << "Parse Succesful" << endl;
    } else 
        cout << "Parse Unsuccesful" << endl;

    if(errorCount == 0) {
        cout << "\nNo Syntax or Semantic Errors" << endl;
    }

    cout << "Compilation Finished" << endl;

    // close the input file
    fclose(input);

    return 0;
}