/*
 * main.cpp
 * Knowledge Representation: Project 1 (SAT)
 *
 * Studentnames:     Max Bos & Erik Stammes
 * Studentnumbers:   10669027 & 10559736
 */

#include <fstream>
#include <iostream>
#include <cstdlib>
#include <string>
#include <vector>

using namespace std;

class DavisPutnam {
  string strategy;
  string inputFilePath;
public:
  DavisPutnam           (string strategy, string inputFilePath);
  void simplify         ();
};

vector< vector<int> > readDimacsFile(string loc) {
    ifstream dimacsFile;
    dimacsFile.open(loc);

    vector<int> empty_vec;
    vector< vector<int> > clauses;
    clauses.push_back(empty_vec);

    string skip;
    // Skip the first 4 strings in the first line
    dimacsFile >> skip >> skip >> skip >> skip;

    int clause = 0;
    int literal;
    while(dimacsFile >> literal) {
        if (literal == 0) {
            clause++;
            clauses.push_back(empty_vec);
            continue;
        } 
        clauses[clause].push_back(literal);
    }
    dimacsFile.close();
    return clauses;
}

void printClauses(vector< vector<int> > clauses) {
  for (int i = 0; i < clauses.size(); i++) {
    for (int j = 0; j < clauses[i].size(); j++) {
        cout<<clauses[i][j]<<" + ";
    }
    cout<<endl;
  }
}

int main() {
  vector< vector<int> > clauses = readDimacsFile("resources/sudoku-rules.txt");
  printClauses(clauses);
  DavisPutnam davisPutnam("S1", "./iets");
  
  return 0;
}

DavisPutnam::DavisPutnam(string strategy, string inputFilePath)
  : strategy(strategy), inputFilePath(inputFilePath) {

}

void DavisPutnam::simplify() {

}
