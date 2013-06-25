#include "ast.h"
#include <iostream>
#include <typeinfo>
#include <stack>

using namespace std;
using namespace llvm;

const int CODEGENDEBUG = 0;

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

Type* findconttype(Ident* id)
{
	for(int i = globaldec.size()-1; i >=0; i--){
		for(int j = globaldec[i]->vec.size()-1; j >= 0; j--){
			if(typeid(*(globaldec[i]->vec[j])) == typeid(arrayType)) {
					arrayType* t = (arrayType*)globaldec[i]->vec[j];
					if(t->id->s() == id->s()) return t->contentType;
			}
		}
	}
	throw string("ContentType Not Found.");
	return 0;
}

Type* findtype(Ident* id)
{
	for(int i = globaldec.size()-1; i >=0; i--){
		for(int j = globaldec[i]->vec.size()-1; j >= 0; j--){
			if(typeid(*(globaldec[i]->vec[j])) == typeid(typeDec) ||
				typeid(*(globaldec[i]->vec[j])) == typeid(aliasType) ||
				typeid(*(globaldec[i]->vec[j])) == typeid(arrayType) ||
				typeid(*(globaldec[i]->vec[j])) == typeid(structType)) {
					typeDec* t = (typeDec*)globaldec[i]->vec[j];
					if(t->id->s() == id->s()) return t->type;
			}
		}
	}
	throw string("Type Not Found.");
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
	throw string("Identifier Not Found.");
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
	throw string("Function Not Found");
	return 0;
}

void node::gen()
{
	if (CODEGENDEBUG == 1) cerr << "Generating code for " << typeid(*this).name() << endl;
}

void Ident::gen()
{
	if (CODEGENDEBUG == 1) cerr << "Generating code for " << typeid(*this).name() << endl;
}

void Declaration::gen()
{
	if (CODEGENDEBUG == 1) cerr << "Generating code for " << typeid(*this).name() << endl;
}

void typeDec::gen()
{
	if (CODEGENDEBUG == 1) cerr << "Generating code for " << typeid(*this).name() << endl;
}

void aliasType::gen()
{
	if (CODEGENDEBUG == 1) cerr << "Generating code for " << typeid(*this).name() << endl;
	type = findtype(content);
}

void arrayType::gen()
{
	if (CODEGENDEBUG == 1) cerr << "Generating code for " << typeid(*this).name() << endl;
	type = PointerType::get(findtype(content), 0);
	contentType = findtype(content);
}

void typeField::gen()
{
	if (CODEGENDEBUG == 1) cerr << "Generating code for " << typeid(*this).name() << endl;
}

void typeFields::gen()
{
	if (CODEGENDEBUG == 1) cerr << "Generating code for " << typeid(*this).name() << endl;
}

void structType::gen()
{
	if (CODEGENDEBUG == 1) cerr << "Generating code for " << typeid(*this).name() << endl;
}

void Declarations::gen()
{
	if (CODEGENDEBUG == 1) cerr << "Generating code for " << typeid(*this).name() << endl;
	Declarations* decblk = new Declarations;
	globaldec.push_back(decblk);
	for(int i = 0; i < vec.size(); i++){
		decblk->add(vec[i]);
		vec[i]->gen();
	}
	if (CODEGENDEBUG == 1) cerr << "Generating code for " << typeid(*this).name() << " done" << endl;
}

void Expression::gen()
{
	if (CODEGENDEBUG == 1) cerr << "Generating code for " << typeid(*this).name() << endl;
}

void Expressions::gen()
{
	if (CODEGENDEBUG == 1) cerr << "Generating code for " << typeid(*this).name() << endl;
	for(int i = 0; i < vec.size(); i++) vec[i]->gen();
	if(vec.size()) v = vec.back()->v;
	if (CODEGENDEBUG == 1) cerr << "Generating code for " << typeid(*this).name() << " done" << endl;
}

void expBlock::gen()
{
	if (CODEGENDEBUG == 1) cerr << "Generating code for " << typeid(*this).name() << endl;
	decs->gen();
	exp->gen();
	v = exp->v;
	globaldec.pop_back();
	if (CODEGENDEBUG == 1) cerr << "Generating code for " << typeid(*this).name() << " done" << endl;
}

void funcDec::gen()
{
	if (CODEGENDEBUG == 1) cerr << "Generating code for " << typeid(*this).name() << endl;
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
	int i;
	for (ait = f->arg_begin(), i = 0; ait != f->arg_end(); ait++, i++) {
		Expression* argValue = new Expression;
		argValue->v = ait;
		argDec->add(new Variable(params->vec[i]->id, params->vec[i]->type, argValue));
	}
	argDec->gen();
	exp->gen();
	b->CreateRet(exp->v);
	popb();
	globaldec.pop_back();
	if (CODEGENDEBUG == 1) cerr << "Generating code for " << typeid(*this).name() << " done" << endl;
}

