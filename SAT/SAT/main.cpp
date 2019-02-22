/*
 * main.cpp
 * Knowledge Representation: Project 1 (SAT)
 *
 * Studentnames:     Max Bos & Erik Stammes
 * Studentnumbers:   10669027 & 10559736
 */

#include <fstream>
#include <iostream>
#include <vector>
#include <set>
#include <map>

using namespace std;

struct Formula {
    map<int, vector<int> > clauses;
    map<int, vector<int> > index;
};

Formula readDimacsFile      (string);
void printClauses           (map<int, vector<int> >);

class DavisPutnam {
    string strategy;
    string inputFilePath;
    Formula formula;
    Formula setup                                   (Formula);
    vector<int> recursive                           (Formula, vector<int>);
    tuple<Formula, vector<int>> pureLiterals        (Formula, vector<int>);
    tuple<Formula, vector<int>> unitPropagate       (Formula, vector<int>);
    Formula simplify                                (Formula, int);
    Formula removeTautologies                       (Formula);
    vector<vector<int> > removeItemsByIndices       (vector<vector<int> >, vector<int>);
    int getNextLiteral                              (map<int, vector<int> >, set<int>);
    set<int> getVariables                           (vector<int>);
    bool containsEmptyClause                        (map<int, vector<int> >);
    int getHighestKey                               (map<int, vector<int> >);
    
public:
    DavisPutnam(string strategy, Formula f);
};

int main() {
    // TODO load the inputfile dynamically
    Formula f = readDimacsFile("resources/1000-sudokus/1000.txt");
//    printClauses(f.clauses);
    DavisPutnam davisPutnam("S1", f);
    return 0;
}

// Reads and parses a DIMACS file from a given file location.
// The DIMACS clauses are stored as a Formula consisting
// of clauses and an index map.
Formula readDimacsFile(string loc) {
    ifstream dimacsFile;
    dimacsFile.open(loc);
    
    map<int, vector<int> > clauses;
    map<int, vector<int> > index;
    
    string skip;
    // Skip the first 4 strings in the first line
    dimacsFile >> skip >> skip >> skip >> skip;
    
    int literal;
    vector<int> currentClause;
    int clauseNumber = 0;
    
    while (dimacsFile >> literal) {
        if (literal != 0) {
            currentClause.push_back(literal);
            if (index.count(literal) == 0) {
                index.insert(pair<int, vector<int> >(literal, vector<int> { clauseNumber }));
            } else {
                index[literal].push_back(clauseNumber);
            }
        } else {
            clauses.insert(pair<int, vector<int> >(clauseNumber, currentClause));
            clauseNumber++;
            currentClause = vector<int>();
        }
    }
    dimacsFile.close();
    return Formula { clauses, index };
}

