all:
	g++ -o main lcskpp.cpp main.cpp -O2 -std=c++11

clean:
	rm -f tester
