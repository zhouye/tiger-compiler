#ifndef __TIGERC_H
#define __TIGERC_H
#include <vector>
#include <string>
#include <sstream>
#include <stack>
#include <typeinfo>
#include <llvm/Module.h>
#include <llvm/Function.h>
#include <llvm/Constants.h>
#include <llvm/Type.h>
#include <llvm/DerivedTypes.h>
#include <llvm/LLVMContext.h>
#include <llvm/PassManager.h>
#include <llvm/Instructions.h>
#include <llvm/CallingConv.h>
#include <llvm/Value.h>
#include <llvm/Bitcode/ReaderWriter.h>
#include <llvm/Analysis/Verifier.h>
#include <llvm/Assembly/PrintModulePass.h>
#include <llvm/Support/IRBuilder.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/ExecutionEngine/GenericValue.h>
//#include <llvm/ExecutionEngine/JIT.h>
#include <llvm/Support/raw_ostream.h>

using namespace llvm;
using namespace std;

struct node {
	string comment;
	vector<node*> childs;
	Value* v;
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