#ifndef __TIGERC_H
#define __TIGERC_H
#include <vector>
#include <string>
#include <sstream>
#include <stack>
#include <typeinfo>

using namespace std;

struct node {
	string comment;
	vector<node*> childs;
	node() {}
	node(const string s) { comment = s; }
	node(const string* s) { comment = *s; }
	node(const char* s) { comment = string(s); }
	node(const int token) { ostringstream ost; ost << "Token" << token; comment = ost.str(); }
	void add(node* s) { childs.push_back(s); }
};
	
typedef node* Program;
typedef node* Exp;
typedef node* Fields;
typedef node* Params;
typedef node* Array;
typedef node* Lvalue;
typedef node* Exps;
typedef node* Decs;
typedef node* Dec;
typedef node* Vardec;
typedef node* Ty;
typedef node* Tyfields;
typedef node* Typeid;
typedef node* Id;

#endif