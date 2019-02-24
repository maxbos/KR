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
#include <array>
#include <set>
#include <ctime>

using namespace std;

vector<vector<int> > readDimacsFile   (string);
void printClauses                     (vector<vector<int> >);

class DavisPutnam {
    string strategy;
    string inputFilePath;
    vector<vector<int> > clauses;
    vector<vector<int> > setup                                          (vector<vector<int> >);
    set<int> recursive                                                  (vector<vector<int> >, set<int>);
    tuple<vector<vector<int> >, set<int> > unitPropagate                (vector<vector<int> >, set<int>);
    vector<vector<int> > simplify                                       (vector<vector<int> >, int);
    vector<vector<int> > removeTautologies                              (vector<vector<int> >);
    vector<vector<int> > removeItemsByIndices                           (vector<vector<int> >, vector<int>);
    int getNextLiteral                                                  (vector<vector<int> >, set<int>);
    set<int> getVariables                                               (set<int>);
    bool containsEmptyClause                                            (vector<vector<int> >);
    
public:
    DavisPutnam                         (string strategy, vector<vector<int> > clauses);
};

int main() {
    // TODO load the inputfile dynamically
    vector<vector<int> > clauses = readDimacsFile("resources/1000-sudokus/1001.txt");
//     printClauses(clauses);
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
    time_t tstart, tend;
    tstart = time(0);
    // Initialize the recursive Davis Putnam algorithm with an empty set
    // of assignments.
    set<int> assignments;
    clauses = setup(clauses);
    set<int> finalAssignments = recursive(clauses, assignments);
    tend = time(0);
    cout << "It took "<< difftime(tend, tstart) <<" second(s)."<< endl;
    cout << "Number of assignments is: " << finalAssignments.size() << endl;
    cout << "The final assignment is: ";
    int count = 0;
    for (auto const& i: finalAssignments) {
        if (i > 0) {
            count++;
            cout << i << " ";
        }
    }
    cout << endl << "Number of positive assignments: " << count << endl;
    cout << endl;
}

// Perform essential steps before starting the recursive Davis Putnam algorithm,
// such as removing tautologies from the initial Formula.
vector<vector<int> > DavisPutnam::setup(vector<vector<int> > F) {
    F = removeTautologies(F);
    return F;
}

set<int> DavisPutnam::recursive(vector<vector<int> > clauses, set<int> assignments) {
    vector<vector<int> > F;
//    tie(F, assignments) = pureLiterals(clauses, assignments);
    tie(F, assignments) = unitPropagate(clauses, assignments);
    // When the set of clauses contains an empty clause, the problem is unsatisfiable.
    if (containsEmptyClause(F)) return {};
    // We have found a successfull assignment when we have no clauses left.
    if (F.empty()) return assignments;
    // We perform the branching step by picking a literal that is not yet included
    // in our partial assignment.
    int literal = getNextLiteral(F, getVariables(assignments));
    // Split into the TRUE value for the new variable.
    F.push_back({ literal });
    assignments.insert(literal);
    set<int> leftSplitAssignments = recursive(F, assignments);
    if (!leftSplitAssignments.empty()) return leftSplitAssignments;
    // If the TRUE value branching step did not yield a successfull assignment,
    // we try the FALSE value for the same variable.
    F[F.size()-1] = { -literal };
    assignments.erase(literal);
    assignments.insert(-literal);
    return recursive(F, assignments);
}

// Iterate through the clauses and maintain a state for which literals no counterpart is found.
//tuple<vector<vector<int> >, vector<int>> DavisPutnam::pureLiterals(
//                                                                   vector<vector<int> > F, vector<int> assignments
//                                                                   ) {
//    vector<int> foundLiterals;
//    vector<int> discardLiterals;
//    for (auto const& clause : F) {
//        for (auto const& literal : clause) {
//            // If the literal is in our discardList we discard this literal.
//            if (find(discardLiterals.begin(), discardLiterals.end(), literal) != discardLiterals.end()) {
//                continue;
//            }
//            // If the opposite of the literal is in our list we discard this literal
//            // and we remove the literal from our foundLiterals.
//            if (find(foundLiterals.begin(), foundLiterals.end(), -literal) != foundLiterals.end()) {
//                discardLiterals.push_back(literal);
//                discardLiterals.push_back(-literal);
//                foundLiterals.erase(remove(foundLiterals.begin(), foundLiterals.end(), literal), foundLiterals.end());
//                foundLiterals.erase(remove(foundLiterals.begin(), foundLiterals.end(), -literal), foundLiterals.end());
//            // If the literal is not yet in our list we add it.
//            } else if (!(find(foundLiterals.begin(), foundLiterals.end(), literal) != foundLiterals.end())) {
//                foundLiterals.push_back(literal);
//            }
//        }
//    }
//    vector<vector<int> > newF = F;
//    for (auto const& clause : F) {
//        for (auto const& literal : clause) {
//            // If the literal is in our foundLiterals list we
//            if (find(foundLiterals.begin(), foundLiterals.end(), literal) != foundLiterals.end()) {
//                int const literal = clause[0];
//                // Simplify the Formula by removing all clauses containing the `literal`
//                // and by removing the literal from a clause where it is `-literal`.
//                newF = simplify(newF, literal);
//                assignments.push_back(literal);
//            }
//        }
//    }
//    return make_tuple(newF, assignments);
//}

