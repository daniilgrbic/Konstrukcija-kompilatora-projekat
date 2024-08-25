#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Argument.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/Module.h"
#include "llvm/Transforms/Utils/Cloning.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"

#include <queue>
#include <set>
#include <map>

using namespace llvm;

struct ImplicitMulOperandUpcastPass : public FunctionPass {   

    static char ID;

    ImplicitMulOperandUpcastPass() : FunctionPass(ID) {}

    void buildCFG(std::map<BasicBlock*, std::set<BasicBlock*>>& CFG, BasicBlock* BB) {

        if(CFG.find(BB) != CFG.end())
            return;
        
        for (auto SuccBB : successors(BB)) {
            CFG[BB].insert(SuccBB);
            buildCFG(CFG, SuccBB);
        }
    } 

    bool canIntegerTypeCast(Type *T1, Type *T2) {
        if (T1->isIntegerTy() && T2->isIntegerTy()) {
            unsigned bitWidth1 = T1->getIntegerBitWidth();
            unsigned bitWidth2 = T2->getIntegerBitWidth();
            return bitWidth1 <= bitWidth2;
        }
        return false;
    }

    virtual bool runOnFunction(Function& F) {

        bool modified = false;

        std::map<BasicBlock*, std::set<BasicBlock*>> CFG;
        buildCFG(CFG, &F.getEntryBlock());

        std::queue<BasicBlock*> CFGLeaves;
        for(BasicBlock& BB : F) {
            if(CFG[&BB].size() == 0) {
                CFGLeaves.push(&BB);
            }
        }

        std::map<Value*, Type*> PrefferedValueType;
        std::map<Value*, Instruction*> SExtInstructions;

        while(not CFGLeaves.empty()) {

            BasicBlock* BB = CFGLeaves.front();
            CFGLeaves.pop();

            for(Instruction& I : reverse(*BB)) {
                
                Value* V = &I;
                Type *IType = I.getType();

                // errs() << I << " -> ";
                // IType->print(errs());
                // errs() << "\n";

                if(PrefferedValueType.find(V) == PrefferedValueType.end()) {
                    PrefferedValueType[V] = IType;
                }

                if(isa<SExtInst>(I)) {
                    Value* OperandToImplicitlyCast = I.getOperand(0);
                    PrefferedValueType[OperandToImplicitlyCast] = IType;
                    SExtInstructions[OperandToImplicitlyCast] = &I;
                }

                if(isa<MulOperator>(I)) {

                    Type* PType = PrefferedValueType[V];

                    if(IType == PType || !canIntegerTypeCast(IType, PType))
                        continue;

                    modified = true;

                    Value* Operand0 = I.getOperand(0);
                    Value* Operand1 = I.getOperand(1);

                    IRBuilder<> Builder(&I);

                    Value* SExtInst0 = Builder.CreateSExt(Operand0, PType, "");
                    Value* SExtInst1 = Builder.CreateSExt(Operand1, PType, "");
                    Value* NewMul = Builder.CreateMul(SExtInst0, SExtInst1);

                    SExtInstructions[V]->replaceAllUsesWith(NewMul);
                }
            }

            for(BasicBlock* PredBB : predecessors(BB)) {
                CFG[PredBB].erase(BB);
                if(CFG[PredBB].empty()) {
                    CFGLeaves.push(PredBB);
                }
            }
        }

        return modified;
    }
};

char ImplicitMulOperandUpcastPass::ID = 0;

static RegisterPass<ImplicitMulOperandUpcastPass> X(
    "implicit-muloperand-upcast",
    "Implicit MulOperand Upcast Pass",
    false,
    false
);
