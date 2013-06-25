#include "tigerc.h"
#include "y.tab.h"
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <fstream>

int lineno = 1;
int nodecount = 0;
int parent[10000];
string label[10000];
node* root;
extern void yyrestart(FILE*);

void preorder(node* n, int dep, int fa)
{
	nodecount++;
	parent[nodecount] = fa;
	label[nodecount] = n->comment.c_str();
	int now = nodecount;
	for(int i = 0; i < dep; i++) printf("  ");
	printf("%s", n->comment.c_str());
	if(n->childs.size()){
		printf(" {\n");
		for(int i = 0; i < n->childs.size(); i++)
			preorder(n->childs[i], dep + 1, now);
		for(int i = 0; i < dep; i++) printf("  ");
		printf("}\n");
	}else printf("\n");
}

void orz(char* c)
{
	printf("%s", c);
	fflush(stdout);
}

int main(int argc, char* argv[])
{
	if(argc>1) {
		FILE* f = fopen(argv[1], "r");
		yyrestart(f);
	}
	root = new node;
	yyparse();
	preorder(root, 0, 0);
	
	for(int i=1; i<=nodecount; i++)
		if (label[i][0]=='"')
			label[i]=label[i].substr(1,label[i].length()-2);

	ofstream fout("treeplot.m");
	fout<<"(* ::Package:: *)\n\nTreePlot[{";
	for(int i=2; i<nodecount; i++)
		fout<<'"'<<parent[i]<<':'<<label[parent[i]]<<'"'<<"->"<<'"'<<i<<':'<<label[i]<<'"'<<',';
	fout<<'"'<<parent[nodecount]<<':'<<label[parent[nodecount]]<<'"'<<"->"<<'"'<<nodecount<<':'<<label[nodecount]<<'"';
	fout<<"},Top,\"1:program\",VertexLabeling->True,DirectedEdges->True,ImageSize->3000]"<<endl;
	fout.close();
}