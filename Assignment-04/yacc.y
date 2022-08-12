/* 
  Author: Saaketh Gunti, 190101080.
  Programming Assignment 04, CS348
*/

%token define_PROGRAM 1
%token define_VAR 2
%token define_BEGIN 3
%token define_END 4
%token define_END_DOT 5
%token <int_val> define_INT_TYPE 6
%token <int_val> define_REAL_TYPE 7
%token define_FOR 8
%token define_READ 9
%token define_WRITE 10
%token define_TO 11
%token define_DO 12
%token define_SEMICOLON 13
%token define_COLON 14
%token define_COMMA 15
%token define_ASSIGN 16
%token define_PLUS 17
%token define_MINUS 18
%token define_MULT 19
%token define_DIV 20
%token define_OPEN_BRACKET 21
%token define_CLOSE_BRACKET 22
%token <string_val> define_ID 23
%token <int_val> define_INT 24
%token <double_val> define_REAL 25

%{

#include <bits/stdc++.h>
using namespace std;

bool ok = false;
int variableActivated = 0; // to find variable declaration
vector<string> idStore; // LIst of all ids inn the present declaration statement

extern int yylex(void);
extern int lc, errorCount;
extern vector<string> KeyWords;

// Function used to handle syntax errors
void yyerror(const char* error) {
	char* newError;
	newError = (char *)malloc(sizeof(char)* 256);
    newError[0] = '\0';
    sprintf(newError, "Error at line: %d, %s\n", (lc + 1), error);
    string errstr(newError);
	errorCount++;
	if(errorCount == 1) {
		cout << endl << "--ERRORS--" << endl;
	}
	cout << errstr << endl;
}

// Function to print the error message
void addErrorMessage(char* error) {
    string errstr(error);
	errorCount++;
	if(errorCount == 1) {
		cout << endl << "--ERRORS--" << endl;
	}
	cout << errstr << endl;
}

// Function to handle the Type mismatch errors
void handleTypeError(int a, int b) {
	char* newError;
	newError = (char *)malloc(sizeof(char)* 256);
    newError[0] = '\0';
	// A corresponds to type a, lly B corresponds to type b
    string A = "REAL", B = "REAL";

    if(a == 6) {
        A = "INTEGER";
    }

    if(b == 6) {
        B = "INTEGER";
    }

    sprintf(newError, "Error at line %d, Incompatible types %s and %s", (lc + 1), A.c_str(), B.c_str());
    string errstr(newError);
	errorCount++;
	if(errorCount == 1) {
		cout << endl << "--ERRORS--" << endl;
	}
	cout << errstr << endl;
}

// structure used to store symbol
struct variable {
    string name; // name of the variable
    int type; // Type of the variable ex: integer or real
    int line; // line number where it is declared
};

// hashtable structure
template<class Key, class Value>
class hashTable{
public:
	struct Node{
		Key key;
		Value val;
		struct Node* next;

		Node(){
			next=NULL;
		}

		Node(Key k, Value v){
			key=k;
			val=v;
			next=NULL;
		}
	};

	Node* data[10000];

	int hash(Key key){
		ostringstream oss;
		oss<<key;
		string s=oss.str();
		int ans=0;
		for(int i=0;i<s.length();i++){
			ans+=(i*((int)s[i]));
			ans%=10000;
		}
		return ans;
	}

	hashTable(){
	 	for(int i=0;i<10000;i++)
	 		data[i]=NULL;
	}

	int insert(Key key, Value val){
		int h=hash(key);
		Node* temp=data[h];
		while(temp!=NULL && temp->key!=key)
			temp=temp->next;
		if(temp==NULL){
			temp=new Node(key, val);
			temp->next=data[h];
			data[h]=temp;
			return 1;
		}
		else{
			temp->val=val;
			return 0;
		}
		return 0;
	}

  int install_id(Key key) {
    return insert(key, 0);
  }

  int install_num(Key key) {
    return insert(key, 1);
  }

	void update(Key key, Value val){
		int h=hash(key);
		Node* temp=data[h];
		while(temp!=NULL && temp->key!=key)
			temp=temp->next;
		if(temp==NULL){
			temp=new Node(key, val);
			temp->next=data[h];
			data[h]=temp;
		}
		else{
			temp->val=val;
		}
	}

	void erase(Key key){
		int h=hash(key);
		Node* temp=data[h];
		Node* del;
		if(data[h]->key==key){
			del=data[h];
			data[h]=data[h]->next;
		}
		else{
			while(temp->next!=NULL && temp->next->key!=key)
				temp=temp->next;
			if(temp->next==NULL){
				cout<<"error";
			}
			else{
				del=temp->next;
				temp->next=del->next;
				delete del;
			}
		}
	}

	Value& operator[](Key key){
		int h=hash(key);
		Node* temp=data[h];
		while(temp!=NULL && temp->key!=key)
			temp=temp->next;
		if(temp==NULL){
			temp=new Node();
			temp->key=key;
			temp->next=data[h];
			data[h]=temp;
		}
		return temp->val;
	}
};

// unordered map for symboltable
unordered_map<string, struct variable> symbolTable;

%}

// union structure
%union {
    int int_val; // to store integer values 
    double double_val; // to store double values 
    char *string_val; // to store string values 
}

// need tp specify the return type of the rules 
%type <string_val> identifier
%type <int_val> variableType exp terms factors

%%

	// If there is "VAR" then set variableActivated else don't
	// once the declarations are completed reset variableActivated
prog : define_PROGRAM programName define_VAR {variableActivated = 1;} declarationList {variableActivated = 0;} define_BEGIN statementList define_END_DOT
		;

