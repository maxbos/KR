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
#include <map>
#include <memory>

using namespace std;

struct Formula {
    vector<vector<int> > clauses;
    map<int, vector<shared_ptr<vector<int> > > > literals;
    bool containsEmptyClause;
    // Loop through the clause and remove the clause pointer for each literal within it.
    // Then, erase the clause from `clauses`.
    void eraseClause(shared_ptr<vector<int> > const& clause) {
        for (auto const& literal : *clause) {
            auto const& clausePtrs = literals[literal];
            auto it = find(clausePtrs.begin(), clausePtrs.end(), clause);
            if (it != clausePtrs.end()) {
                literals[literal].erase(it);
            }
        }
        auto it = find(clauses.begin(), clauses.end(), *clause);
        if (it != clauses.end()) {
            clauses.erase(it);
        }
    };
    void addClauseForLiteral(int literal) {
        clauses.push_back({ literal });
        literals[literal].push_back(make_shared<vector<int> >(clauses.back()));
    };
    // Erase each clause in which the given `literal` occurs. Finally, erase the `literal`
    // key from the Formula.literals map.
    void eraseClausesForLiteral(int literal) {
        auto const clausesContainingLiteral = literals[literal];
//        cout << clausesContainingLiteral.size() << endl;
        for (auto const& clausePtr : clausesContainingLiteral) {
//            cout << "clause ptr: " << &clausePtr << endl;
            eraseClause(clausePtr);
        }
//        cout << "done erasing it" << endl;
        literals.erase(literal);
    };
    // Erase the literal instances from the clauses in which the given `literal`
    // occurs. If a clause remains empty after erasing, set the Formula.containsEmptyClause
    // flag to TRUE. Finally, erase the `literal` key from the Formula.literals map.
    void eraseLiteralInClauses(int literal) {
        auto const clausesContainingLiteral = literals[literal];
        for (auto const& clausePtr : clausesContainingLiteral) {
            for (int i = 0; i < clausePtr->size(); i++) {
                int l = clausePtr->at(i);
                if (l == literal) {
                    clausePtr->erase(clausePtr->begin() + i);
                    // Important: check for an empty clause after erasing.
                    if (clausePtr->empty()) {
                        containsEmptyClause = true;
                    }
                    continue;
                }
            }
        }
        literals.erase(literal);
    };
    void printLiterals() {
        for (auto const& literalClausePtrs : literals) {
            cout << "literal: " << literalClausePtrs.first << endl;
            for (auto const& clause : literalClausePtrs.second) {
                cout << "clause: ";
                for (auto const& l : *clause) {
                    cout << l << " + ";
                }
                cout << endl;
            }
            cout << endl;
        }
    };
};

Formula readDimacsFile                  (string);
void printClauses                       (vector<vector<int> >);

class DavisPutnam {
    string strategy;
    string inputFilePath;
    Formula formula;
    Formula setup                                               (Formula);
    Formula removeTautologies                                   (Formula);
    vector<int> recursive                                       (Formula, vector<int>);
    tuple<Formula, vector<int> > pureLiterals                   (Formula, vector<int>);
    tuple<Formula, vector<int> > unitPropagate                  (Formula, vector<int>);
    Formula simplify                                            (Formula, int);
    int getNextLiteral                                          (Formula, set<int>);
    set<int> getVariables                                       (vector<int>);
    
public:
    DavisPutnam                         (string strategy, Formula formula);
};

int main() {
    // TODO load the inputfile dynamically
    Formula formula = readDimacsFile("resources/1000-sudokus/1000.txt");
    cout << formula.literals.size() << endl;
//     printClauses(formula.clauses);
//    formula.printLiterals();
    DavisPutnam davisPutnam("S1", formula);
    return 0;
}

