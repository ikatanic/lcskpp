all:
	g++ -o test test.cpp -O2 -std=c++11

clean:
	rm -f test

