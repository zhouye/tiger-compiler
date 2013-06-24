#ifndef __AST_H
#define __AST_H

#include <llvm/Instructions.h>
#include <llvm/CallingConv.h>
#include <llvm/Bitcode/ReaderWriter.h>
#include <llvm/Analysis/Verifier.h>
#include <llvm/Assembly/PrintModulePass.h>
#include <llvm/Support/IRBuilder.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/ExecutionEngine/GenericValue.h>
#include <llvm/ExecutionEngine/JIT.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Analysis/Passes.h>
#include <llvm/Analysis/Verifier.h>
#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/JIT.h>
#include <llvm/DerivedTypes.h>
#include <llvm/Support/IRBuilder.h>
#include <llvm/LLVMContext.h>
#include <llvm/Module.h>
#include <llvm/PassManager.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Bitcode/ReaderWriter.h>
#include <llvm/Transforms/Scalar.h>
#include <cstdio>
#include <map>
#include <string>
#include <vector>

using namespace std;
using namespace llvm;

extern Module *module;

struct node
{
	node* father;
	virtual void gen();
};

struct Ident : public node
{
	string str;
	Ident(string _str) : str(_str) {}
	operator string() { return str; }
	string s() { return str; }
	virtual void gen();
};

struct Declaration : public node
{
	virtual void gen();
};

struct typeDec : public Declaration
{
	Ident* id;
	Type* type;
	typeDec(Ident* _id) : id(_id) { if (_id!=NULL) _id->father=this;}
	virtual void gen();
};

struct aliasType : public typeDec
{
	Ident* content;
	aliasType(Ident* _id, Ident* _content) : typeDec(_id), content(_content) 
		{ if (_id!=NULL) _id->father=this; if (_content!=NULL) _content->father=this;}
	virtual void gen();
};

struct arrayType : public typeDec
{
	Ident* content;
	Type* contentType;
	arrayType(Ident* _id, Ident* _content) : typeDec(_id), content(_content) 
		{ if (_id!=NULL) _id->father=this; if (_content!=NULL) _content->father=this;}
	virtual void gen();
};

struct typeField : public node
{
	Ident* id;
	Ident* type;
	typeField(Ident* _id, Ident* _type) : id(_id), type(_type) 
		{ if (_id!=NULL) _id->father=this; if (_type!=NULL) _type->father=this;}
	virtual void gen();
};

struct typeFields : public node
{
	vector<typeField*> vec;
	void add(typeField* s) { vec.push_back(s); if (s!=NULL) s->father=this;}
	virtual void gen();
};

struct structType : public typeDec
{
	typeFields* fields;
	Type* type;
	structType(Ident* _id, typeFields* _fields) : typeDec(_id), fields(_fields) 
		{ if (_id!=NULL) _id->father=this; if (_fields!=NULL) _fields->father=this;}
	virtual void gen();
};

struct Declarations : public node
{
	vector<Declaration*> vec;
	void add(Declaration* s) { vec.push_back(s); if (s!=NULL) s->father=this;}
	virtual void gen();
};

struct Expression : public node
{
	Value* v;
	Expression() : v(0) {}
	virtual void gen();
};

struct Expressions : public Expression
{
	vector<Expression*> vec;
	void add(Expression* s) { vec.push_back(s); if (s!=NULL) s->father=this;}
	virtual void gen();
};

struct expBlock : public Expression
{
	Declarations* decs;
	Expression* exp;
	expBlock(Declarations* _decs, Expression* _exp) : decs(_decs), exp(_exp) 
		{ if (_decs!=NULL) _decs->father=this; if (_exp!=NULL) _exp->father=this;}
	virtual void gen();
};

