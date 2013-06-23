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
	root->gen();
	module->dump();
	std::cout << "Running code...\n";
	ExecutionEngine *ee = EngineBuilder(module).create();
	vector<GenericValue> noargs;
	GenericValue v = ee->runFunction(root->mainFunc, noargs);
}