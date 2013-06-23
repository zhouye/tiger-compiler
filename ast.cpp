#include "ast.h"
#include <iostream>
#include <typeinfo>
#include <stack>

using namespace std;
using namespace llvm;

stack<IRBuilder<>*> builders;
vector<Declarations*> globaldec;
Module *module;

IRBuilder<>* pushb(IRBuilder<>* blk)
{
	builders.push(blk);
	return blk;
}

IRBuilder<>* popb()
{
	IRBuilder<>* res = builders.top();
	builders.pop();
	return res;
}

IRBuilder<>* topb()
{
	return builders.top();
}

void node::gen()
{
	cerr << "Generating code for " << typeid(*this).name() << endl;
}

void Ident::gen()
{
	cerr << "Generating code for " << typeid(*this).name() << endl;
}

void Declaration::gen()
{
	cerr << "Generating code for " << typeid(*this).name() << endl;
}

void typeDec::gen()
{
	cerr << "Generating code for " << typeid(*this).name() << endl;
}

void aliasType::gen()
{
	cerr << "Generating code for " << typeid(*this).name() << endl;
}

void arrayType::gen()
{
	cerr << "Generating code for " << typeid(*this).name() << endl;
}

void typeField::gen()
{
	cerr << "Generating code for " << typeid(*this).name() << endl;
}

void typeFields::gen()
{
	cerr << "Generating code for " << typeid(*this).name() << endl;
}

void structType::gen()
{
	cerr << "Generating code for " << typeid(*this).name() << endl;
}

void Declarations::gen()
{
	cerr << "Generating code for " << typeid(*this).name() << endl;
	for(int i = 0; i < vec.size(); i++) vec[i]->gen();
	cerr << "Generating code for " << typeid(*this).name() << " done" << endl;
}

void Expression::gen()
{
	cerr << "Generating code for " << typeid(*this).name() << endl;
}

void Expressions::gen()
{
	cerr << "Generating code for " << typeid(*this).name() << endl;
	for(int i = 0; i < vec.size(); i++) vec[i]->gen();
	if(vec.size()) v = vec.back()->v;
	cerr << "Generating code for " << typeid(*this).name() << " done" << endl;
}

void expBlock::gen()
{
	cerr << "Generating code for " << typeid(*this).name() << endl;
	decs->gen();
	globaldec.push_back(decs);
	exp->gen();
	globaldec.pop_back();
	cerr << "Generating code for " << typeid(*this).name() << " done" << endl;
}

void funcDec::gen()
{
	cerr << "Generating code for " << typeid(*this).name() << endl;
}

void Variable::gen()
{
	cerr << "Generating code for " << typeid(*this).name() << endl;
	for(int i = 0; i < globaldec.size(); i++){
		for(int j = 0; j < globaldec[i]->vec.size(); j++){
			if(typeid(*(globaldec[i]->vec[j])) == typeid(typeDec)){
				typeDec* t = (typeDec*)globaldec[i]->vec[j];
				if(t->id->s() == type->s()){
					IRBuilder<>* b = topb();
					v = b->CreateAlloca(t->type, 0, id->s());
				}
			}
		}
	}
	cerr << "Generating code for " << typeid(*this).name() << " done" << endl;
}

void Program::gen()
{
	cerr << "Generating code for " << typeid(*this).name() << endl;
	module = new Module("", getGlobalContext());
	FunctionType *funcType = FunctionType::get(Type::getInt32Ty(getGlobalContext()), false);
	Function *mainFunc = Function::Create(funcType, Function::ExternalLinkage, "main", module);
	BasicBlock *entry = BasicBlock::Create(getGlobalContext(), "", mainFunc);
	IRBuilder<>* b;
	Declarations* typedecs = new Declarations;
	typeDec* inttype = new typeDec;
	inttype->id = new Ident(string("int"));
	inttype->type = Type::getInt32Ty(getGlobalContext());
	typedecs->add(inttype);
	globaldec.push_back(typedecs);
	b = pushb(new IRBuilder<>(getGlobalContext()));
	b = pushb(new IRBuilder<>(entry));
	exp->gen();
	b = popb();
	b->CreateRet(ConstantInt::get(b->getInt32Ty(), 0, true));
	cerr << "Generating code for " << typeid(*this).name() << " done" << endl;
}

