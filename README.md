# LCSk++

This is repository for [Bioinformatics](http://www.fer.unizg.hr/predmet/bio) course project at University of Zagreb.

It's a faster implementation of LCSk++ algorithm introduced in [1].

### How to use

First, build the project using `make`. For that you'll need C++11 compatible
compiler. Then use `./main` or `./main_fasta` executables. Both of them read
two strings from given files and output their LCSk++ to a file. Only difference
is that the first one takes raw text files while the second one takes files in FASTA
format. Running them with no arguments will give you more details on how to use
them.

### Using it in your own project

Files you are going to need to include in your project are:
- lcskpp.cpp
- lcskpp.h
- utils.h

Only one method is exposed, and it's given in `lcskpp.h`.

### Testing

See `test/README.md`.

### References
[1] Filip Pavetic, Goran Zuzic, Mile Sikic: _LCSk++: Practical similarity metric for long strings_, http://arxiv.org/abs/1407.2407  
