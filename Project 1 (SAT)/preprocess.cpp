/*
 * preprocess.cpp
 * Knowledge Representation: Project 1 (SAT)
 *
 * This program expects DIMACS filepaths as arguments, and
 * concatenates these files into one DIMACS file.
 * The last argument is the output file name.
 * 
 * Studentnames:     Max Bos & Erik Stammes
 * Studentnumbers:   10669027 & ...
 */

#include <fstream>
#include <iostream>
#include <cstdlib>
#include <string>

using namespace std;

int main(int argc, char** argv) {
  ofstream ofile(argv[argc-1]);

  for (int i = 1; i < argc-1; ++i) {
    ifstream ifile(argv[i]);
    ofile << ifile.rdbuf();
  }
  
  return 0;
}