void constValue::gen()
{
	cerr << "Generating code for " << typeid(*this).name() << endl;
}

void nullConstant::gen()
{
	cerr << "Generating code for " << typeid(*this).name() << endl;
}

void intConstant::gen()
{
	cerr << "Generating code for " << typeid(*this).name() << endl;
	IRBuilder<>* b = topb();
	v = ConstantInt::get(b->getInt32Ty(), i, true);
	cerr << "Generating code for " << typeid(*this).name() << " done" << endl;
}

void stringConstant::gen()
{
	cerr << "Generating code for " << typeid(*this).name() << endl;
	IRBuilder<>* b = topb();
	v = b->CreateGlobalStringPtr(s);
	cerr << "Generating code for " << typeid(*this).name() << " done" << endl;
}

void uniOperator::gen()
{
	cerr << "Generating code for " << typeid(*this).name() << endl;
}

void binOperator::gen()
{
	cerr << "Generating code for " << typeid(*this).name() << endl;
}

void leftValue::gen()
{
	cerr << "Generating code for " << typeid(*this).name() << endl;
}

void assignOperator::gen()
{
	cerr << "Generating code for " << typeid(*this).name() << endl;
	IRBuilder<>* b = topb();
	lvalue->gen();
	exp->gen();
	v = b->CreateStore(exp->v, lvalue->v);
	cerr << "Generating code for " << typeid(*this).name() << " done" << endl;
}

void ifthenBlock::gen()
{
	cerr << "Generating code for " << typeid(*this).name() << endl;
}

void ifelseBlock::gen()
{
	cerr << "Generating code for " << typeid(*this).name() << endl;
}

void forBlock::gen()
{
	cerr << "Generating code for " << typeid(*this).name() << endl;
}

void whileBlock::gen()
{
	cerr << "Generating code for " << typeid(*this).name() << endl;
}

void breakLoop::gen()
{
	cerr << "Generating code for " << typeid(*this).name() << endl;
}

void callerParam::gen()
{
	cerr << "Generating code for " << typeid(*this).name() << endl;
}

void callerParams::gen()
{
	cerr << "Generating code for " << typeid(*this).name() << endl;
}

void functionCaller::gen()
{
	cerr << "Generating code for " << typeid(*this).name() << endl;
}

void arrayOperator::gen()
{
	cerr << "Generating code for " << typeid(*this).name() << endl;
}

void arrayOperators::gen()
{
	cerr << "Generating code for " << typeid(*this).name() << endl;
}

void structField::gen()
{
	cerr << "Generating code for " << typeid(*this).name() << endl;
}

void structFields::gen()
{
	cerr << "Generating code for " << typeid(*this).name() << endl;
}

void structConstant::gen()
{
	cerr << "Generating code for " << typeid(*this).name() << endl;
}

void arrayLvalue::gen()
{
	cerr << "Generating code for " << typeid(*this).name() << endl;
}

void arrayConstant::gen()
{
	cerr << "Generating code for " << typeid(*this).name() << endl;
}

void varLvalue::gen()
{
	cerr << "Generating code for " << typeid(*this).name() << endl;
	for(int i = 0; i < globaldec.size(); i++){
		for(int j = 0; j < globaldec[i]->vec.size(); j++){
			if(typeid(*(globaldec[i]->vec[j])) == typeid(Variable)){
				Variable* t = (Variable*)globaldec[i]->vec[j];
				if(t->id->s() == id->s()){
					v = t->v;
				}
			}
		}
	}
	cerr << "Generating code for " << typeid(*this).name() << " done" << endl;
}

void structLvalue::gen()
{
	cerr << "Generating code for " << typeid(*this).name() << endl;
}