struct funcDec : public Declaration
{
	Ident* id;
	typeFields* params;
	Ident* ret;
	Expression* exp;
	funcDec(Ident* _id, typeFields* _params, Expression* _exp) : id(_id), params(_params), ret(0), exp(_exp) 
		{ if (_id!=NULL) _id->father=this; if (_params!=NULL) _params->father=this; if (_exp!=NULL) _exp->father=this;}
	funcDec(Ident* _id, typeFields* _params, Ident* _ret, Expression* _exp) : id(_id), params(_params), ret(_ret), exp(_exp) 
		{ if (_id!=NULL) _id->father=this; if (_params!=NULL) _params->father=this; if (_ret!=NULL) _ret->father=this; if (_exp->father!=NULL) _exp->father=this;} 
	Function* f;
	virtual void gen();
};

struct Variable : public Declaration
{
	Ident* id;
	Ident* type;
	Expression* exp;
	Value* v;
	Variable(Ident* _id, Ident* _type, Expression* _exp) : id(_id), type(_type), exp(_exp) 
		{ if (_id!=NULL) _id->father=this; if (_type!=NULL) _type->father=this; if (_exp!=NULL) _exp->father=this;}
	Variable(Ident* _id, Expression* _exp) : id(_id), type(0), exp(_exp) 
		{ if (_id!=NULL) _id->father=this; if (_exp!=NULL) _exp->father=this;}
	virtual void gen();
};

struct Program : public node
{
	Function *mainFunc;
	Expression* exp;
	Program(Expression* _exp) : exp(_exp) { if (_exp!=NULL) _exp->father=this;}
	virtual void gen();
};

struct constValue : public Expression
{
	virtual void gen();
};

struct nullConstant : public constValue
{
	nullConstant() {}
	virtual void gen();
};

struct intConstant : public constValue
{
	int i;
	intConstant(int _i) : i(_i) {}
	virtual void gen();
};

struct stringConstant : public constValue
{
	string s;
	stringConstant(string _s) : s(_s) {}
	virtual void gen();
};

struct uniOperator : public Expression
{
	string op;
	Expression* exp;
	uniOperator(string _op, Expression* _exp) : op(_op), exp(_exp) { if (_exp!=NULL) _exp->father=this;}
	virtual void gen();
};

struct binOperator : public Expression
{
	string op;
	Expression* expa;
	Expression* expb;
	binOperator(string _op, Expression* _expa, Expression* _expb) : op(_op), expa(_expa), expb(_expb) 
		{ if (_expa!=NULL) _expa->father=this; if (_expb!=NULL) _expb->father=this;}
	virtual void gen();
};

struct leftValue : public Expression
{
	Value* lv;
	virtual void gen();
};

struct assignOperator : public Expression
{
	leftValue* lvalue;
	Expression* exp;
	assignOperator(leftValue* _lvalue, Expression* _exp) : lvalue(_lvalue), exp(_exp) 
		{ if (_lvalue!=NULL) _lvalue->father=this; if (_exp!=NULL) _exp->father=this;}
	virtual void gen();
};

struct ifthenBlock : public Expression
{
	Expression* cond;
	Expression* thenBlock;
	ifthenBlock(Expression* _cond, Expression* _thenBlock) : cond(_cond), thenBlock(_thenBlock) 
		{ if (_cond!=NULL) _cond->father=this; if (_thenBlock!=NULL) _thenBlock->father=this;}
	virtual void gen();
};

struct ifelseBlock : public Expression
{
	Expression* cond;
	Expression* thenBlock;
	Expression* elseBlock;
	ifelseBlock(Expression* _cond, Expression* _thenBlock, Expression* _elseBlock) : 
		cond(_cond), thenBlock(_thenBlock), elseBlock(_elseBlock) 
		{ if (_cond!=NULL) _cond->father=this; if (_thenBlock!=NULL) _thenBlock->father=this; if (_elseBlock!=NULL) _elseBlock->father=this;}
	virtual void gen();
};

