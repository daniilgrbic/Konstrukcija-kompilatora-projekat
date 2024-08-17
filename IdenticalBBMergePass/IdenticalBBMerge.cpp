#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Module.h"
#include "llvm/Transforms/Utils/Cloning.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"

#include <vector>
#include <set>
#include <map>

using namespace llvm;

struct IdenticalBBMergePass : public FunctionPass {   

    static char ID;

    IdenticalBBMergePass() : FunctionPass(ID) {}

    std::set<BasicBlock*> MarkedBlocks;

    std::map<Value*, Value*> IdentialInstructions;

    unsigned instructionCount(BasicBlock* BB) {

        unsigned Count = 0;

        for(Instruction& Instr : BB->instructionsWithoutDebug()) {
            Count++;
        }

        return Count;
    }

    std::vector<std::pair<Instruction*, Instruction*>> 
    zipBBIterator(BasicBlock* BB1, BasicBlock* BB2) {

        std::vector<std::pair<Instruction*, Instruction*>> Result;
        auto BB1IteratorRange = BB1->instructionsWithoutDebug();
        auto BB2Iterator = BB2->instructionsWithoutDebug().begin();

        for(auto BB1Iterator = BB1IteratorRange.begin(); 
            BB1Iterator != BB1IteratorRange.end(); 
            BB1Iterator++, BB2Iterator++) {

            Result.push_back({&*BB1Iterator, &*BB2Iterator});
        }

        return Result;
    }

    bool canMergeInstructions(Instruction* Instruction1, Instruction* Instruction2) {

        if(not Instruction1->isSameOperationAs(Instruction2))
            return false;

        if(Instruction1->getNumOperands() != Instruction2->getNumOperands())
            return false;

        for(unsigned OperandIndex = 0; OperandIndex != Instruction1->getNumOperands(); OperandIndex++) {

            if(Instruction1->getOperand(OperandIndex) == Instruction2->getOperand(OperandIndex))
                continue;

            if(IdentialInstructions[Instruction1->getOperand(OperandIndex)] == Instruction2->getOperand(OperandIndex))
                continue;

            return false;
        }

        IdentialInstructions.insert({Instruction1, Instruction2});
        IdentialInstructions.insert({Instruction2, Instruction1});

        return true;
    }

    void updateBranchSuccessors(BasicBlock* BBErase, BasicBlock* BBKeep) {

        for(BasicBlock *BB : predecessors(BBErase)) {

            Instruction* Terminator = BB->getTerminator();

            for(unsigned OperandIndex = 0; OperandIndex < Terminator->getNumOperands(); OperandIndex++) {
                if(Terminator->getOperand(OperandIndex) == BBErase) {
                    Terminator->setOperand(OperandIndex, BBKeep);
                }
            }
        }
    }

    bool mergeDuplicateBlocks(BasicBlock* BB1) {

        if(&BB1->getParent()->getEntryBlock() == BB1)
            return false;
        
        BranchInst* BB1Terminator = dyn_cast<BranchInst>(BB1->getTerminator());
        if(not BB1Terminator || BB1Terminator->isConditional())
            return false;

        for(BasicBlock* BB2 : predecessors(BB1Terminator->getSuccessor(0))) {

            if(&BB2->getParent()->getEntryBlock() == BB2)
                continue;

            BranchInst *BB2Terminator = dyn_cast<BranchInst>(BB2->getTerminator());
            if(not BB2Terminator || BB2Terminator->isConditional())
                continue;

            if(BB1 == BB2)
                continue;

            if(MarkedBlocks.count(BB2))
                continue;

            if(instructionCount(BB1) != instructionCount(BB2))
                continue;

            IdentialInstructions.clear();
            bool CanMergeAllInstructions = true;

            for(std::pair<Instruction*, Instruction*> PI : zipBBIterator(BB1, BB2)) {
                if(not canMergeInstructions(PI.first, PI.second)) {
                    CanMergeAllInstructions = false;
                    break;
                }
            }

            if(not CanMergeAllInstructions)
                continue;

            updateBranchSuccessors(BB1, BB2);
            MarkedBlocks.insert(BB1);
            return true;
        }

        return false;
    }

    virtual bool runOnFunction(Function& F) {

        bool Changed = false;

        for(BasicBlock& BB : F) {
            Changed |= mergeDuplicateBlocks(&BB);
        }

        for(BasicBlock* BB : MarkedBlocks) {
            DeleteDeadBlock(BB);
        }

        return Changed;
    }
};

char IdenticalBBMergePass::ID = 0;

static RegisterPass<IdenticalBBMergePass> X(
    "identical-bb-merge",
    "Idential Basic Block Merge Pass",
    false,
    false
);
