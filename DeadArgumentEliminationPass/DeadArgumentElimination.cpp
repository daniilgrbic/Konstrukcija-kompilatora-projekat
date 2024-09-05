#include "llvm/IR/Function.h"
#include "llvm/IR/Module.h"
#include "llvm/Pass.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/Transforms/Utils/Cloning.h"

using namespace llvm;

#include <vector>

struct DeadArgumentElimination : public ModulePass 
{
  static char ID;
  DeadArgumentElimination() : ModulePass(ID) {}

  bool findIsArgumentDead(Function &F, Value *Arg){
    for(BasicBlock &BB : F){
      for(Instruction &I : BB){
        for(Use &U : I.operands()){
          Value *Val = U.get();
          if(Val == Arg){
            if(isa<LoadInst>(I)){
              return false;
            }
          }
        }
      }
    }
    return true;
  }
  
  bool isDead(Function &F, Argument &Arg){
    for(BasicBlock &BB : F){
      for(Instruction &I : BB){
        for(Use &U : I.operands()){
          Value *Val = U.get();
            if(Val == &Arg){
              if(findIsArgumentDead(F, I.getOperand(1))){
                errs() << Arg << " is Dead " << "\n";
                // Arg.replaceAllUsesWith(PoisonValue::get(Arg.getType()));
                return true;
              }
              else{
                errs() << Arg << " is not Dead " << "\n";
                return false;
              }
            }
        }
      }
    }
    return false;
  }

  void replaceFunctionUses(llvm::Function *OldFunc, llvm::Function *NewFunc, std::vector<int> ArgIndices) {
    // Collect all the uses of the old function
    std::vector<llvm::Use *> usesToReplace;
    for (auto &Use : OldFunc->uses()) {
      usesToReplace.push_back(&Use);
    }

    std::vector<CallInst*> toDelete;
    // Replace all uses of the old function with the new function
    for (llvm::Use *U : usesToReplace) {
      // Get the user (Instruction) of the function use
      if (llvm::CallInst *CI = llvm::dyn_cast<llvm::CallInst>(U->getUser())) {
        // If it's a call instruction, replace the called function
        // CI->setCalledFunction(NewFunc);
        
        std::vector<Value*> NewCiOperands;
        for(auto index : ArgIndices) {
          NewCiOperands.push_back(CI->getOperand(index));
        }

        IRBuilder<> Builder(CI);
        auto fv = Builder.CreateCall(NewFunc, NewCiOperands);

        errs() << *NewFunc << "\n";
        errs() << "IDE GAS : " << *fv << "\n";

        // CI->eraseFromParent();
        CI->replaceAllUsesWith(fv);
        toDelete.push_back(CI);
        // OldFunc->replaceAllUsesWith(fv);
      }
      // You can handle other cases like indirect calls or function pointers here if needed
    }

    for(auto *CI : toDelete) {
      CI->eraseFromParent();
    }

    // Optionally, you can delete the old function if no longer needed
    // if (OldFunc->use_empty()) {
    //     OldFunc->eraseFromParent();
    // }
    // OldFunc->eraseFromParent();
  }

  virtual bool runOnModule(Module &M) {
    bool Changed = false;

    std::vector<Function*> functionsToDelete;

    for (Function &F : M) {
      if (F.isDeclaration()) continue;

      SmallVector<unsigned, 4> DeadArgs;
      for (unsigned i = 0, e = F.arg_size(); i != e; ++i) {
        Argument *Arg = F.getArg(i);
        if (isDead(F, *Arg)) {
          User *User = *Arg->user_begin();
          Instruction* storeInst = dyn_cast<Instruction>(User);
          Instruction* allocaInst = dyn_cast<Instruction>(storeInst->getOperand(1));
          allocaInst->eraseFromParent();
          storeInst->eraseFromParent();
          errs() << F << "\n";
          F.setAttributes(F.getAttributes().removeParamAttributes(F.getContext(), i));
          DeadArgs.push_back(i);
        }
      }
    

      if (DeadArgs.empty()) {
        continue;
      }

      Changed = true;

      std::vector<Type *> ArgTypes;
      std::vector<int> ArgIndices;
      for (auto &Arg : F.args()) {
        if (std::find(DeadArgs.begin(), DeadArgs.end(), Arg.getArgNo()) == DeadArgs.end()) {
          ArgTypes.push_back(Arg.getType());
          ArgIndices.push_back(Arg.getArgNo());
        }
      }

      FunctionType *FTy = FunctionType::get(F.getReturnType(), ArgTypes, F.isVarArg());
      Function *NewF = Function::Create(FTy, F.getLinkage(), F.getName(), &M);
      NewF->copyAttributesFrom(&F);

      for (BasicBlock &BB : llvm::make_early_inc_range(F)) {
          BB.moveBefore(&*NewF->begin());
      }

      // errs() << "##############################\n" << NewF << "\n" << "############################BEFORE##\n";
      // F.replaceAllUsesWith(NewF);
      replaceFunctionUses(&F, NewF, ArgIndices);
      // errs() << "##############################\n" << M << "\n" << "#############################AFTER##\n";

      // F.eraseFromParent();
      functionsToDelete.push_back(&F);
    }


    for(Function* f :  functionsToDelete)
      f->eraseFromParent();

      errs() << M << "\n";

    return Changed;
  }

}; 

char DeadArgumentElimination::ID = 0;



static RegisterPass<DeadArgumentElimination> X("dead-arg-elim", "Dead Argument Elimination Pass", false, false);
