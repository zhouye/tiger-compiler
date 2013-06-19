#include "tigerc.h"
#include "y.tab.h"
#include <stdio.h>

int lineno = 1;
node* root;

void preorder(node* n, int dep)
{
	for(int i = 0; i < dep; i++) printf("  ");
	printf("%s", n->comment.c_str());
	if(n->childs.size()){
		printf(" {\n");
		for(int i = 0; i < n->childs.size(); i++)
			preorder(n->childs[i], dep + 1);
		for(int i = 0; i < dep; i++) printf("  ");
		printf("}\n");
	}else printf("\n");
}

void orz(char* c)
{
	printf("%s", c);
	fflush(stdout);
}

IRBuilder<> *b; 

int main()
{
	root = new node;
	//preorder(root, 0);
	
	LLVMContext& context = getGlobalContext();
	Module *module = new Module("top", context);
	IRBuilder<> x(context); 
	FunctionType *funcType = FunctionType::get(x.getInt32Ty(), false);
	Function *mainFunc = Function::Create(funcType, llvm::Function::ExternalLinkage, "main", module);
	BasicBlock *entry = BasicBlock::Create(context, "", mainFunc);
	x.SetInsertPoint(entry);
	b = new IRBuilder<>(entry);
	yyparse();
	x.CreateRet(ConstantInt::get(b->getInt32Ty(), 0, true));
	module->dump();
}