// For each unit clause in the Formula, set the literal from that clause to TRUE,
// and simplify the Formula.
tuple<vector<vector<int> >, set<int>> DavisPutnam::unitPropagate(
                                                                    vector<vector<int> > F, set<int> assignments
) {
    vector<vector<int> > newF = F;
    for (auto const& clause : F) {
        if (clause.size() == 1) {
            int const literal = clause[0];
            // Simplify the Formula by removing all clauses containing the `literal`
            // and by removing the literal from a clause where it is `-literal`.
            newF = simplify(newF, literal);
            assignments.insert(literal);
        }
    }
    return make_tuple(newF, assignments);
}

// Simplify the Formula by removing all clauses containing the `literal`
// and by removing the literal from a clause where it is `-literal`.
vector<vector<int> > DavisPutnam::simplify(vector<vector<int> > F, int subjectLiteral) {
    vector<vector<int> > newF = F;
    int numberOfRemovedClauses = 0;
    for (int i = 0; i < F.size(); i++) {
        vector<int> const& clause = F[i];
        int numberOfRemovedLiterals = 0;
        for (int j = 0; j < clause.size(); j++) {
            int const& literal = clause[j];
            // Remove the clause if it contains the `subjectLiteral`, since this means
            // the clause is TRUE.
            if (literal == subjectLiteral) {
                // Since the index `i` is based on the original Formula, erase the clause at
                // index `i` adjusted by the number of already removed clauses.
                newF.erase(newF.begin() + i - numberOfRemovedClauses);
                numberOfRemovedClauses++;
                continue;
                // Remove the `literal` from its clause if it contains the opposite of the
                // `subjectLiteral`, since this means the `literal` is FALSE.
            } else if (literal == -subjectLiteral) {
                // Since the index `j` is based on the original Clause, erase the literal
                // at index `j` adjusted by the number of already removed literals within
                // this clause.
                vector<int> *newClause = &newF.at(i - numberOfRemovedClauses);
                newClause->erase(newClause->begin() + j - numberOfRemovedLiterals);
                numberOfRemovedLiterals++;
                continue;
            }
        }
    }
    return newF;
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

// Based on the set heuristic, pick the next (TRUE) literal to branch into.
// Iterate through the current formula (clauses set), and find the first variable (TRUE literal)
// that is not yet already included in our current set of variables.
int DavisPutnam::getNextLiteral(vector<vector<int> > F, set<int> currentVariables) {
    int nextLiteral;
    for (auto const& clause : F) {
        for (int const& literal : clause) {
            // Find whether the variable (positive value of the literal) already is included
            // in the current set of variables.
            const bool literalIsAlreadyAssigned = find(
                                                       currentVariables.begin(), currentVariables.end(), abs(literal)
                                                       ) != currentVariables.end();
            
            if (!literalIsAlreadyAssigned) {
                nextLiteral = abs(literal);
                // Jump out of the nested for loops and return the next literal.
                goto end;
            }
        }
    }
end:
    return nextLiteral;
}

// Calculates the absolute (TRUE) values of each assignment, which represents
// each literal.
set<int> DavisPutnam::getVariables(set<int> assignments) {
    set<int> variables;
    for (auto const& el : assignments) {
        variables.insert(abs(el));
    }
    return variables;
}

// Checks whether a given set of clauses contains an empty clause.
bool DavisPutnam::containsEmptyClause(vector<vector<int> > F) {
    return find_if(F.begin(), F.end(), [](vector<int> const& clause) {
        return clause.empty();
    }) != F.end();
}

