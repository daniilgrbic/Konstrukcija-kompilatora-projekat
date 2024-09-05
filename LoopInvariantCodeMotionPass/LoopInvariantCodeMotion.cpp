#include "llvm/IR/Function.h"
#include "llvm/Pass.h"
#include "llvm/Analysis/LoopPass.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/Analysis/ValueTracking.h"
#include "llvm/IR/Instructions.h"

using namespace llvm;

namespace {
    struct LICMPass : public LoopPass {
        static char ID;
        LICMPass() : LoopPass(ID) {}

        bool runOnLoop(Loop *L, LPPassManager &LPM) {
            bool changed = false;

            // getting block before the loop leads into it
            BasicBlock *preheader = L->getLoopPreheader();
            if (!preheader) return false;

            for (auto *BB : L->blocks()) {
                for (auto it = BB->begin(); it != BB->end(); ) {
                    Instruction &I = *it++;

                    if (L->hasLoopInvariantOperands(&I)) {
                        if (isSafeToSpeculativelyExecute(&I)) {
                            I.moveBefore(preheader->getTerminator());
                            changed = true;
                        }
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
}

char LICMPass::ID = 0;

static RegisterPass<LICMPass> X(
    "loop-invariant-code-motion",
    "Loop-Invariant Code Motion Pass",
    false,
    false
);