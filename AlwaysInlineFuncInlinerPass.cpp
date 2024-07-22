#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Module.h"

using namespace llvm;

struct AIFIPass : public ModulePass
{
  static char ID;
  AIFIPass() : ModulePass(ID) {}

  virtual bool runOnModule(Module &M)
  {
    for(auto& F : M) {
      errs() << F.getName() << "\n";
    }
    return true;
  }
};

char AIFIPass::ID = 0;

static RegisterPass<AIFIPass> X(
    "aifipass", "Always-Inline Function Inliner Pass",
    false,
    false);
