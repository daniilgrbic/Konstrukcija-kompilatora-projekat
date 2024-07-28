#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Module.h"
#include "llvm/Transforms/Utils/Cloning.h"
#include "llvm/IR/IRBuilder.h"


using namespace llvm;

struct AlwaysInlinePass : public ModulePass
{
  static char ID;
  AlwaysInlinePass() : ModulePass(ID) {}

  virtual bool runOnModule(Module &M) {
    bool changed = false;

    for (Function &F : M) {
      if (F.isDeclaration()) {
          continue;
      }

      std::vector<CallBase*> CallsToInline;

      for (auto &BB : F) {
          for (auto &I : BB) {
              if (auto *Call = dyn_cast<CallBase>(&I)) {
                  if (Function *Callee = Call->getCalledFunction()) {
                      if (!Callee->isDeclaration() && canInline(Callee)) {
                          CallsToInline.push_back(Call);
                      }
                  }
              }
          }
      }

      for (CallBase *Call : CallsToInline) {
          Function *Callee = Call->getCalledFunction();
          InlineFunctionInfo IFI;
          InlineResult IR = InlineFunction(*Call, IFI);
          if (IR.isSuccess()) {
            errs() << "Inlined call to function " << Callee->getName() << " in function " << F.getName() << "\n";
            changed = true;
          }
      }
    }
    return changed;
  }

  bool canInline(Function *Callee) {
    // Check if the function is suitable for inlining
    return Callee->hasFnAttribute(Attribute::AlwaysInline);
  }
};

char AlwaysInlinePass::ID = 0;

static RegisterPass<AlwaysInlinePass> X(
    "always-inline-pass", "Always-Inline Function Inliner Pass",
    false,
    false
);
