#include "ast.h"
#include <iostream>
#include <typeinfo>
#include <stack>

using namespace std;
using namespace llvm;

stack<IRBuilder<>*> builders;
vector<Declarations*> globaldec;
Module *module;

Function* createPrintfFunction()
{
    std::vector<Type*> printf_arg_types;
    printf_arg_types.push_back(Type::getInt8PtrTy(getGlobalContext()));
    FunctionType* printf_type = FunctionType::get(Type::getInt32Ty(getGlobalContext()), printf_arg_types, true);
    Function *func = Function::Create(printf_type, Function::ExternalLinkage, "printf", module);
    func->setCallingConv(CallingConv::C);
    return func;
}

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

Type* findtype(Ident* id)
{
	for(int i = globaldec.size()-1; i >=0; i--){
		for(int j = globaldec[i]->vec.size()-1; j >= 0; j--){
			if(typeid(*(globaldec[i]->vec[j])) == typeid(typeDec)){
				typeDec* t = (typeDec*)globaldec[i]->vec[j];
				if(t->id->s() == id->s()) return t->type;
			}
		}
	}
	return 0;
}

Value* findvar(Ident* id)
{
	for(int i = globaldec.size()-1; i >=0; i--){
		for(int j = globaldec[i]->vec.size()-1; j >= 0; j--){
			if(typeid(*(globaldec[i]->vec[j])) == typeid(Variable)){
				Variable* t = (Variable*)globaldec[i]->vec[j];
				if(t->id->s() == id->s()) return t->v;
			}
		}
	}
	return 0;
}

Function* findfunc(Ident* id)
{
	for(int i = globaldec.size()-1; i >=0; i--){
		for(int j = globaldec[i]->vec.size()-1; j >= 0; j--){
			if(typeid(*(globaldec[i]->vec[j])) == typeid(funcDec)){
				funcDec* t = (funcDec*)globaldec[i]->vec[j];
				if(t->id->s() == id->s()) return t->f;
			}
		}
	}
	return 0;
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
	Declarations* decblk = new Declarations;
	globaldec.push_back(decblk);
	for(int i = 0; i < vec.size(); i++){
		vec[i]->gen();
		decblk->add(vec[i]);
	}
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
	exp->gen();
	v = exp->v;
	globaldec.pop_back();
	cerr << "Generating code for " << typeid(*this).name() << " done" << endl;
}

void funcDec::gen()
{
	cerr << "Generating code for " << typeid(*this).name() << endl;
	Type* rettype = 0;
	if(ret) rettype = findtype(ret);
	else rettype = Type::getVoidTy(getGlobalContext());
	vector<Type*> argTypes;
	for(int i = 0; i < params->vec.size(); i++) argTypes.push_back(findtype(params->vec[i]->type));
	Declarations* argDec = new Declarations;
	FunctionType *ftype = FunctionType::get(rettype, makeArrayRef(argTypes), false);
	f = Function::Create(ftype, GlobalValue::InternalLinkage, id->s(), module);
	Function::arg_iterator ait;
	BasicBlock *bblock = BasicBlock::Create(getGlobalContext(), "entry", f, 0);
    IRBuilder<>* b = new IRBuilder<>(bblock);
	pushb(b);
	for (ait = f->arg_begin(); ait != f->arg_end(); ait++) {
		Expression* argValue = new Expression;
		argValue->v = ait;
		for(int i = 0; i < params->vec.size(); i++) argDec->add(new Variable(params->vec[i]->id, argValue));
	}
	argDec->gen();
	exp->gen();
	b->CreateRet(exp->v);
	popb();
	globaldec.pop_back();
	cerr << "Generating code for " << typeid(*this).name() << " done" << endl;
}

void Variable::gen()
{
	cerr << "Generating code for " << typeid(*this).name() << endl;
	exp->gen();
	IRBuilder<>* b = topb();
	Type* vartype;
	if(type) vartype = findtype(type);
	else vartype = Type::getInt32Ty(getGlobalContext());
	v = b->CreateAlloca(vartype, 0, id->s());
	b->CreateStore(exp->v, v);
	cerr << "Generating code for " << typeid(*this).name() << " done" << endl;
}

