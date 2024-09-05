# LLVM Optimizations

Projekat za kurs Konstrukcija Kompilatora na Matematičkom fakultetu Univerzita u Beogradu

## Odrađene optimizacije

- [Always Inline Pass](/AlwaysInlinePass/)
- [Dead Argument Elimination Pass](/DeadArgumentEliminationPass/)
- [Identical (Duplicate) Block Merge Pass](/IdenticalBBMergePass/)
- [Implicit MulOperand Upcast Pass](/ImplicitMulOperandUpcastPass/)
- [Loop Invariant Code Motion Pass](/LoopInvariantCodeMotionPass/)

## Potrebne biblioteke
Instalirati sledeće alate:
- llvm
- clang
- cmake

_Na Ubuntu masini moze se ukucati:_ <br>
`sudo apt install llvm clang cmake`

## Kako generisati IR

Obezbediti prava izvršavanja pomoćnoj skripti: <br>
`chmod +x generate_ir.sh`
<br>

Zatim za svako generisanje IR-a pokrenuti: <br>
`./generate_ir.sh`

Skripta je interkativna.