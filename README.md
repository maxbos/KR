# Generating X-Sudokus from regular Sudokus

SAT solver written in C++ that solves sudokus in DIMACS format and can generate x-sudokus (sudokus with diagonal constraints) from regular sudokus.

## How to run
**Using binary (Linux or macOS)**  
Skip the compilation step, run the binary as specified below. Unfortunately, we don't have access to a Windows environment so no Windows binary is available. In all cases it's best to compile the code yourself to avoid compilation differences between computers.   
**Compile using g++**
```
g++ -std=c++14 -o SAT main.cpp
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

