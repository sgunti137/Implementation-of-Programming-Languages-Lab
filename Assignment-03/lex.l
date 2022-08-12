%{

#include <bits/stdc++.h>
using namespace std;

#define define_PROGRAM 1
#define define_VAR 2
#define define_BEGIN 3
#define define_END 4
#define define_END_DOT 5
#define define_INTEGER 6
#define define_FOR 7
#define define_READ 8
#define define_WRITE 9
#define define_TO 10
#define define_DO 11
#define define_SEMICOLON 12
#define define_COLON 13
#define define_COMMA 14
#define define_ASSIGN 15
#define define_PLUS 16
#define define_MINUS 17
#define define_MULT 18
#define define_DIV 19
#define define_OPEN_BRACKET 20
#define define_CLOSE_BRACKET 21
#define define_ID 22
#define define_INT 23

typedef union {
	char *str;
	int num;
} records;

int lc = 1;
records record;
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

hashTable<string, int> Hash;
%}

%option noyywrap

%%

PROGRAM {
  return define_PROGRAM;
}

VAR {
  return define_VAR;
}

BEGIN {
  return define_BEGIN;
}

END {
  return define_END;
}

"END." {
  return define_END_DOT;
}

INTEGER {
  return define_INTEGER;
}

FOR {
  return define_FOR;
}

READ {
  return define_READ;
}

WRITE {
  return define_WRITE;
}

TO {
  return define_TO;
}

DO {
  return define_DO;
}

";" {
  return define_SEMICOLON;
}

":" {
  return define_COLON;
}

"," {
  return define_COMMA;
}

":=" {
  return define_ASSIGN;
}

"+" {
  return define_PLUS;
}

"-" {
  return define_MINUS;
}

"*" {
  return define_MULT;
}

DIV {
  return define_DIV;
}

"(" {
  return define_OPEN_BRACKET;
}

")" {
  return define_CLOSE_BRACKET;
}

[A-Z]([A-Z0-9])* {
  record.str = (char *)malloc(strlen(yytext) + 1);
  strcpy(record.str, yytext);
  Hash.install_id(yytext);
  return define_ID;
}

[0-9]+ {
  record.num = atoi(yytext);
  Hash.install_num(yytext);
  return define_INT;
}
[\t] {}

" " {}

"\n" {
  lc++;
}
. {}

%%

int main() {
  int temp = 0;
  yyin = fopen("input.txt", "r");
	if(!yyin) {
		return 1;
	}

	cout << "Line    Token    Token Specifier" << endl;
	while(temp=yylex()) {
		switch(temp) {
			case define_ID:
				cout << lc << "        22      ^" << record.str << endl;
				free((void *)record.str);
				break;
			case define_INT: 
				cout << lc << "        23      #" << record.num << endl;
				break;
			default:
				cout << lc << "        " << temp << endl;
				break;
		}
	}
  return 0;
}