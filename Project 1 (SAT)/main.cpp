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

vector<vector<int> > readDimacsFile   (string);
void printClauses                     (vector<vector<int> >);

class DavisPutnam {
  string strategy;
  string inputFilePath;
  pair<vector<vector<int> >, vector<int> > unitPropagate (vector<vector<int> >, vector<int>);
public:
  DavisPutnam                         (string strategy, string inputFilePath);
  void recursive                      (vector<vector<int> >, vector<int>);
  void simplify                       ();
};

int main() {
  vector<vector<int> > clauses = readDimacsFile("resources/inputfile.txt");
  DavisPutnam davisPutnam("S1", "./iets");
  return 0;
}

// Reads and parses a DIMACS file from a given file location.
// The DIMACS clauses are stored as a (formula) vector of clause
// vectors.
vector<vector<int> > readDimacsFile(string loc) {
  ifstream dimacsFile;
  dimacsFile.open(loc);

  vector<int> empty_vec;
  vector<vector<int> > clauses;
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

// Prints a formula of clauses, where each clause is printed
// on its individual line.
void printClauses(vector<vector<int> > clauses) {
  for (int i = 0; i < clauses.size(); i++) {
    for (int j = 0; j < clauses[i].size(); j++) {
      cout << clauses[i][j] << " + ";
    }
    cout << endl;
  }
}

DavisPutnam::DavisPutnam(string strategy, string inputFilePath)
  : strategy(strategy), inputFilePath(inputFilePath) {

}

// pair<vector<vector<int>>, vector<int>> DavisPutnam::unitPropagate(
//   vector<vector<int>> F, vector<int> partialAssignments
// ) {
//   int numberOfClauses = F.size();
//   for (int i = 0; i < numberOfClauses; i++) {
//     if (F[i].empty()) {
//       return make_pair({{}}, {});
//     }
//   }
// }

void DavisPutnam::recursive(vector<vector<int> > F, vector<int> partialAssignments) {
  simplify();
  // split();
  // backtrack();
}

void DavisPutnam::simplify() {

}
