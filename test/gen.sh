mkdir tests

g++ gen.cpp -std=c++0x -o gen
g++ gen_fasta.cpp -std=c++0x -o gen_fasta

# plaintext
./gen 10000 4 40 tests/test.1
./gen 10000 4 50 tests/test.2
./gen 10000 4 60 tests/test.3
./gen 10000 4 70 tests/test.4
./gen 10000 4 80 tests/test.5
./gen 10000 4 90 tests/test.6
./gen 100000 4 70 tests/test.7
./gen 1000000 4 90 tests/test.8
./gen 10000 2 50 tests/test.9
./gen 100000 5 60 tests/test.10
./gen 100000 8 70 tests/test.11
./gen 1000000 16 80 tests/test.12
./gen 1000000 26 80 tests/test.13
./gen 1000 3 95 tests/test.14
./gen 100 2 95 tests/test.15
# FASTA
./gen_fasta 10000 4 70 tests/test.16.fa
./gen_fasta 100000 4 70 tests/test.17.fa
./gen_fasta 1000000 4 70 tests/test.18.fa

rm gen gen_fasta
