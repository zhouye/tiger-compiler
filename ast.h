#ifndef __AST_H
#define __AST_H

#include "llvm/Analysis/Passes.h"
#include "llvm/Analysis/Verifier.h"
#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/ExecutionEngine/JIT.h"
#include "llvm/DerivedTypes.h"
#include "llvm/Support/IRBuilder.h"
#include "llvm/LLVMContext.h"
#include "llvm/Module.h"
#include "llvm/PassManager.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Transforms/Scalar.h"
#include <cstdio>
#include <map>
#include <string>
#include <vector>

using namespace std;

struct node
{
	virtual void gen();
};

struct Ident : public node
{
	string* str;
	Ident(string* _str) : str(_str) {}
	virtual void gen();
};

struct Declaration : public node
{
	virtual void gen();
};

struct Type : public Declaration
{
	Ident* id;
	virtual void gen();
};

struct aliasType : public Type
{
	Ident* id;
	Ident* content;
	aliasType(Ident* _id, Ident* _content) : id(_id), content(_content) {}
	virtual void gen();
};

struct arrayType : public Type
{
	Ident* id;
	Ident* content;
	arrayType(Ident* _id, Ident* _content) : id(_id), content(_content) {}
	virtual void gen();
};

struct typeField : public node
{
	Ident* id;
	Ident* type;
	typeField(Ident* _id, Ident* _type) : id(_id), type(_type) {}
	virtual void gen();
};

struct typeFields : public node
{
	vector<typeField*> vec;
	void add(typeField* s) { vec.push_back(s); }
	virtual void gen();
};

struct structType : public Type
{
	Ident* id;
	typeFields* fields;
	structType(Ident* _id, typeFields* _fields) : id(_id), fields(_fields) {}
	virtual void gen();
};

struct Declarations : public node
{
	vector<Declaration*> vec;
	void add(Declaration* s) { vec.push_back(s); }
	virtual void gen();
};

struct Expression : public node
{
	Declarations* decs;
	llvm::Value* v;
	void dec(Declarations* _decs) { decs = _decs; }
	Expression() : v(0) {}
	virtual void gen();
};

struct Expressions : public Expression
{
	Declarations* decs;
	llvm::Value* v;
	vector<Expression*> vec;
	void add(Expression* s) { vec.push_back(s); }
	virtual void gen();
};

struct Function : public Declaration
{
	Ident* id;
	typeFields* params;
	Ident* ret;
	Expression* exp;
	Function(Ident* _id, typeFields* _params, Expression* _exp) : id(_id), params(_params), ret(0), exp(_exp) {}
	Function(Ident* _id, typeFields* _params, Ident* _ret, Expression* _exp) : id(_id), params(_params), ret(_ret), exp(_exp) {} 
	llvm::Function* f;
	virtual void gen();
};

struct Variable : public Declaration
{
	Ident* id;
	Ident* type;
	Expression* exp;
	Variable(Ident* _id, Ident* _type, Expression* _exp) : id(_id), type(_type), exp(_exp) {}
	Variable(Ident* _id, Expression* _exp) : id(_id), type(0), exp(_exp) {}
	virtual void gen();
};

struct Program : public node
{
	Expression* exp;
	Program(Expression* _exp) : exp(_exp) {}
	virtual void gen();
};

struct Constant : public Expression
{
	Declarations* decs;
	llvm::Value* v;
	virtual void gen();
};

struct nullConstant : public Constant
{
	Declarations* decs;
	llvm::Value* v;
	nullConstant() : v(0) {}
	virtual void gen();
};

struct intConstant : public Constant
{
	Declarations* decs;
	llvm::Value* v;
	int i;
	intConstant(int _i) : i(_i) {}
	virtual void gen();
};

struct stringConstant : public Constant
{
	Declarations* decs;
	llvm::Value* v;
	string s;
	stringConstant(string _s) : s(_s) {}
	virtual void gen();
};

struct uniOperator : public Expression
{
	Declarations* decs;
	llvm::Value* v;
	string op;
	Expression* exp;
	uniOperator(string _op, Expression* _exp) : op(_op), exp(_exp) {}
	virtual void gen();
};

struct binOperator : public Expression
{
	Declarations* decs;
	llvm::Value* v;
	string op;
	Expression* expa;
	Expression* exadd;
	binOperator(string _op, Expression* _expa, Expression* _exadd) : op(_op), expa(_expa), exadd(_exadd) {}
	virtual void gen();
};

