#include "ast.h"
#include "y.tab.h"
#include <stdio.h>

int lineno = 1;
Program* root; 

int main()
{
	yyparse();
	root->gen();
	module->dump();
}