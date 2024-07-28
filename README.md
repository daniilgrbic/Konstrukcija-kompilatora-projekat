# LLVM Optimizations

Projekat za kurs Konstrukcija Kompilatora na Matematičkom fakultetu Univerzita u Beogradu

# Odrađene optimizacije

- Function Inliner
- Dead Argument Elimination

# Potrebne biblioteke
Instlirati sledece alate:
- llvm
- clang
- cmake

_Na Ubuntu masini moze se ukucati:_ <br>
`sudo apt install llvm clang cmake`

# Kako generisati IR

Omoguciti prava pristupa skripti: <br>
`chmod +x generate_ir.sh`
<br>

Zatim za svako generisanje IR-a pokrenuti: <br>
`./generate_ir.sh IME_FOLDERA IME_EXAMPLE_FAJLA`

_Za promenu pass-a, promeniti `IME_FOLDERA`_
_Za promenu primera, promeniti promenljivu `IME_EXAMPLE_FAJLA`_

<b>
Primer: </b><br>

`./generate_ir.sh AlwaysInlinePass InlinePrint`