void Variable::gen()
{
	if (CODEGENDEBUG == 1) cerr << "Generating code for " << typeid(*this).name() << endl;
	exp->gen();
	IRBuilder<>* b = topb();
	Type* vartype;
	if(type) vartype = findtype(type);
	else vartype = Type::getInt32Ty(getGlobalContext());
	v = b->CreateAlloca(vartype, 0, id->s());
	if(exp->v) b->CreateStore(exp->v, v);
	if (CODEGENDEBUG == 1) cerr << "Generating code for " << typeid(*this).name() << " done" << endl;
}

void Program::gen()
{
	if (CODEGENDEBUG == 1) cerr << "Generating code for " << typeid(*this).name() << endl;
	module = new Module("", getGlobalContext());
	FunctionType *funcType = FunctionType::get(Type::getInt32Ty(getGlobalContext()), false);
	mainFunc = Function::Create(funcType, GlobalValue::InternalLinkage, "main", module);
	BasicBlock *entry = BasicBlock::Create(getGlobalContext(), "entry", mainFunc);
	IRBuilder<>* b;
	Declarations* typedecs = new Declarations;
	funcDec* printDec = new funcDec(new Ident("print"), 0, new Expression);
	printDec->f = createPrintfFunction();
	typedecs->add(printDec);
	typeDec* inttype = new typeDec(new Ident(string("int")));
	inttype->type = Type::getInt32Ty(getGlobalContext());
	typedecs->add(inttype);
	globaldec.push_back(typedecs);
	b = pushb(new IRBuilder<>(getGlobalContext()));
	b = pushb(new IRBuilder<>(entry));
	exp->gen();
	b = popb();
	b->CreateRet(exp->v);
	//b->CreateRet(ConstantInt::get(b->getInt32Ty(), 0, true));
	if (CODEGENDEBUG == 1) cerr << "Generating code for " << typeid(*this).name() << " done" << endl;
}

void constValue::gen()
{
	if (CODEGENDEBUG == 1) cerr << "Generating code for " << typeid(*this).name() << endl;
}

void nullConstant::gen()
{
	if (CODEGENDEBUG == 1) cerr << "Generating code for " << typeid(*this).name() << endl;
	v = 0;
}

void intConstant::gen()
{
	if (CODEGENDEBUG == 1) cerr << "Generating code for " << typeid(*this).name() << endl;
	IRBuilder<>* b = topb();
	v = ConstantInt::get(Type::getInt32Ty(getGlobalContext()), i, true);
	if (CODEGENDEBUG == 1) cerr << "Generating code for " << typeid(*this).name() << " done" << endl;
}

void stringConstant::gen()
{
	if (CODEGENDEBUG == 1) cerr << "Generating code for " << typeid(*this).name() << endl;
	IRBuilder<>* b = topb();
	v = b->CreateGlobalStringPtr(s);
	if (CODEGENDEBUG == 1) cerr << "Generating code for " << typeid(*this).name() << " done" << endl;
}

void uniOperator::gen()
{
	if (CODEGENDEBUG == 1) cerr << "Generating code for " << typeid(*this).name() << endl;
	IRBuilder<>* b = topb();
	exp->gen();
	Value* expv = exp->v;
	if(op == "-") v = b->CreateNeg(expv);
}

void binOperator::gen()
{
	if (CODEGENDEBUG == 1) cerr << "Generating code for " << typeid(*this).name() << endl;
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
	if (CODEGENDEBUG == 1) cerr << "Generating code for " << typeid(*this).name() << endl;
}

void assignOperator::gen()
{
	if (CODEGENDEBUG == 1) cerr << "Generating code for " << typeid(*this).name() << endl;
	IRBuilder<>* b = topb();
	lvalue->gen();
	exp->gen();
	v = b->CreateStore(exp->v, lvalue->lv);
	if (CODEGENDEBUG == 1) cerr << "Generating code for " << typeid(*this).name() << " done" << endl;
}

void ifthenBlock::gen()
{
	if (CODEGENDEBUG == 1) cerr << "Generating code for " << typeid(*this).name() << endl;
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
	if (CODEGENDEBUG == 1) cerr << "Generating code for " << typeid(*this).name() << endl;
}

void ifelseBlock::gen()
{
	if (CODEGENDEBUG == 1) cerr << "Generating code for " << typeid(*this).name() << endl;
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
	if (CODEGENDEBUG == 1) cerr << "Generating code for " << typeid(*this).name() << endl;
}

