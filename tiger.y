%{
#include <cstdio>
#include <cstdlib>
#include "tigerc.h"
using namespace std;

extern node* root;
int yylex();
extern int lineno;
extern char *yytext;

/* parse error */
void yyerror ( const char* s ) {
	printf("\n*** %s (line : %d, token: '%s')\n",
         s, lineno + 1, yytext);
	exit(1);
}

%}

%union {
	Program t_program;
	Fields t_fields;
	Params t_params;
	Array t_array;
	Lvalue t_lvalue;
	Exps t_exps;
	Decs t_decs;
	Dec t_dec;
	Vardec t_vardec;
	Ty t_ty;
	Tyfields t_tyfields;
	Id t_id;
	Exp t_exp;
	char* str;
	int token;
}

%type <t_program> program
%type <t_exp> exp
%type <t_fields> fields
%type <t_params> params
%type <t_array> array
%type <t_lvalue> lvalue
%type <t_exps> exps
%type <t_decs> decs
%type <t_dec> dec
%type <t_vardec> vardec
%type <t_ty> ty
%type <t_tyfields> tyfields
%type <t_id> id

%token <token>	ARRAY IF THEN ELSE WHILE FOR TO DO LET IN END OF FUNCTION VAR TYPE ERROR
				COMMA COLON SEMICOLON LPAREN RPAREN LBRACKET RBRACKET LBRACE RBRACE 
				DOT PLUS MINUS STAR SLASH EQ NEQ GT GE LT LE AND OR ASSIGN
				ID STRINGT INTEGERT NIL BREAK

%nonassoc	ASSIGN
%nonassoc	LOWER_THAN_OP
%left		OR
%left		AND
%nonassoc	EQ NEQ LT LE GT GE
%left		PLUS MINUS
%left		STAR SLASH
%nonassoc	UMINUS
%left		DOT
%nonassoc	LOWER_THAN_OF
%nonassoc	HIGHER_THAN_OP
%nonassoc	LOWER_THAN_ELSE
%nonassoc	ELSE

%%

program		: exp { root->comment = "program"; root->add($1); }
			| decs { root->comment = "header"; root->add($1); }
			;
fields		: { $$ = new node("fields"); }
			| id EQ exp { $$ = new node("fields"); Fields n = new node("EQ"); n->add($1); n->add($3); $$->add(n); }
			| fields COMMA id EQ exp { $$ = $1; Fields n = new node("EQ"); n->add($3); n->add($5); $1->add(n); }
			;
params		: { $$ = new node("params"); }
			| exp { $$ = new node("params"); $$->add($1); }
			| params COMMA exp { $$ = $1; $1->add($3); }
			;
array		: LBRACKET exp RBRACKET { $$ = $2; }
			| array LBRACKET exp RBRACKET { $$ = new node("op[]"); $$->add($1); $$->add($3); }
			;
lvalue		: id { $$ = new node("lvalue"); $$->add($1); }
			| id array { $$ = new node("op[]"); $$->add($1); $$->add($2); }
			| lvalue DOT id { $$ = new node("DOT"); $$->add($1); $$->add($3);}
			| lvalue DOT id array { $$ = new node("op[]"); Lvalue n = new node("DOT"); n->add($1); n->add($3); $$->add(n); $$->add($3); }
			;
exps		: exps SEMICOLON exp { $$ = $1; $$->add($3); }
			| exp { $$ = new node("exps"); $$->add($1); }
			| { $$ = new node("exps"); }
			;
decs		: { $$ = new node("decs"); }
			| decs dec { $$ = $1; $1->add($2); }
			;
dec			: TYPE id EQ ty { $$ = new node("typedef"); $$->add($2); $$->add($4); }
			| vardec { $$ = $1; }
			| FUNCTION id LPAREN tyfields RPAREN EQ exp { $$ = new node("function"); $$->add($2); $$->add($4); $$->add($7); }
			| FUNCTION id LPAREN tyfields RPAREN COLON id EQ exp { $$ = new node("function"); $$->add($2); $$->add($4); $$->add($7); $$->add($9); }
			;
vardec		: VAR id ASSIGN exp { $$ = new node("vardef"); $$->add($2); $$->add($4); }
			| VAR id COLON id ASSIGN exp { $$ = new node("vardef"); $$->add($2); $$->add($4); $$->add($6); }
			;
ty			: id { $$ = $1; }
			| LBRACE tyfields RBRACE { $$ = $2; }
			| ARRAY OF id { $$ = new node("arrayof"); $$->add($3); }
			;
tyfields	: id COLON id { $$ = new node("fields"); Tyfields n = new node("type"); n->add($1); n->add($3); $$->add(n); }
			| tyfields COMMA id COLON id { $$ = $1; Tyfields n = new node("type"); n->add($3); n->add($5); $$->add(n); }
			;
id			: ID { $$ = new node(yytext); }
			;
exp			: NIL { $$ = new node($1); }
			| INTEGERT { $$ = new node(yytext); }
			| STRINGT { $$ = new node(yytext); }
			| lvalue { $$ = $1; }
			| id array OF exp %prec HIGHER_THAN_OP { $$ = new node("constarr"); Lvalue n = new node("op[]"); n->add($1); n->add($2); $$->add(n); $$->add($4); }
			| id LBRACE fields RBRACE { $$ = new node("fields"); $$->add($1); $$->add($3); }
			| id LPAREN params RPAREN { $$ = new node("params"); $$->add($1); $$->add($3); }
			| MINUS exp %prec UMINUS { $$ = new node("-"); $$->add($2); }
			| exp PLUS exp { $$ = new node("+"); $$->add($1); $$->add($3); }
			| exp MINUS exp { $$ = new node("-"); $$->add($1); $$->add($3); }
			| exp STAR exp { $$ = new node("*"); $$->add($1); $$->add($3); }
			| exp SLASH exp { $$ = new node("/"); $$->add($1); $$->add($3); }
			| exp EQ exp { $$ = new node("="); $$->add($1); $$->add($3); }
			| exp NEQ exp { $$ = new node("<>"); $$->add($1); $$->add($3); }
			| exp LT exp { $$ = new node("<"); $$->add($1); $$->add($3); }
			| exp GT exp { $$ = new node(">"); $$->add($1); $$->add($3); }
			| exp LE exp { $$ = new node("<="); $$->add($1); $$->add($3); }
			| exp GE exp { $$ = new node(">="); $$->add($1); $$->add($3); }
			| exp AND exp { $$ = new node("&"); $$->add($1); $$->add($3); }
			| exp OR exp { $$ = new node("|"); $$->add($1); $$->add($3); }
			| LPAREN exps RPAREN { $$ = new node("paren"); $$->add($2); }
			| lvalue ASSIGN exp { $$ = new node("assign"); $$->add($1); $$->add($3); }
			| IF exp THEN exp %prec LOWER_THAN_ELSE { $$ = new node("if"); $$->add($2); $$->add($4); }
			| IF exp THEN exp ELSE exp { $$ = new node("if-else"); $$->add($2); $$->add($4); $$->add($6); }
			| WHILE exp DO exp %prec HIGHER_THAN_OP { $$ = new node("while"); $$->add($2); $$->add($4); }
			| FOR id ASSIGN exp TO exp DO exp %prec HIGHER_THAN_OP { $$ = new node("for"); Exp n = new node("cond"); n->add($2); n->add($4); n->add($6); $$->add(n); $$->add($8); }
			| BREAK { $$ = new node($1); }
			| LET decs IN exps END { $$ = new node("block"); $$->add($2); $$->add($4); }
			;

%%