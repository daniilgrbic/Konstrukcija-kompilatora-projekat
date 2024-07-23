# Function inliner in LLVM

Projekat za kurs Konstrukcija Kompilatora na Matematičkom fakultetu Univerzita u Beogradu

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
`./generate_ir.sh IME_FAJLA.cpp`

_Za promenu primera, promeniti promenljivu `IME_FAJLA`_
