all: tester pavetic

tester:
	g++ -o tester ../lcskpp.cpp ../../lcskpp_pavetic/lcskpp/lcskpp.cpp tester.cpp -O2 -std=c++11

pavetic:
	g++ -o pavetic ../../lcskpp_pavetic/lcskpp/lcskpp.cpp pavetic_runner.cpp -O2 -std=c++11

clean:
	rm -f tester pavetic

