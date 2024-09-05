cmake_minimum_required(VERSION 3.22)
project(
    KonstrukcijaKompilatoraProjekat
)

add_subdirectory(AlwaysInlinePass)
add_subdirectory(DeadArgumentEliminationPass)
add_subdirectory(IdenticalBBMergePass)
add_subdirectory(ImplicitMulOperandUpcastPass)
add_subdirectory(LoopInvariantCodeMotionPass)
