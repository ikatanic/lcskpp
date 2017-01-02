all:
	g++ -o tester pavetic.cpp tester.cpp -O2 -std=c++11

clean:
	rm -f tester