struct forBlock : public Expression
{
	Ident* id;
	Expression* loopFrom;
	Expression* loopTo;
	Expression* loopBlock;
	forBlock(Ident* _id, Expression* _loopFrom, Expression* _loopTo, Expression* _loopBlock) : 
		id(_id), loopFrom(_loopFrom), loopTo(_loopTo), loopBlock(_loopBlock) 
		{ if (_id!=NULL) _id->father=this; if (_loopFrom!=NULL) _loopFrom->father=this; if (_loopTo!=NULL) _loopTo->father=this; if (_loopBlock!=NULL) _loopBlock->father=this;}
	virtual void gen();
};

struct whileBlock : public Expression
{
	Expression* cond;
	Expression* loopBlock;
	whileBlock(Expression* _cond, Expression* _loopBlock) : cond(_cond), loopBlock(_loopBlock) 
		{ if (_cond!=NULL) _cond->father=this; if (_loopBlock!=NULL) _loopBlock->father=this;}
	virtual void gen();
};

struct breakLoop : public Expression
{
	virtual void gen();
};

struct callerParam : public node
{
	Expression* exp;
	Value* v;
	callerParam(Expression* _exp) : exp(_exp) { if (_exp!=NULL) _exp->father=this;}
	virtual void gen();
};

struct callerParams : public node
{
	vector<callerParam*> vec;
	void add(callerParam* s) { vec.push_back(s); if (s!=NULL) s->father=this;}
	virtual void gen();
};

struct functionCaller : public Expression
{
	Ident* id;
	callerParams* params;
	functionCaller(Ident* _id, callerParams* _params) : id(_id), params(_params) 
		{ if (_id!=NULL) _id->father=this; if (_params!=NULL) _params->father=this;}
	virtual void gen();
};

struct arrayOperator : public node
{
	Value* v;
	Expression* exp;
	arrayOperator(Expression* _exp) : exp(_exp) { if (_exp!=NULL) _exp->father=this;} 
	virtual void gen();
};

struct arrayOperators : public node
{
	vector<arrayOperator*> vec;
	void add(arrayOperator* s) { vec.push_back(s); if (s!=NULL) s->father=this;}
	virtual void gen();
};

struct structField : public node
{
	Ident* id;
	Expression* exp;
	structField(Ident* _id, Expression* _exp) : id(_id), exp(_exp) 
		{ if (_id!=NULL) _id->father=this; if (_exp!=NULL) _exp->father=this;}
	virtual void gen();
};

struct structFields : public node
{
	vector<structField*> vec;
	void add(structField* s) { vec.push_back(s); if (s!=NULL) s->father=this;}
	virtual void gen();
};

struct structConstant : public constValue
{
	Ident* id;
	structFields* fields;
	structConstant(Ident* _id, structFields* _fields) : id(_id), fields(_fields) 
		{ if (_id!=NULL) _id->father=this; if (_fields!=NULL) _fields->father=this; }
	virtual void gen();
};

struct arrayLvalue : public leftValue
{
	leftValue* id;
	arrayOperators* arr;
	arrayLvalue(leftValue* _id, arrayOperators* _arr) : id(_id), arr(_arr) 
		{ if (_id!=NULL) _id->father=this; if (_arr!=NULL) _arr->father=this;}
	virtual void gen();
};

struct arrayConstant : public constValue
{
	Ident* type;
	arrayOperators* size;
	Expression* exp;
	arrayConstant(Ident* _type, arrayOperators* _size, Expression* _exp) : type(_type), size(_size), exp(_exp) 
		{ if (_type!=NULL) _type->father=this; if (_size!=NULL) _size->father=this; if (_exp!=NULL) _exp->father=this;}
	virtual void gen();
};

struct varLvalue : public leftValue
{
	Ident* id;
	varLvalue(Ident* _id) : id(_id) { if (_id!=NULL) _id->father=this;}
	virtual void gen();
};

struct structLvalue : public leftValue
{
	leftValue* id;
	Ident* field;
	structLvalue(leftValue* _id, Ident* _field) : id(_id), field(_field) 
		{ if (_id!=NULL) _id->father=this; if (_field!=NULL) _field->father=this;}
	virtual void gen();
};

#endif