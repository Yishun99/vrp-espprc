# vrp-espprc

Elementary Shortest Path Problem with Resource Constrains.

A sub-problem of Vehicle Route Problem.

Backtracking + Branch and Bound

## Introduction
C++ implementation of "An Exact Algorithm for the Elementary Shortest Path Problem with Resource Constraints".




## Compile

`g++ -std=c++11 main.cpp espprc.cpp graph.cpp -o vrp_espprc`

or

`cmake . && make`

## Run

./vrp_espprc data/solomon_100/***.txt