// Reads and parses a DIMACS file from a given file location.
// The DIMACS clauses are stored as a (formula) vector of clause
// vectors.
Formula readDimacsFile(string loc) {
    ifstream dimacsFile;
    dimacsFile.open(loc);
    
    Formula formula;
    vector<vector<int> > clauses;
    
    string skip;
    // Skip the first 4 strings in the first line
    dimacsFile >> skip >> skip >> skip >> skip;
    
    int literal;
    int prevLiteral = 0;
    shared_ptr<vector<int> > lastClause(nullptr);
    
    while (dimacsFile >> literal) {
        if (literal != 0) {
            if (prevLiteral == 0) {
                vector<int> clause = { literal };
                formula.clauses.push_back(clause);
                lastClause = make_shared<vector<int> >(formula.clauses.back());
                formula.literals[literal].push_back(lastClause);
            } else {
                lastClause->push_back(literal);
                formula.literals[literal].push_back(lastClause);
            }
        }
        prevLiteral = literal;
    }
    dimacsFile.close();
    return formula;
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

DavisPutnam::DavisPutnam(string strategy, Formula formula)
: strategy(strategy), formula(formula) {
    // Initialize the recursive Davis Putnam algorithm with an empty set
    // of assignments.
    vector<int> assignments;
    formula = setup(formula);
    vector<int> finalAssignments = recursive(formula, assignments);
    cout << finalAssignments.size() << endl;
    cout << "The final assignment is: ";
    int count = 0;
    for (auto const& i: finalAssignments) {
        if (i > 0) {
            count++;
            cout << i << " ";
        }
    }
    cout << endl << count << endl;
    cout << endl;
}

// Perform essential steps before starting the recursive Davis Putnam algorithm,
// such as removing tautologies from the initial Formula.
Formula DavisPutnam::setup(Formula formula) {
    formula = removeTautologies(formula);
    return formula;
}

vector<int> DavisPutnam::recursive(Formula formula, vector<int> assignments) {
    cout << " recursive call " << endl;
    Formula F = formula;
    tie(F, assignments) = pureLiterals(F, assignments);
    tie(F, assignments) = unitPropagate(F, assignments);
    cout << "number of clauses left: " << F.clauses.size() << endl;
    // When the set of clauses contains an empty clause, the problem is unsatisfiable.
    if (F.containsEmptyClause) return {};
    // We have found a successfull assignment when we have no clauses left.
    if (F.clauses.empty()) return assignments;
    // We perform the branching step by picking a literal that is not yet included
    // in our partial assignment.
    int literal = getNextLiteral(F, getVariables(assignments));
    // Split into the TRUE value for the new variable.
    Formula leftSplitFormula = F;
    leftSplitFormula.addClauseForLiteral(literal);
    assignments.push_back(literal);
    if (!recursive(leftSplitFormula, assignments).empty()) return assignments;
    // If the TRUE value branching step did not yield a successful assignment,
    // we try the FALSE value for the same variable.
    Formula rightSplitFormula = F;
    rightSplitFormula.addClauseForLiteral(-literal);
    assignments[assignments.size()-1] = -literal;
    return recursive(rightSplitFormula, assignments);
}

// Iterate through the clauses and maintain a state for which literals no counterpart is found.
tuple<Formula, vector<int> > DavisPutnam::pureLiterals(Formula formula, vector<int> assignments) {
    Formula newFormula = formula;
    for (auto const& literalClauses : formula.literals) {
        int literal = literalClauses.first;
        // If the formula does not contain the opposite of the `literal`, we can
        // say that `literal` is a pure literal. If so we set this `literal` to TRUE
        // and simplify the formula.
        if (formula.literals.count(-literal) == 0) {
            assignments.push_back(literal);
            newFormula = simplify(newFormula, literal);
        }
    }
    return make_tuple(newFormula, assignments);
}

// For each unit clause in the Formula, set the literal from that clause to TRUE,
// and simplify the Formula.
tuple<Formula, vector<int> > DavisPutnam::unitPropagate(Formula formula, vector<int> assignments) {
    Formula newFormula = formula;
    for (auto const& literalClauses : formula.literals) {
        auto const& literal = literalClauses.first;
        auto const& clauses = literalClauses.second;
        for (auto const& clausePtr : clauses) {
            // If there is only one literal in the current clause, the clause is
            // a unit clause. So, set the `literal` to TRUE and simplify the current
            // state of the formula. Then, continue to the next literal in our literals-
            // clause-pointers list.
            if (clausePtr->size() == 1) {
                assignments.push_back(literal);
                newFormula = simplify(newFormula, literal);
                continue;
            }
        }
    }
    return make_tuple(newFormula, assignments);
}

// Simplify the Formula by removing all clauses containing the `literal`
// and by removing the literal from a clause where it is `-literal`.
Formula DavisPutnam::simplify(Formula formula, int subjectLiteral) {
    Formula newFormula = formula;
    // Remove all clauses that contain the `subjectLiteral`.
    newFormula.eraseClausesForLiteral(subjectLiteral);
    // Remove all -subjectLiteral instances from all clauses in which it occurs.
    newFormula.eraseLiteralInClauses(-subjectLiteral);
    return newFormula;
}

Formula DavisPutnam::removeTautologies(Formula formula) {
    map<int, vector<shared_ptr<vector<int> > > > literals = formula.literals;
    for (auto const& literalClauses : literals) {
        auto const& literal = literalClauses.first;
        if (literal > 0) {
            // Find the FALSE instance of this literal.
            if (formula.literals.count(-literal) > 0) {
                auto const& clauses = literalClauses.second;
                for (auto const& clausePtr : clauses) {
                    // Find if the clause pointer also is contained within the
                    // clauses-pointer-list of the `-literal`.
                    auto oppLiteralClauses = formula.literals[-literal];
                    if (find(oppLiteralClauses.begin(), oppLiteralClauses.end(), clausePtr) != oppLiteralClauses.end()) {
                        formula.eraseClause(clausePtr);
                    }
                }
            }
        }
    }
    return formula;
}

// Based on the set heuristic, pick the next (TRUE) literal to branch into.
// Iterate through the current formula (clauses set), and find the first variable (TRUE literal)
// that is not yet already included in our current set of variables.
int DavisPutnam::getNextLiteral(Formula formula, set<int> currentVariables) {
    int nextLiteral;
    for (auto const& literalClauses : formula.literals) {
        auto const& literal = literalClauses.first;
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
end:
    return nextLiteral;
}

// Calculates the absolute (TRUE) values of each assignment, which represents
// each literal.
set<int> DavisPutnam::getVariables(vector<int> assignments) {
    set<int> variables;
    for (auto const& el : assignments) {
        variables.insert(abs(el));
    }
    return variables;
}
