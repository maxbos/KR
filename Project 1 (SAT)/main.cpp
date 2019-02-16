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
  tuple<vector<vector<int> >, vector<int>> setup                    (vector<vector<int> >);
  vector<int> recursive                                             (vector<vector<int> >, vector<int>);
  tuple<vector<vector<int> >, vector<int>> unitPropagate            (vector<vector<int> >, vector<int>);
  vector<vector<int> > removeTautologies                            (vector<vector<int> >);
  vector<vector<int> > removeItemsByIndices                         (vector<vector<int> >, vector<int>);
  int getNextLiteral                                                (vector<int>);
  vector<int> getLiterals                                           (vector<int>);
  bool containsEmptyClause                                          (vector<vector<int> >);

public:
  DavisPutnam                         (string strategy, vector<vector<int> > clauses);
};

int main() {
  // TODO load the inputfile dynamically
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

  vector<vector<int> > clauses;

  string skip;
  // Skip the first 4 strings in the first line
  dimacsFile >> skip >> skip >> skip >> skip;

  int literal;
  int prevLiteral = 0;
  vector<int> *lastClause;

  while (dimacsFile >> literal) {
    if (literal != 0) {
      if (prevLiteral == 0) {
        vector<int> clause = { literal };
        clauses.push_back(clause);
        lastClause = &clauses.back();
      } else {
        lastClause->push_back(literal);
      }
    }
    prevLiteral = literal;
  }
  dimacsFile.close();
  return clauses;
}

// Prints a formula of clauses, where each clause is printed
// on its individual line.
void printClauses(vector<vector<int> > clauses) {
  for (vector<int> const& clause: clauses) {
    for (int const& literal: clause) {
      cout << literal << " + ";
    }
    cout << endl;
  }
}

DavisPutnam::DavisPutnam(string strategy, vector<vector<int> > clauses)
: strategy(strategy), clauses(clauses) {
  // Initialize the recursive Davis Putnam algorithm with an empty set
  // of assignments.
  vector<int> assignments;
  tie(clauses, assignments) = setup(clauses);
  vector<int> finalAssignments = recursive(clauses, assignments);
  for (auto const& i: finalAssignments) {
    cout << i << " ";
  }
}

// Perform essential steps before starting the recursive Davis Putnam algorithm,
// such as removing tautologies from the initial Formula, and assigning the
// first literal.
tuple<vector<vector<int> >, vector<int>> DavisPutnam::setup(vector<vector<int> > F) {
  F = removeTautologies(F);
  vector<int> assignments = { getNextLiteral(F) };
  return make_tuple(F, assignments);
}

vector<int> DavisPutnam::recursive(vector<vector<int> > clauses, vector<int> assignments) {
  cout << " recursive call " << endl;
  tie(clauses, assignments) = unitPropagate(clauses, assignments);
  cout << "number of clauses left: " << clauses.size() << endl;
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
  // Save indices of clauses that can be removed and add those variables
  // to the partial assignments list
  vector<int> removeIndices;
  for (int i = 0; i < F.size(); i++) {
    if (F[i].size() == 1) {
      removeIndices.push_back(i);
      assignments.push_back(F[i][0]);
    }
  }
  F = removeItemsByIndices(F, removeIndices);
  return make_tuple(F, assignments);
}

vector<vector<int> > DavisPutnam::removeTautologies(vector<vector<int> > F) {
  vector<int> removeIndices;
  for (int i = 0; i < F.size(); i++) {
    vector<int> const& clause = F[i];
    for (int const& literal : clause) {
      if (find(clause.begin(), clause.end(), -literal) != clause.end()) {
        removeIndices.push_back(i);
      }
    }
  }
  return removeItemsByIndices(F, removeIndices);
}

vector<vector<int> > DavisPutnam::removeItemsByIndices(vector<vector<int> > F, vector<int> removeIndices) {
  // Sort the indices such that we can remove the highest indices first
  sort(removeIndices.begin(), removeIndices.end(), greater<int>());
  // Remove the clauses from the formula list
  for (auto const& index : removeIndices) {
    F.erase(F.begin() + index);
  }
  return F;
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
  for (auto const& el : assignments) {
    literals.push_back(abs(el));
  }
  return literals;
}

// Checks whether a given set of clauses contains an empty clause.
bool DavisPutnam::containsEmptyClause(vector<vector<int> > F) {
  return find_if(F.begin(), F.end(), [](vector<int> const& clause) {
    return clause.empty();
  }) != F.end();
}