void Program::gen()
{
	cerr << "Generating code for " << typeid(*this).name() << endl;
	module = new Module("", getGlobalContext());
	FunctionType *funcType = FunctionType::get(Type::getInt32Ty(getGlobalContext()), false);
	mainFunc = Function::Create(funcType, GlobalValue::InternalLinkage, "main", module);
	BasicBlock *entry = BasicBlock::Create(getGlobalContext(), "entry", mainFunc);
	IRBuilder<>* b;
	Declarations* typedecs = new Declarations;
	funcDec* printDec = new funcDec(new Ident("print"), 0, new Expression);
	printDec->f = createPrintfFunction();
	typedecs->add(printDec);
	typeDec* inttype = new typeDec;
	inttype->id = new Ident(string("int"));
	inttype->type = Type::getInt32Ty(getGlobalContext());
	typedecs->add(inttype);
	globaldec.push_back(typedecs);
	b = pushb(new IRBuilder<>(getGlobalContext()));
	b = pushb(new IRBuilder<>(entry));
	exp->gen();
	b = popb();
	b->CreateRet(exp->v);
	//b->CreateRet(ConstantInt::get(b->getInt32Ty(), 0, true));
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
	v = ConstantInt::get(Type::getInt32Ty(getGlobalContext()), i, true);
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
	IRBuilder<>* b = topb();
	exp->gen();
	Value* expv = exp->v;
	if(op == "-") v = b->CreateNeg(expv);
}

void binOperator::gen()
{
	cerr << "Generating code for " << typeid(*this).name() << endl;
	IRBuilder<>* b = topb();
	expa->gen();
	expb->gen();
	Value* expav = expa->v;
	Value* expbv = expb->v;
	if(op == "+") v = b->CreateAdd(expav, expbv);
	if(op == "-") v = b->CreateSub(expav, expbv);
	if(op == "*") v = b->CreateMul(expav, expbv);
	if(op == "/") v = b->CreateSDiv(expav, expbv);
	if(op == "=") v = b->CreateICmpEQ(expav, expbv);
	if(op == "<>") v = b->CreateICmpNE(expav, expbv);
	if(op == "<") v = b->CreateICmpSLT(expav, expbv);
	if(op == ">") v = b->CreateICmpSGT(expav, expbv);
	if(op == "<=") v = b->CreateICmpSLE(expav, expbv);
	if(op == ">=") v = b->CreateICmpSGE(expav, expbv);
	if(op == "&") v = b->CreateAnd(expav, expbv);
	if(op == "|") v = b->CreateOr(expav, expbv);
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
	v = b->CreateStore(exp->v, lvalue->lv);
	cerr << "Generating code for " << typeid(*this).name() << " done" << endl;
}

void ifthenBlock::gen()
{
	cerr << "Generating code for " << typeid(*this).name() << endl;
	IRBuilder<>* b = topb();
	cond->gen();
	Function *TheFunction = b->GetInsertBlock()->getParent();
	BasicBlock *ThenBB = BasicBlock::Create(getGlobalContext(), "then", TheFunction);
	BasicBlock *MergeBB = BasicBlock::Create(getGlobalContext(), "ifcont");
	b->CreateCondBr(cond->v, ThenBB, MergeBB);
	b->SetInsertPoint(ThenBB);
	thenBlock->gen();
	b->CreateBr(MergeBB);
	ThenBB = b->GetInsertBlock();
	TheFunction->getBasicBlockList().push_back(MergeBB);
	b->SetInsertPoint(MergeBB);
	v = 0;
	cerr << "Generating code for " << typeid(*this).name() << endl;
}

