#include "llvm/IR/Function.h"
#include "llvm/IR/Module.h"
#include "llvm/Pass.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/Transforms/Utils/Cloning.h"

using namespace llvm;

struct DeadArgumentElimination : public ModulePass 
{
  static char ID;
  DeadArgumentElimination() : ModulePass(ID) {}

  virtual bool runOnModule(Module &M) {
    bool Changed = false;

    for (Function &F : M) {
      if (F.isDeclaration()) continue;

      SmallVector<unsigned, 4> DeadArgs;
      for (unsigned i = 0, e = F.arg_size(); i != e; ++i) {
        Argument *Arg = F.getArg(i);
        if (Arg->use_empty()) {
          DeadArgs.push_back(i);
        }
      }

      if (DeadArgs.empty()) {
        continue;
      }

      Changed = true;

      std::vector<Type *> ArgTypes;
      for (auto &Arg : F.args()) {
        if (std::find(DeadArgs.begin(), DeadArgs.end(), Arg.getArgNo()) == DeadArgs.end()) {
          ArgTypes.push_back(Arg.getType());
        }
      }

      FunctionType *FTy = FunctionType::get(F.getReturnType(), ArgTypes, F.isVarArg());
      Function *NewF = Function::Create(FTy, F.getLinkage(), F.getName(), &M);
      NewF->copyAttributesFrom(&F);

      for (BasicBlock &BB : llvm::make_early_inc_range(F)) {
          BB.moveBefore(&*NewF->begin());
      }
      
      F.replaceAllUsesWith(NewF);
      F.eraseFromParent();
    }

    return Changed;
  }

}; 

char DeadArgumentElimination::ID = 0;



static RegisterPass<DeadArgumentElimination> X("dead-arg-elim", "Dead Argument Elimination Pass", false, false);
