%{
#include <cstdio>
#include <cstdlib>
#include "ast.h"
using namespace std;

extern Program* root;
int yylex();
extern int lineno;
extern char *yytext;
extern int status;

/* parse error */
void yyerror ( const char* s ) {
	printf("%s in line %d at '%s'\n", s,
         lineno , yytext);
	//exit(1);
	status = 0;
}

%}

%union {
	Program* t_program;
	structFields* t_fields;
	callerParams* t_params;
	arrayOperators* t_array;
	leftValue* t_lvalue;
	Expressions* t_exps;
	Expression* t_exp;
	Declarations* t_decs;
	Declaration* t_dec;
	Variable* t_vardec;
	typeFields* t_tyfields;
	Ident* t_id;
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

program		: exp { root = new Program($1); }
			;
fields		: { $$ = new structFields; }
			| id EQ exp { $$ = new structFields; $$->add(new structField($1, $3)); }
			| fields COMMA id EQ exp { $$ = $1; $1->add(new structField($3, $5)); }
			;
params		: { $$ = new callerParams; }
			| exp { $$ = new callerParams; $$->add(new callerParam($1)); }
			| params COMMA exp { $$ = $1; $1->add(new callerParam($3)); }
			;
array		: LBRACKET exp RBRACKET { $$ = new arrayOperators; $$->add(new arrayOperator($2)); }
			| array LBRACKET exp RBRACKET { $$ = $1; $$->add(new arrayOperator($3)); }
			;
lvalue		: id { $$ = new varLvalue($1); }
			| id array { $$ = new arrayLvalue(new varLvalue($1), $2); }
			| lvalue DOT id { $$ = new structLvalue($1, $3); }
			| lvalue DOT id array { $$ = new arrayLvalue(new structLvalue($1, $3), $4); }
			;
exps		: exps SEMICOLON exp { $$ = $1; $$->add($3); }
			| exp { $$ = new Expressions; $$->add($1); }
			| { $$ = new Expressions; }
			;
decs		: { $$ = new Declarations; }
			| decs dec { $$ = $1; $1->add($2); }
			;
dec			: TYPE id EQ id { $$ = new aliasType($2, $4); }
			| TYPE id EQ LBRACE tyfields RBRACE { $$ = new structType($2, $5); }
			| TYPE id EQ ARRAY OF id { $$ = new arrayType($2, $6); }
			| vardec { $$ = $1; }
			| FUNCTION id LPAREN tyfields RPAREN EQ exp { $$ = new funcDec($2, $4, $7); }
			| FUNCTION id LPAREN tyfields RPAREN COLON id EQ exp { $$ = new funcDec($2, $4, $7, $9); }
			| error { yyerror("Invalid declarartion"); $$ = new Declaration; }
			;
vardec		: VAR id ASSIGN exp { $$ = new Variable($2, $4); }
			| VAR id COLON id ASSIGN exp { $$ = new Variable($2, $4, $6); }
			;
tyfields	: { $$ = new typeFields; }
			| id COLON id { $$ = new typeFields; $$->add(new typeField($1, $3)); }
			| tyfields COMMA id COLON id { $$ = $1; $$->add(new typeField($3, $5)); }
			;
id			: ID { $$ = new Ident(string(yytext)); }
			;
exp	        : NIL { $$ = new nullConstant; }
			| INTEGERT { $$ = new intConstant(atoi(yytext)); }
			| STRINGT { string s = yytext; $$ = new stringConstant(s.substr(1, s.length()-2)); }
			| lvalue { $$ = $1; }
			| id array OF exp %prec HIGHER_THAN_OP { $$ = new arrayConstant($1, $2, $4); }
			| id LBRACE fields RBRACE { $$ = new structConstant($1, $3); }
			| id LPAREN params RPAREN { $$ = new functionCaller($1, $3); }
			| MINUS exp %prec UMINUS { $$ = new uniOperator("-", $2); }
			| exp PLUS exp { $$ = new binOperator("+", $1, $3); }
			| exp MINUS exp { $$ = new binOperator("-", $1, $3); }
			| exp STAR exp { $$ = new binOperator("*", $1, $3); }
			| exp SLASH exp { $$ = new binOperator("/", $1, $3); }
			| exp EQ exp { $$ = new binOperator("=", $1, $3); }
			| exp NEQ exp { $$ = new binOperator("<>", $1, $3); }
			| exp LT exp { $$ = new binOperator("<", $1, $3); }
			| exp GT exp { $$ = new binOperator(">", $1, $3); }
			| exp LE exp { $$ = new binOperator("<=", $1, $3); }
			| exp GE exp { $$ = new binOperator(">=", $1, $3); }
			| exp AND exp { $$ = new binOperator("&", $1, $3); }
			| exp OR exp { $$ = new binOperator("|", $1, $3); }
			| LPAREN exps RPAREN { $$ = $2; }
			| lvalue ASSIGN exp { $$ = new assignOperator($1, $3); }
			| IF exp THEN exp %prec LOWER_THAN_ELSE { $$ = new ifthenBlock($2, $4); }
			| IF exp THEN exp ELSE exp { $$ = new ifelseBlock($2, $4, $6); }
			| WHILE exp DO exp %prec HIGHER_THAN_OP { $$ = new whileBlock($2, $4); }
			| FOR id ASSIGN exp TO exp DO exp %prec HIGHER_THAN_OP { $$ = new forBlock($2, $4, $6, $8); }
			| BREAK { $$ = new breakLoop; }
			| LET decs IN exps END { $$ = new expBlock($2, $4); }
			| ERROR { $$ = new nullConstant; }
			| error { yyerror("Invalid expression");$$ = new nullConstant; }
			;

%%