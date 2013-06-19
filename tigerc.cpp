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

int main()
{
	root = new node;
	yyparse();
	preorder(root, 0);
}