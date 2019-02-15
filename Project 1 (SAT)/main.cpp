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
#include <algorithm>
#include <iterator>

using namespace std;

vector<vector<int> > readDimacsFile   (string);
void printClauses                     (vector<vector<int> >);

class DavisPutnam {
  string strategy;
  string inputFilePath;
  vector<vector<int> > clauses;
  tuple<vector<vector<int> >, vector<int>> unitPropagate (vector<vector<int> >, vector<int>);
  int getNextLiteral                                                (vector<int>);
  vector<int> getLiterals                                           (vector<int>);
  bool containsEmptyClause                                          (vector<vector<int> >);

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
  // Initialize the recursive Davis Putnam algorithm with an empty set
  // of assignments.
  vector<int> assignments = recursive(clauses, {});
  for (auto const& i: assignments) {
    cout << i << " ";
  }
}

vector<int> DavisPutnam::recursive(vector<vector<int> > clauses, vector<int> assignments) {
  tie(clauses, assignments) = unitPropagate(clauses, assignments);
  // When the set of clauses contains an empty clause, the problem is unsatisfiable.
  if (containsEmptyClause(clauses)) return {};
  // We have found a successfull assignment when we have no clauses left.
  if (clauses.empty()) return assignments;
  // We perform the branching step by picking a literal that is not yet included
  // in out partial assignment.
  int literal = getNextLiteral(getLiterals(assignments));
  assignments.push_back(literal);
  if (!recursive(clauses, assignments).empty()) return assignments;
  // Re-set the last assignment to its counterpart value, the False assignment.
  int lastAssignmentIndex = assignments.size()-1;
  assignments[lastAssignmentIndex] = assignments[lastAssignmentIndex] * -1;
  // Call the recursive method with the literal having assigned a False value.
  return recursive(clauses, assignments);
}

tuple<vector<vector<int> >, vector<int>> DavisPutnam::unitPropagate(
  vector<vector<int> > F, vector<int> assignments
) {
  F.erase(remove_if(F.begin(), F.end(), [](vector<int> n) {return n.size() == 1;}), F.end());
  return make_tuple(F, assignments);
}

// Based on the set heuristic, pick the next literal to branch into.
int DavisPutnam::getNextLiteral(vector<int> currentLiterals) {
  if (strategy == "S2")
    return 111;
  else if (strategy == "S3")
    return 111;
  return 111;
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

// Checks whether a given set of clauses contains an empty clause.
bool DavisPutnam::containsEmptyClause(vector<vector<int> > F) {
  return find_if(F.begin(), F.end(), [](const vector<int> n) {
    return n.empty();
  }) != F.end();
}
