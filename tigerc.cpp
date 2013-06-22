#include "ast.h"
#include "y.tab.h"
#include <stdio.h>

int lineno = 1;
Program* root; 

int main()
{
	/*LLVMContext& context = getGlobalContext();
	Module *module = new Module("top", context);
	IRBuilder<> x(context); 
	FunctionType *funcType = FunctionType::get(x.getInt32Ty(), false);
	Function *mainFunc = Function::Create(funcType, llvm::Function::ExternalLinkage, "main", module);
	BasicBlock *entry = BasicBlock::Create(context, "", mainFunc);
	x.SetInsertPoint(entry);
	b = new IRBuilder<>(entry);
	yyparse();
	x.CreateRet(ConstantInt::get(b->getInt32Ty(), 0, true));
	module->dump();*/
	yyparse();
	root->gen();
}