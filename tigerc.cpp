#include "tigerc.h"
#include "y.tab.h"

int lineno = 0;
node* root;

int main()
{
	root = new node;
	yyparse();
}