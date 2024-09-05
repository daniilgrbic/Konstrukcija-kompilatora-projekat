#include "llvm/Pass.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"

#include <vector>
#include <set>
#include <map>

using namespace llvm;

struct IdenticalBBMergePass : public FunctionPass {   

    static char ID;

    IdenticalBBMergePass() : FunctionPass(ID) {}

    std::set<BasicBlock*> BlocksMarkedForDeletion;
    std::map<Value*, Value*> IdenticalInstructions;

    /**
     * Counts instructions in a given basic block, skipping debug instructions.
     */
    unsigned instructionCount(BasicBlock* BB) {
        
        return std::distance(
            BB->instructionsWithoutDebug().begin(),
            BB->instructionsWithoutDebug().end()
        );
    }

    /**
     * Zips instructions from two given basic blocks and returns a vector of pairs of pointers to paired
     * instructions. The function assumes both basic blocks have the same number of instructions.
     */
    std::vector<std::pair<Instruction*, Instruction*>> zipBBIterator(BasicBlock* BB1, BasicBlock* BB2) {

        auto BB1IteratorRange = BB1->instructionsWithoutDebug();
        auto BB2Iterator = BB2->instructionsWithoutDebug().begin();

        std::vector<std::pair<Instruction*, Instruction*>> Result;
        for(auto BB1Iterator = BB1IteratorRange.begin(); 
            BB1Iterator != BB1IteratorRange.end(); 
            BB1Iterator++, BB2Iterator++) {

            Result.push_back({&*BB1Iterator, &*BB2Iterator});
        }

        return Result;
    }

    /**
     * Determines if instructions I1 and I2 can be merged. 
     * 
     * After some basic checks (operation type, operand count), the function compares the operands. 
     * Two operands are considered the same if they are completely identical, or represent the same
     * value. 
     * 
     * E.g. consider the following sequence of instructions: `c = a + b; d = a + b;` Although `c`
     * and `d` are not the same LLVM IR variable, they still represent the same value, and two
     * instructions utilizing `c` and `d` in the future can also be considered identical.
     * 
     * For the last part, the function utilizes previous results from `IdenticalInstructions`.
     */
    bool canMergeInstructions(Instruction* Instruction1, Instruction* Instruction2) {

        if(not Instruction1->isSameOperationAs(Instruction2))
            return false;

        if(Instruction1->getNumOperands() != Instruction2->getNumOperands())
            return false;

        for(unsigned OperandIndex = 0; OperandIndex != Instruction1->getNumOperands(); OperandIndex++) {

            if(Instruction1->getOperand(OperandIndex) == Instruction2->getOperand(OperandIndex))
                continue;

            if(IdenticalInstructions[Instruction1->getOperand(OperandIndex)] == Instruction2->getOperand(OperandIndex))
                continue;

            return false;
        }

        IdenticalInstructions.insert({Instruction1, Instruction2});
        IdenticalInstructions.insert({Instruction2, Instruction1});

        return true;
    }

    /**
     * Given two identical basic blocks with common predecessors, this function updates the last
     * instruction of each predecessor of the erased block to instead jump to the kept block.
     */
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

        // Entry block can't have duplicates and thus can't be merged
        if(BB1->isEntryBlock())
            return false;
        
        // Skip last block in the function (if last instruction is ret and not branching), 
        // also skip blocks with more than one successor for simplicity
        BranchInst* BB1Terminator = dyn_cast<BranchInst>(BB1->getTerminator());
        if(not BB1Terminator || BB1Terminator->isConditional())
            return false;

        // We want to compare BB1 to blocks which share a common successor with BB1
        for(BasicBlock* BB2 : predecessors(BB1Terminator->getSuccessor(0))) {

            // As with BB1, we check that the last instruction in BB2 is an unconditional jump
            BranchInst *BB2Terminator = dyn_cast<BranchInst>(BB2->getTerminator());
            if(not BB2Terminator || BB2Terminator->isConditional())
                continue;

            if(BB1 == BB2)
                continue;

            if(BlocksMarkedForDeletion.count(BB2))
                continue;

            if(instructionCount(BB1) != instructionCount(BB2))
                continue;

            IdenticalInstructions.clear();
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
            BlocksMarkedForDeletion.insert(BB1);
            return true;
        }

        return false;
    }

    virtual bool runOnFunction(Function& F) {

        bool Changed = false;

        for(BasicBlock& BB : F) {
            Changed |= mergeDuplicateBlocks(&BB);
        }

        for(BasicBlock* BB : BlocksMarkedForDeletion) {
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
