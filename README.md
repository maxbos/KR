# Generating X-Sudokus from regular Sudokus

SAT solver written in C++ that solves sudokus in DIMACS format and can generate x-sudokus (sudokus with diagonal constraints) from regular sudokus.

## How to run
**Compile using g++**
```
g++ -std=c++11 -o SAT main.cpp
```
Run with arguments
```
./SAT [-S1,-S2,-S3] [inputfile]
```
Optionally specify an input directory and the number of files (`numfiles`) in that directory, which are named [0.txt, ..., `numfiles`.txt]
```
./SAT [-S1,-S2,-S3] [inputdirectory] [numfiles]
```

**Compile and run using Xcode (macOS only)**  
Just open the `SAT.xcodeproj` file

