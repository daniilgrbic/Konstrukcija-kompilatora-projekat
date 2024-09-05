#include "llvm/IR/Function.h"
#include "llvm/Pass.h"
#include "llvm/Analysis/LoopPass.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/Analysis/ValueTracking.h"
#include "llvm/IR/Instructions.h"

#include <map>

using namespace llvm;

struct LICMPass : public LoopPass {
    static char ID;
    LICMPass() : LoopPass(ID) {}

    bool checkIfOperandChangedInLoop(Loop* L, Value* V, BasicBlock* Entry) {
        for (BasicBlock *BB : L->blocks()) {
            if(BB == Entry)
                continue;
            for(Instruction& I : *BB) {
                if(isa<StoreInst>(I)) {
                    if(V == I.getOperand(1)) {
                        return true;
                    }
                }
            }
        }
        return false;
    }

    bool runOnLoop(Loop *L, LPPassManager &LPM) {
        bool changed = false;

        // getting block before the loop leads into it
        BasicBlock *preheader = L->getLoopPreheader();
        if (!preheader) return false;

        BasicBlock& LoopEntry = **(L->block_begin());

        std::map<Value*, Instruction*> instructionValues;
        for (auto it = LoopEntry.begin(); it != LoopEntry.end(); ) {
            Instruction &I = *it++;

            instructionValues.insert({&I, &I});
            if (auto *BinOp = dyn_cast<BinaryOperator>(&I)) {
                if(BinOp->getOpcode() == Instruction::Add) {
                    Value* op0 = instructionValues[I.getOperand(0)]->getOperand(0);
                    Value* op1 = instructionValues[I.getOperand(1)]->getOperand(0);
                    if(checkIfOperandChangedInLoop(L, op0, &LoopEntry))
                        continue;
                    if(checkIfOperandChangedInLoop(L, op1, &LoopEntry))
                        continue;
                    instructionValues[I.getOperand(0)]->moveBefore(preheader->getTerminator());
                    instructionValues[I.getOperand(1)]->moveBefore(preheader->getTerminator());
                    I.moveBefore(preheader->getTerminator());
                }
            }
        }

        return changed;
    }

    void getAnalysisUsage(AnalysisUsage &AU) const override {
        AU.addRequired<LoopInfoWrapperPass>();
        AU.addPreserved<LoopInfoWrapperPass>();
    }
};

char LICMPass::ID = 0;

static RegisterPass<LICMPass> X(
    "loop-invariant-code-motion",
    "Loop-Invariant Code Motion Pass",
    false,
    false
);