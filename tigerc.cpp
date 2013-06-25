#include "ast.h"
#include "y.tab.h"
#include <iostream>
#include <fstream>
#include <string>
#include <llvm/ExecutionEngine/GenericValue.h>
#include <llvm/ExecutionEngine/JIT.h>

using namespace std;
int lineno = 1;
Program* root; 
int status = 1;
extern void yyrestart(FILE*);

int main(int argc, char* argv[])
{
	if(argc>1) {
		FILE* f = fopen(argv[1], "r");
		yyrestart(f);
	}
	yyparse();
	if (status) {
	InitializeNativeTarget();
	try{
		root->gen();
	}
	catch (string &e){
		cout<<e<<"\n";
		exit(1);
	}
	PassManager pm;
	string s;
	raw_string_ostream* ost = new raw_string_ostream(s);
	pm.add(createBasicAliasAnalysisPass());
	pm.add(createPromoteMemoryToRegisterPass());
	pm.add(createInstructionCombiningPass());
	pm.add(createReassociatePass());
	pm.add(createGVNPass());
	pm.add(createCFGSimplificationPass());
	if(argc>2) pm.add(createPrintModulePass(ost));
	else pm.add(createPrintModulePass(&outs()));
	pm.run(*module);
	if(argc>2) {
		ofstream fout(argv[2]);
		fout << s;
		fout.close();
	}
	std::cout << "Running code...\n";
	ExecutionEngine *ee = EngineBuilder(module).create();
	vector<GenericValue> noargs;
	GenericValue v = ee->runFunction(root->mainFunc, noargs);
	cout << endl;
	//cout << endl << v.IntVal.getSExtValue() << endl; 
	}
}