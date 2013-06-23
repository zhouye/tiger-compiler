#include "ast.h"
#include "y.tab.h"
#include <iostream>
#include <llvm/ExecutionEngine/GenericValue.h>
#include <llvm/ExecutionEngine/JIT.h>

using namespace std;
int lineno = 1;
Program* root; 

int main()
{
	yyparse();
	InitializeNativeTarget();
	root->gen();
	PassManager pm;
	pm.add(createPrintModulePass(&outs()));
	pm.run(*module);
	std::cout << "Running code...\n";
	ExecutionEngine *ee = EngineBuilder(module).create();
	vector<GenericValue> noargs;
	GenericValue v = ee->runFunction(root->mainFunc, noargs);
	cout << endl;
	//cout << endl << v.IntVal.getSExtValue() << endl; 
}