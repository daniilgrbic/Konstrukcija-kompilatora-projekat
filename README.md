# LLVM Optimizations

Projekat za kurs Konstrukcija Kompilatora na Matematičkom fakultetu Univerzita u Beogradu

# Odrađene optimizacije

- Always Inline Pass
- Dead Argument Elimination Pass
- Identical (Duplicate) Block Merge Pass

# Potrebne biblioteke
Instalirati sledeće alate:
- llvm
- clang
- cmake

_Na Ubuntu masini moze se ukucati:_ <br>
`sudo apt install llvm clang cmake`

# Kako generisati IR

Obezbediti prava izvršavanja pomoćnoj skripti: <br>
`chmod +x generate_ir.sh`
<br>

Zatim za svako generisanje IR-a pokrenuti: <br>
`./generate_ir.sh`

Skripta je interkativna.