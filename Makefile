all:
	g++ -o lcskpp lcskpp.cpp pavetic.cpp -O2 -std=c++11

clean:
	rm -f lcskpp

