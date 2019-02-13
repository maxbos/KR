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
  vector<vector<int> > clauses;
  vector<int> getLiterals                                           (vector<int>);
  tuple<vector<vector<int> >, vector<int>, bool> unitPropagate      (vector<vector<int> >, vector<int>);

public:
  DavisPutnam                         (string strategy, vector<vector<int> > clauses);
  vector<int> recursive               (vector<vector<int> >, vector<int>);
  void simplify                       ();
};

int main() {
  vector<vector<int> > clauses = readDimacsFile("resources/inputfile.txt");
  DavisPutnam davisPutnam("S1", clauses);
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
  while (dimacsFile >> literal) {
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

DavisPutnam::DavisPutnam(string strategy, vector<vector<int> > clauses)
  : strategy(strategy), clauses(clauses) {

}

vector<int> DavisPutnam::recursive(vector<vector<int> > clauses, vector<int> assignments) {
  bool emptyClause;

  tie(clauses, assignments, emptyClause) = unitPropagate(clauses, assignments);
  // When the set of clauses contains an empty clause, the problem is unsatisfiable.
  if (emptyClause) {
    return {};
  }
  // We have found a successfull assignment when we have no clauses left.
  if (clauses.empty()) {
    return assignments;
  }
  // We perform the branching step by picking a literal that is not yet included
  // in out partial assignment.
  int literal = getNextLiteral(getLiterals(assignments));
  if (recursive(clauses, assignments.push_back(literal))) {
    return assignments;
  }
  // Call the recursive method with the literal having assigned a False value.
  return recursive(clauses, assignments.push_back(literal * -1));
}

// Calculates the absolute (TRUE) values of each assignment, which represents
// each literal.
vector<int> DavisPutnam::getLiterals(vector<int> assignments) {
  vector<int> literals;
  for (auto& el : assignments) {
    literals.push_back(abs(el));
  }
  return literals;
}
