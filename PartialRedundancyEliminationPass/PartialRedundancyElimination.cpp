#include "llvm/IR/Function.h"
#include "llvm/Pass.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/IR/Dominators.h"
#include "llvm/IR/CFG.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"

using namespace llvm;

namespace {
    struct PRE : public FunctionPass {
        static char ID;
        PRE() : FunctionPass(ID) {}

        bool runOnFunction(Function &F) override {
            bool changed = false;

            DominatorTree &DT = getAnalysis<DominatorTreeWrapperPass>().getDomTree();
            std::set<Instruction*> redundantInstrs;

            for (auto &BB : F) {
                for (auto &I : BB) {
                    if (isRedundant(&I, DT)) {
                        redundantInstrs.insert(&I);
                    }
                }
            }

            for (Instruction *I : redundantInstrs) {
                hoistInstruction(I, DT);
                changed = true;
            }

            return changed;
        }

        bool isRedundant(Instruction *I, DominatorTree &DT) {
            if (I->isBinaryOp()) {
                for (auto *U : I->users()) {
                    if (auto *Inst = dyn_cast<Instruction>(U)) {
                        if (Inst->isBinaryOp() && Inst->getOpcode() == I->getOpcode()) {
                            if (DT.dominates(I, Inst)) {
                                return true;
                            }
                        }
                    }
                }
            }

            return false;
        }

        void hoistInstruction(Instruction *I, DominatorTree &DT) {
            BasicBlock *BB = I->getParent();
            BasicBlock *DomBB = DT.findNearestCommonDominator(BB, BB->getSinglePredecessor());

            if (DomBB) {
                I->moveBefore(DomBB->getTerminator());
            }
        }

        void getAnalysisUsage(AnalysisUsage &AU) const override {
            AU.addRequired<DominatorTreeWrapperPass>();
            AU.setPreservesCFG();
        }
    };
}

char PRE::ID = 0;

static RegisterPass<PRE> X(
    "pre",
    "Partial Redundancy Elimination pass",
    false,
    false
);