programName : identifier
		;

dec : identifierList define_COLON variableType {
	// iterate through the declaration list
	for(int i = 0; i < idStore.size(); ++i) {
		// Find if the symbol is defined earlier
		auto iterator = symbolTable.find(idStore[i]);
		if(iterator == symbolTable.end()) {
			// If not defined earlier, do it now
			struct variable newVariable;
			newVariable.name = idStore[i];
			newVariable.type = $3;
			newVariable.line = lc;
			// Add the newVariable to symbolTable
			symbolTable[idStore[i]] = newVariable;
		} else {
			// If already defined, then produce error
			char* newError;
			newError = (char *)malloc(sizeof(char)* 256);
			newError[0] = '\0';
			sprintf(newError, "Duplicate Variable definition at line %d, %s is already defined at line %d", (lc + 1), idStore[i].c_str(), (iterator->second.line + 1));
			addErrorMessage(newError);
		}
	}
	// Clear the list in the current declaration statement
	idStore.clear();
}	;

declarationList : dec | declarationList define_SEMICOLON dec
		;

// returns the type of the variable
variableType : define_INT_TYPE {
		$$ = define_INT_TYPE;
		ok = true;
		}
	| define_REAL_TYPE {
		$$ = define_REAL_TYPE;
		ok = true;
		}
	;

identifierList : identifier {
		// if the VAR statement if getting processed
		// then push the var into idstore
		if(variableActivated == 1) {
			string var($1);
			idStore.push_back(var);
		}
	}
	| identifierList define_COMMA identifier {
		// if the VAR statement if getting processed
		// then push the var into idstore
		if(variableActivated) {
			string var($3);
			idStore.push_back(var);
		}
	}
	;

statementList : assign | read | write | for | statementList define_SEMICOLON statement

statement : assign | read | write | for
	;

assign : identifier define_ASSIGN exp {
		string var($1);
		auto iterator = symbolTable.find(var);
		// search for symbol in symbolTable
		if(iterator == symbolTable.end()) {
			// if not found then produce error
			// variable not declared
			char* newError;
			newError = (char *)malloc(sizeof(char)* 256);
			newError[0] = '\0';
			sprintf(newError,"Error at line %d, %s variable is not declared in the scope", (lc + 1), $1);
			addErrorMessage(newError);
		} else if (iterator->second.type != $3) {
			// if the variable is declared and
			// it is a type mismatch then produce type mismatch error
			handleTypeError(iterator->second.type, $3);
		}
	}
	;

// If single type then the type is same as that of the term
exp : terms {$$ = $1;}
	| exp define_PLUS terms {
		// if same type
		if($1 == $3) {$$ = $1; ok = true;}
		// handle different types
		else {handleTypeError($1, $3); ok = true;}
	}
	| exp define_MINUS terms {
		// if same type
		if($1 == $3) {$$ = $1; ok = true;}
		// handle different types
		else {handleTypeError($1, $3); ok = true;}
	}
	;

factors : identifier {
		string var($1);
		if(symbolTable.find(var) == symbolTable.end()) {
			// if not found then produce error
			char* newError;
			newError = (char *)malloc(sizeof(char)* 256);
			newError[0] = '\0';
			sprintf(newError,"Error at line %d, %s variable is not declared in the scope", (lc + 1), $1);
			addErrorMessage(newError);
		} else {
			// if found then return the type of the variable
			$$ = symbolTable.find(var)->second.type;
		}
	}
	| define_INT {$$  = define_INT_TYPE; ok = true;}
	| define_REAL {$$ = define_REAL_TYPE; ok = true;}
	| define_OPEN_BRACKET exp define_CLOSE_BRACKET {$$ = $2; ok = true;}
	;

terms : factors {$$ = $1;}
	 | terms define_MULT factors {
		if($1 == $3) {$$ = $1; ok = true;}
		else {handleTypeError($1, $3); ok = true;}
	 }
	 | terms define_DIV factors {
		if($1 == $3) {$$ = $1; ok = true;}
		else {handleTypeError($1, $3); ok = true;}
	 }
	 ;

read : define_READ define_OPEN_BRACKET identifierList define_CLOSE_BRACKET
	;

write : define_WRITE define_OPEN_BRACKET identifierList define_CLOSE_BRACKET
	;

for : define_FOR index_exp define_DO statement | define_BEGIN statementList define_END
	;

index_exp : identifier define_ASSIGN exp define_TO exp {
		string var($1);
		if(symbolTable.find(var) == symbolTable.end()) {
			char* newError;
			newError = (char *)malloc(sizeof(char)* 256);
			newError[0] = '\0';
			sprintf(newError,"Error at line %d, %s variable is not declared in the scope", (lc + 1), $1);
			addErrorMessage(newError);
		} else {
			if(symbolTable.find(var)->second.type != $3) {
				handleTypeError(symbolTable.find(var)->second.type, $3);
			} else if(symbolTable.find(var)->second.type != $5) {
				handleTypeError(symbolTable.find(var)->second.type, $5);
			} else {

			}
		} 
	}
	;

identifier : define_ID {
		string temp(yylval.string_val);
		for(int i = 0; i < KeyWords.size(); ++i) {
			// find if any reserved keyword matches
			// if match found then error
			if(temp == KeyWords[i]) {
				char* newError;
				newError = (char *)malloc(sizeof(char)* 256);
				newError[0] = '\0';
				sprintf(newError,"Error at line %d, variable %s cannot be a reserved keyword", (lc + 1), yylval.string_val);
				addErrorMessage(newError);
			}
		}
		// return the variable name
		$$ = yylval.string_val;
	}
	;
%%