struct leftValue : public Expression
{
	Declarations* decs;
	llvm::Value* v;
	virtual void gen();
};

struct assignOperator : public Expression
{
	Declarations* decs;
	llvm::Value* v;
	leftValue* lvalue;
	Expression* exp;
	assignOperator(leftValue* _lvalue, Expression* _exp) : lvalue(_lvalue), exp(_exp) {}
	virtual void gen();
};

struct ifthenBlock : public Expression
{
	Declarations* decs;
	llvm::Value* v;
	Expression* cond;
	Expression* thenBlock;
	ifthenBlock(Expression* _cond, Expression* _thenBlock) : cond(_cond), thenBlock(_thenBlock) {}
	virtual void gen();
};

struct ifelseBlock : public Expression
{
	Declarations* decs;
	llvm::Value* v;
	Expression* cond;
	Expression* thenBlock;
	Expression* elseBlock;
	ifelseBlock(Expression* _cond, Expression* _thenBlock, Expression* _elseBlock) : 
		cond(_cond), thenBlock(_thenBlock), elseBlock(_elseBlock) {}
	virtual void gen();
};

struct forBlock : public Expression
{
	Declarations* decs;
	llvm::Value* v;
	Ident* id;
	Expression* loopFrom;
	Expression* loopTo;
	Expression* loopBlock;
	forBlock(Ident* _id, Expression* _loopFrom, Expression* _loopTo, Expression* _loopBlock) : 
		id(_id), loopFrom(_loopFrom), loopTo(_loopTo), loopBlock(_loopBlock) {}
	virtual void gen();
};

struct whileBlock : public Expression
{
	Declarations* decs;
	llvm::Value* v;
	Expression* cond;
	Expression* loopBlock;
	whileBlock(Expression* _cond, Expression* _loopBlock) : cond(_cond), loopBlock(_loopBlock) {}
	virtual void gen();
};

struct breakLoop : public Expression
{
	Declarations* decs;
	llvm::Value* v;
	virtual void gen();
};

struct callerParam : public node
{
	Expression* exp;
	callerParam(Expression* _exp) : exp(_exp) {}
	virtual void gen();
};

struct callerParams : public node
{
	vector<callerParam*> vec;
	void add(callerParam* s) { vec.push_back(s); }
	virtual void gen();
};

struct functionCaller : public Expression
{
	Declarations* decs;
	llvm::Value* v;
	Ident* id;
	callerParams* params;
	functionCaller(Ident* _id, callerParams* _params) : id(_id), params(_params) {}
	virtual void gen();
};

struct arrayOperator : public leftValue
{
	Declarations* decs;
	llvm::Value* v;
	Expression* exp;
	arrayOperator(Expression* _exp) : exp(_exp) {} 
	virtual void gen();
};

struct arrayOperators : public leftValue
{
	vector<arrayOperator*> vec;
	void add(arrayOperator* s) { vec.push_back(s); }
	virtual void gen();
};

struct structField : public node
{
	Ident* id;
	Expression* exp;
	structField(Ident* _id, Expression* _exp) : id(_id), exp(_exp) {}
	virtual void gen();
};

struct structFields : public node
{
	vector<structField*> vec;
	void add(structField* s) { vec.push_back(s); }
	virtual void gen();
};

struct structConstant : public Constant
{
	Ident* id;
	structFields* fields;
	structConstant(Ident* _id, structFields* _fields) : id(_id), fields(_fields) {}
	virtual void gen();
};

struct arrayLvalue : public leftValue
{
	leftValue* id;
	arrayOperators* arr;
	arrayLvalue(leftValue* _id, arrayOperators* _arr) : id(_id), arr(_arr) {}
	virtual void gen();
};

struct arrayConstant : public Constant
{
	Ident* type;
	arrayOperators* size;
	Expression* exp;
	arrayConstant(Ident* _type, arrayOperators* _size, Expression* _exp) : type(_type), size(_size), exp(_exp) {}
	virtual void gen();
};

struct varLvalue : public leftValue
{
	Ident* id;
	varLvalue(Ident* _id) : id(_id) {}
	virtual void gen();
};

struct structLvalue : public leftValue
{
	leftValue* id;
	Ident* field;
	structLvalue(leftValue* _id, Ident* _field) : id(_id), field(_field) {}
	virtual void gen();
};

#endif