void ifelseBlock::gen()
{
	cerr << "Generating code for " << typeid(*this).name() << endl;
	IRBuilder<>* b = topb();
	cond->gen();
	Function *TheFunction = b->GetInsertBlock()->getParent();
	BasicBlock *ThenBB = BasicBlock::Create(getGlobalContext(), "then", TheFunction);
	BasicBlock *ElseBB = BasicBlock::Create(getGlobalContext(), "else");
	BasicBlock *MergeBB = BasicBlock::Create(getGlobalContext(), "ifcont");
	b->CreateCondBr(cond->v, ThenBB, ElseBB);
	b->SetInsertPoint(ThenBB);
	thenBlock->gen();
	b->CreateBr(MergeBB);
	ThenBB = b->GetInsertBlock();
	TheFunction->getBasicBlockList().push_back(ElseBB);
	b->SetInsertPoint(ElseBB);
	elseBlock->gen();
	b->CreateBr(MergeBB);
	ElseBB = b->GetInsertBlock();
	TheFunction->getBasicBlockList().push_back(MergeBB);
	b->SetInsertPoint(MergeBB);
	PHINode *PN = b->CreatePHI(thenBlock->v->getType(), 2, "iftmp");
	PN->addIncoming(thenBlock->v, ThenBB);
	PN->addIncoming(elseBlock->v, ElseBB);
	v = PN;
	cerr << "Generating code for " << typeid(*this).name() << endl;
}

void forBlock::gen()
{
	cerr << "Generating code for " << typeid(*this).name() << endl;
}

void whileBlock::gen()
{
	cerr << "Generating code for " << typeid(*this).name() << endl;
	IRBuilder<>* b = topb();
	Function *TheFunction = b->GetInsertBlock()->getParent();
	BasicBlock *CondBB = BasicBlock::Create(getGlobalContext(), "cond", TheFunction);
	BasicBlock *LoopBB = BasicBlock::Create(getGlobalContext(), "loop");
	BasicBlock *MergeBB = BasicBlock::Create(getGlobalContext(), "merge");
	b->CreateBr(CondBB);
	b->SetInsertPoint(CondBB);
	cond->gen();
	b->CreateCondBr(cond->v, LoopBB, MergeBB);
	CondBB = b->GetInsertBlock();
	TheFunction->getBasicBlockList().push_back(LoopBB);
	b->SetInsertPoint(LoopBB);
	loopBlock->gen();
	b->CreateBr(CondBB);
	LoopBB = b->GetInsertBlock();
	TheFunction->getBasicBlockList().push_back(MergeBB);
	b->SetInsertPoint(MergeBB);
	v = 0;
	cerr << "Generating code for " << typeid(*this).name() << endl;
}

void breakLoop::gen()
{
	cerr << "Generating code for " << typeid(*this).name() << endl;
}

void callerParam::gen()
{
	cerr << "Generating code for " << typeid(*this).name() << endl;
	IRBuilder<>* b = topb();
	exp->gen();
	v = exp->v;
	cerr << "Generating code for " << typeid(*this).name() << " done" << endl;
}

void callerParams::gen()
{
	cerr << "Generating code for " << typeid(*this).name() << endl;
	for(int i = 0; i < vec.size(); i++) vec[i]->gen();
	cerr << "Generating code for " << typeid(*this).name() << " done" << endl;	
}

void functionCaller::gen()
{
	cerr << "Generating code for " << typeid(*this).name() << endl;
	IRBuilder<>* b = topb();
	params->gen();
	vector<Value*> vec;
	for(int k = 0; k < params->vec.size(); k++) vec.push_back(params->vec[k]->v);
	v = b->CreateCall(findfunc(id), makeArrayRef(vec));
	cerr << "Generating code for " << typeid(*this).name() << " done" << endl;
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
	IRBuilder<>* b = topb();
	lv = findvar(id);
	v = b->CreateLoad(lv);
	cerr << "Generating code for " << typeid(*this).name() << " done" << endl;
}

void structLvalue::gen()
{
	cerr << "Generating code for " << typeid(*this).name() << endl;
}

