all:
	g++ -o main lcskpp.cpp main.cpp -O2 -std=c++11
	g++ -o main_fasta lcskpp.cpp main_fasta.cpp -O2 -std=c++11

clean:
	rm -f main main_fasta