// Prints a formula of clauses, where each clause is printed
// on its individual line.
void printClauses(map<int, vector<int> > clauses) {
    for (auto const& [clauseNumbers, clause] : clauses ) {
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
    cout << "The final assignment is: ";
    for (auto const& i: finalAssignments) {
        cout << i << " ";
    }
    cout << endl;
}

// Perform essential steps before starting the recursive Davis Putnam algorithm,
// such as removing tautologies from the initial Formula.
Formula DavisPutnam::setup(Formula formula) {
    formula = removeTautologies(formula); //TODO: fix this
    // TODO: set all pure literals to true in our assignment.
    return formula;
}

vector<int> DavisPutnam::recursive(Formula formula, vector<int> assignments) {
    cout << " recursive call " << endl;
    tie(formula, assignments) = pureLiterals(formula, assignments);
    tie(formula, assignments) = unitPropagate(formula, assignments);
    cout << "number of clauses left: " << formula.clauses.size() << endl;
    // When the set of clauses contains an empty clause, the problem is unsatisfiable.
    if (containsEmptyClause(formula.clauses)) return {};
    // We have found a successfull assignment when we have no clauses left.
    if (formula.clauses.empty()) return assignments;
    // We perform the branching step by picking a literal that is not yet included
    // in our partial assignment.
    int literal = getNextLiteral(formula.clauses, getVariables(assignments));
    // Split into the TRUE value for the new variable.
    int newClauseNumber = getHighestKey(formula.clauses) + 1;
    formula.clauses.insert(pair<int, vector<int> >(newClauseNumber, vector<int> {literal}));
    formula.index[literal].push_back(newClauseNumber);
    assignments.push_back(literal);
    if (!recursive(formula, assignments).empty()) return assignments;
    // If the TRUE value branching step did not yield a successfull assignment,
    // we try the FALSE value for the same variable.
    formula.clauses[newClauseNumber] = vector<int> {-literal};
    assignments[assignments.size()-1] = -literal;
    return recursive(formula, assignments);
}

tuple<Formula, vector<int>> DavisPutnam::pureLiterals(Formula formula, vector<int> assignments) {
    
}

// For each unit clause in the Formula, set the literal from that clause to TRUE,
// and simplify the Formula.
tuple<Formula, vector<int>> DavisPutnam::unitPropagate(Formula formula, vector<int> assignments) {
    Formula newFormula = Formula { formula.clauses, formula.index };
    for( auto const& [clauseNumber, clause] : formula.clauses ) {
        if (clause.size() == 1) {
            int const literal = clause[0];
            newFormula = simplify(newFormula, literal);
        }
    }
    return make_tuple(newFormula, assignments);
}

// Simplify the Formula by removing all clauses containing the `literal`
// and by removing the literal from a clause where it is `-literal`.
Formula DavisPutnam::simplify(Formula formula, int subjectLiteral) {
    map<int, vector<int> > clauses = formula.clauses;
    map<int, vector<int> > index = formula.index;
    vector<int> clausesToDelete = index[subjectLiteral];
    for (auto clauseNumber : clausesToDelete) {
        for (auto literal : clauses[clauseNumber]) {
            vector<int> clauseIndices = index[literal];
            clauseIndices.erase(remove(clauseIndices.begin(), clauseIndices.end(), clauseNumber), clauseIndices.end());
        }
        clauses.erase(clauseNumber);
    }
    index.erase(subjectLiteral);
    // Check if the negative of the literal exists in the index
    if (index.count(-subjectLiteral)) {
        vector<int> clausesToDeleteLiteralFrom = index[-subjectLiteral];
        for (auto clauseNumber : clausesToDeleteLiteralFrom) {
            vector<int> clause = clauses[clauseNumber];
            clause.erase(remove(clause.begin(), clause.end(), -subjectLiteral), clause.end()); //TODO: write a simple helper function to delete elements from vector
        }
        index.erase(-subjectLiteral);
    }
    return Formula { clauses, index };
}

Formula DavisPutnam::removeTautologies(Formula formula) {
    map<int, vector<int> > F = formula.clauses;
    map<int, vector<int> > index = formula.index;
    for (auto it = F.cbegin(); it != F.cend();) {
        vector<int> clause = it->second;
        for (int const& literal : clause) {
            if (find(clause.begin(), clause.end(), -literal) != clause.end()) {
                for (int const& literal : clause) {
                    vector <int> occurenceVec = index[literal];
                    occurenceVec.erase(remove(occurenceVec.begin(), occurenceVec.end(), literal)); //TODO: write a simple helper function to delete elements from vector
                }
                it = F.erase(it);
                continue;
            } else {
                ++it;
            }
        }
    }
    return Formula { F, index};
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
int DavisPutnam::getNextLiteral(map<int, vector<int> > clauses, set<int> currentVariables) {
    int nextLiteral;
    for (auto const& [clauseNumbers, clause] : clauses ) {
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
set<int> DavisPutnam::getVariables(vector<int> assignments) {
    set<int> variables;
    for (auto const& el : assignments) {
        variables.insert(abs(el));
    }
    return variables;
}

// Checks whether a given set of clauses contains an empty clause.
bool DavisPutnam::containsEmptyClause(map<int, vector<int> > clauses) {
    for (auto const& [clauseNumbers, clause] : clauses ) {
        if (clause.size() == 0) return true;
    }
    return false;
}

int DavisPutnam::getHighestKey(map<int, vector<int>> map) {
    vector<int> keys;
    for(auto const& imap: map) {
        keys.push_back(imap.first);
    }
    return *max_element(keys.cbegin(), keys.cend());
}