void forBlock::gen()
{
	if (CODEGENDEBUG == 1) cerr << "Generating code for " << typeid(*this).name() << endl;
	IRBuilder<>* b = topb();
	Function *TheFunction = b->GetInsertBlock()->getParent();
	Declarations* argDec = new Declarations;
	loopFrom->gen();
	Variable* loopVar = new Variable(id, new Ident("int"), loopFrom);
	argDec->add(loopVar);
	argDec->gen();
	BasicBlock *LoopBB = BasicBlock::Create(getGlobalContext(), "loop", TheFunction);
	BasicBlock *CondBB = BasicBlock::Create(getGlobalContext(), "cond", TheFunction);
	b->CreateBr(CondBB);
	b->SetInsertPoint(LoopBB);
	loopBlock->gen();
	Value* newv = b->CreateAdd(b->CreateLoad(loopVar->v), ConstantInt::get(Type::getInt32Ty(getGlobalContext()), 1, true));
	b->CreateStore(newv, loopVar->v);
	b->CreateBr(CondBB);
	b->SetInsertPoint(CondBB);
	loopTo->gen();
	Value* EndCond = b->CreateICmpSLE(b->CreateLoad(loopVar->v), loopTo->v);
	BasicBlock *AfterBB = BasicBlock::Create(getGlobalContext(), "afterloop", TheFunction);
	b->CreateCondBr(EndCond, LoopBB, AfterBB);
	b->SetInsertPoint(AfterBB);
	globaldec.pop_back();
}

void whileBlock::gen()
{
	if (CODEGENDEBUG == 1) cerr << "Generating code for " << typeid(*this).name() << endl;
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
	if (CODEGENDEBUG == 1) cerr << "Generating code for " << typeid(*this).name() << endl;
}

void breakLoop::gen()
{
	if (CODEGENDEBUG == 1) cerr << "Generating code for " << typeid(*this).name() << endl;
}

void callerParam::gen()
{
	if (CODEGENDEBUG == 1) cerr << "Generating code for " << typeid(*this).name() << endl;
	IRBuilder<>* b = topb();
	exp->gen();
	v = exp->v;
	if (CODEGENDEBUG == 1) cerr << "Generating code for " << typeid(*this).name() << " done" << endl;
}

void callerParams::gen()
{
	if (CODEGENDEBUG == 1) cerr << "Generating code for " << typeid(*this).name() << endl;
	for(int i = 0; i < vec.size(); i++) vec[i]->gen();
	if (CODEGENDEBUG == 1) cerr << "Generating code for " << typeid(*this).name() << " done" << endl;	
}

void functionCaller::gen()
{
	if (CODEGENDEBUG == 1) cerr << "Generating code for " << typeid(*this).name() << endl;
	IRBuilder<>* b = topb();
	params->gen();
	vector<Value*> vec;
	for(int k = 0; k < params->vec.size(); k++) vec.push_back(params->vec[k]->v);
	v = b->CreateCall(findfunc(id), makeArrayRef(vec));
	if (CODEGENDEBUG == 1) cerr << "Generating code for " << typeid(*this).name() << " done" << endl;
}

void arrayOperator::gen()
{
	if (CODEGENDEBUG == 1) cerr << "Generating code for " << typeid(*this).name() << endl;
	exp->gen();
	v = exp->v;
}

void arrayOperators::gen()
{
	if (CODEGENDEBUG == 1) cerr << "Generating code for " << typeid(*this).name() << endl;
	for(int i = 0; i < vec.size(); i++) vec[i]->gen();
}

void structField::gen()
{
	if (CODEGENDEBUG == 1) cerr << "Generating code for " << typeid(*this).name() << endl;
}

void structFields::gen()
{
	if (CODEGENDEBUG == 1) cerr << "Generating code for " << typeid(*this).name() << endl;
}

void structConstant::gen()
{
	if (CODEGENDEBUG == 1) cerr << "Generating code for " << typeid(*this).name() << endl;
}

void arrayLvalue::gen()
{
	if (CODEGENDEBUG == 1) cerr << "Generating code for " << typeid(*this).name() << endl;
	IRBuilder<>* b = topb();
	id->gen();
	arr->gen();
	vector<Value*> val;
	lv = id->lv;
	v = b->CreateLoad(lv);
	for(int i = 0; i < arr->vec.size(); i++){
		val.clear();
		val.push_back(arr->vec[i]->v);
		lv = GetElementPtrInst::Create(v, makeArrayRef(val), "", b->GetInsertBlock());
		v = b->CreateLoad(lv);
	}
}

void arrayConstant::gen()
{
	if (CODEGENDEBUG == 1) cerr << "Generating code for " << typeid(*this).name() << endl;
	size->gen();
	exp->gen();
	IRBuilder<>* b = topb();
	v = new AllocaInst(findconttype(type), size->vec[0]->v, "", b->GetInsertBlock());
}

void varLvalue::gen()
{
	if (CODEGENDEBUG == 1) cerr << "Generating code for " << typeid(*this).name() << endl;
	IRBuilder<>* b = topb();
	lv = findvar(id);
	v = b->CreateLoad(lv);
	if (CODEGENDEBUG == 1) cerr << "Generating code for " << typeid(*this).name() << " done" << endl;
}

void structLvalue::gen()
{
	if (CODEGENDEBUG == 1) cerr << "Generating code for " << typeid(*this).name() << endl;
}

