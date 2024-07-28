#include "llvm/IR/Function.h"
#include "llvm/IR/Module.h"
#include "llvm/Pass.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/Transforms/Utils/Cloning.h"

using namespace llvm;

namespace {
class DeadArgumentElimination : public ModulePass {
public:
  static char ID;
  DeadArgumentElimination() : ModulePass(ID) {}

  bool runOnModule(Module &M) override;

  void getAnalysisUsage(AnalysisUsage &AU) const override {
    AU.setPreservesAll();
  }
};
} // namespace

char DeadArgumentElimination::ID = 0;

bool DeadArgumentElimination::runOnModule(Module &M) {
  bool Changed = false;

  return Changed;
}

static RegisterPass<DeadArgumentElimination> X("dead-arg-elim", "Dead Argument Elimination Pass", false, false);
