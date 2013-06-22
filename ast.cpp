#include "ast.h"
#include <iostream>
#include <typeinfo>

using namespace std;

void node::gen()
{
	cerr << typeid(*this).name() << " code generator called!" << endl;
}

void Ident::gen()
{
	cerr << typeid(*this).name() << " code generator called!" << endl;
}

void Declaration::gen()
{
	cerr << typeid(*this).name() << " code generator called!" << endl;
}

void Type::gen()
{
	cerr << typeid(*this).name() << " code generator called!" << endl;
}

void aliasType::gen()
{
	cerr << typeid(*this).name() << " code generator called!" << endl;
}

void arrayType::gen()
{
	cerr << typeid(*this).name() << " code generator called!" << endl;
}

void typeField::gen()
{
	cerr << typeid(*this).name() << " code generator called!" << endl;
}

void typeFields::gen()
{
	cerr << typeid(*this).name() << " code generator called!" << endl;
}

void structType::gen()
{
	cerr << typeid(*this).name() << " code generator called!" << endl;
}

void Declarations::gen()
{
	cerr << typeid(*this).name() << " code generator called!" << endl;
}

void Expression::gen()
{
	cerr << typeid(*this).name() << " code generator called!" << endl;
}

void Expressions::gen()
{
	cerr << typeid(*this).name() << " code generator called!" << endl;
}

void Function::gen()
{
	cerr << typeid(*this).name() << " code generator called!" << endl;
}

void Variable::gen()
{
	cerr << typeid(*this).name() << " code generator called!" << endl;
}

void Program::gen()
{
	cerr << typeid(*this).name() << " code generator called!" << endl;
}

void Constant::gen()
{
	cerr << typeid(*this).name() << " code generator called!" << endl;
}

void nullConstant::gen()
{
	cerr << typeid(*this).name() << " code generator called!" << endl;
}

void intConstant::gen()
{
	cerr << typeid(*this).name() << " code generator called!" << endl;
}

void stringConstant::gen()
{
	cerr << typeid(*this).name() << " code generator called!" << endl;
}

void uniOperator::gen()
{
	cerr << typeid(*this).name() << " code generator called!" << endl;
}

void binOperator::gen()
{
	cerr << typeid(*this).name() << " code generator called!" << endl;
}

void leftValue::gen()
{
	cerr << typeid(*this).name() << " code generator called!" << endl;
}

void assignOperator::gen()
{
	cerr << typeid(*this).name() << " code generator called!" << endl;
}

void ifthenBlock::gen()
{
	cerr << typeid(*this).name() << " code generator called!" << endl;
}

void ifelseBlock::gen()
{
	cerr << typeid(*this).name() << " code generator called!" << endl;
}

void forBlock::gen()
{
	cerr << typeid(*this).name() << " code generator called!" << endl;
}

void whileBlock::gen()
{
	cerr << typeid(*this).name() << " code generator called!" << endl;
}

void breakLoop::gen()
{
	cerr << typeid(*this).name() << " code generator called!" << endl;
}

void callerParam::gen()
{
	cerr << typeid(*this).name() << " code generator called!" << endl;
}

void callerParams::gen()
{
	cerr << typeid(*this).name() << " code generator called!" << endl;
}

void functionCaller::gen()
{
	cerr << typeid(*this).name() << " code generator called!" << endl;
}

void arrayOperator::gen()
{
	cerr << typeid(*this).name() << " code generator called!" << endl;
}

void arrayOperators::gen()
{
	cerr << typeid(*this).name() << " code generator called!" << endl;
}

void structField::gen()
{
	cerr << typeid(*this).name() << " code generator called!" << endl;
}

void structFields::gen()
{
	cerr << typeid(*this).name() << " code generator called!" << endl;
}

void structConstant::gen()
{
	cerr << typeid(*this).name() << " code generator called!" << endl;
}

void arrayLvalue::gen()
{
	cerr << typeid(*this).name() << " code generator called!" << endl;
}

void arrayConstant::gen()
{
	cerr << typeid(*this).name() << " code generator called!" << endl;
}

void varLvalue::gen()
{
	cerr << typeid(*this).name() << " code generator called!" << endl;
}

void structLvalue::gen()
{
	cerr << typeid(*this).name() << " code generator called!" << endl;
}

