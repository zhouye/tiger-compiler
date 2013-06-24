%{
  #include "ast.h"
  #include <stdlib.h>
  #include "y.tab.h"
  extern int lineno;
  int yycolumn = 1;
  int nestcomment = 0;
  void yyerror(const char *);
%}
%option noyywrap
%x COMMENT

%%
<*>"/*" { 
  BEGIN(COMMENT);
  nestcomment++;
  yymore(); 
}

<COMMENT>"*/" { 
  nestcomment--;
  if (nestcomment==0)
    BEGIN(INITIAL); 
}

<COMMENT><<EOF>> { 
  yyerror("Unclosed comment.\n");
  return ERROR; 
}
<COMMENT>. {
  yymore();
}
<COMMENT>"\n\r"|"\r\n"|"\r"|"\n" {
  yycolumn = 1;
  lineno++;
  yymore();
}

[ ]+ ;

<*>[\t] {
}

"array"      return ARRAY;
"if"         return IF;
"then"       return THEN;
"else"       return ELSE;
"while"      return WHILE;
"for"        return FOR;
"to"         return TO;
"do"         return DO;
"let"        return LET;
"in"         return IN;
"end"        return END;
"of"         return OF;
"break"      return BREAK;
"nil"        return NIL;
"function"   return FUNCTION;
"var"        return VAR;
"type"       return TYPE;

","	return COMMA;
":"	return COLON;
";"	return SEMICOLON;
"("	return LPAREN;
")"	return RPAREN;
"["	return LBRACKET;
"]"	return RBRACKET;
"{"	return LBRACE;
"}"	return RBRACE;
"."	return DOT;
"+"	return PLUS;
"-"	return MINUS;
"*"	return STAR;
"/"	return SLASH;
"="	return EQ;
"<>" return NEQ;
"<"	return LT;
"<=" return LE;
">"	return GT;
">=" return GE;
"&" return AND;
"|" return OR;
":=" return ASSIGN;

<*>"\n\r"|"\r\n"|"\r"|"\n" {
  yycolumn = 1;
  lineno++;
}

\"[^\"]*\" {
  if(yyleng>257){
    yyerror("String too long");
    return ERROR;
  }

  int i=1;
  int j;
  int len=yyleng-1;
  while(i<len)
  {
	if (yytext[i]=='\\')
	{ 
	  if (i+3<len && yytext[i+1]>='0' && yytext[i+1]<='3' 
	                   && yytext[i+2]>='0' && yytext[i+2]<='7'
					   && yytext[i+3]>='0' && yytext[i+3]<='7') {
		yytext[i]=64*(yytext[i+1]-'0')+8*(yytext[i+2]-'0')+(yytext[i+3]-'0');
		for (j=i+1;j<len-2;j++)
		  yytext[j]=yytext[j+3];
		yytext[len-2]=0;yytext[len-1]=0;yytext[len]=0;
		len=len-3;
	  }
      else if (i+3<len && (yytext[i+1]=='x')
	                        && ((yytext[i+2]>='0' && yytext[i+2]<='9') || 
	                          (yytext[i+2]>='A' && yytext[i+2]<='F') || 
							  (yytext[i+2]>='a' && yytext[i+2]<='f')) 
	                        && ((yytext[i+3]>='0' && yytext[i+3]<='9') || 
						      (yytext[i+3]>='A' && yytext[i+3]<='F') || 
							  (yytext[i+3]>='a' && yytext[i+3]<='f'))) {
		yytext[i]=0;
		if ((yytext[i+2]>='0') && (yytext[i+2]<='9'))
		  yytext[i]+=16*(yytext[i+2]-'0');
		else if ((yytext[i+2]>='A') && (yytext[i+2]<='F'))
		  yytext[i]+=16*(yytext[i+2]-'A');
		else
		  yytext[i]+=16*(yytext[i+2]-'a');		
		if ((yytext[i+3]>='0') && (yytext[i+3]<='9'))
		  yytext[i]+=(yytext[i+3]-'0');
		else if ((yytext[i+3]>='A') && (yytext[i+3]<='F'))
		  yytext[i]+=(yytext[i+3]-'A');
		else
		  yytext[i]+=16*(yytext[i+3]-'a');
		for (j=i+1;j<len-2;j++)
		  yytext[j]=yytext[j+3];
		yytext[len-2]=0;yytext[len-1]=0;yytext[len]=0;
		len=len-3;							  
	  }
	  else if(i+1<len && (yytext[i+1]=='a' || yytext[i+1]=='b' || yytext[i+1]=='f' || yytext[i+1]=='n' ||
	 		                   yytext[i+1]=='r' || yytext[i+1]=='t' || yytext[i+1]=='v' ||
			                   yytext[i+1]=='\\' || yytext[i+1]=='"' )) {
		if (yytext[i+1]=='a') yytext[i]=7;
		if (yytext[i+1]=='b') yytext[i]=8;
		if (yytext[i+1]=='f') yytext[i]=12;
		if (yytext[i+1]=='n') yytext[i]=10;
		if (yytext[i+1]=='r') yytext[i]=13;
		if (yytext[i+1]=='t') yytext[i]=9;
		if (yytext[i+1]=='\\') yytext[i]=92;
		if (yytext[i+1]=='"') yytext[i]=34;
		for (j=i+1;j<len;j++)
			yytext[j]=yytext[j+1];
		yytext[len]=0;
		len--;
	  }
	  else
	    yyerror("Invalid escape sequence");
	}
	i++;
  }
  return STRINGT;
}

\"[^\"]* {
  yyerror("Unclosed string");
  return 0;
}

[a-zA-Z][a-zA-Z_0-9]*|"_main" {
  if(yyleng>255){
    yyerror("Identifier too long");
    return ERROR;
  }
  return ID;
}

[0-9]+ {
  if(yyleng>10||(yyleng==10&&(strcmp(yytext,"2147483647")>0))){
    yyerror("Integer out of range");
    return ERROR;                  
  } 
  return INTEGERT;
}

<<EOF>> return 0;

. {
    yyerror("Unrecognized Character");
  }

%%

