# Testing

## Comparing to baseline implementation

`tester` compares this implementation to `https://github.com/fpavetic/lcskpp`.

Assumes latter is cloned into `../../lcskpp_pavetic/lcskpp`.

Ex. `./tester 1 20 40 90` will run generate tests with k taking all values from [1, 20] and
similarity taking values 40%, 50%, 60%, 70%, 80% and 90%.



## Test files
To generate various test data files use `gen.sh` script:
```
$ . gen.sh
```

Test `x` consists of two files `tests/test.x.A` and `tests/test.x.B`, each file contains
one string.

#### Tests summary

Test id | N (string length) | alphabet size | p_similar | format |
------------ | ------------- | ----------- | -----------
1 | 10000 | 4 | 40% | plaintext
2 | 10000 | 4 | 50% | plaintext
3 | 10000 | 4 | 60% | plaintext
4 | 10000 | 4 | 70% | plaintext
5 | 10000 | 4 | 80% | plaintext
6 | 10000 | 4 | 90% | plaintext
7 | 100000 | 4 | 70% | plaintext
8 | 1000000 | 4 | 90% | plaintext
9 | 10000 | 2 | 50% | plaintext
10 | 100000 | 5 | 60% | plaintext
11 | 100000 | 8 | 70% | plaintext
12 | 1000000 | 16 | 80% | plaintext
13 | 1000000 | 26 | 80% | plaintext
14 | 1000 | 3 | 95% | plaintext
15 | 100 | 2 | 95% | plaintext
16 | 10000 | 4 | 70% | FASTA
17 | 100000 | 4 | 70% | FASTA
17 | 1000000 | 4 | 70% | FASTA
