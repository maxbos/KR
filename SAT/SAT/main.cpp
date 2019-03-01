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
#include <numeric>

using namespace std;

struct formula {
    vector<vector<int> > clauses;
    void push_back(vector<int> clause) { clauses.push_back(clause); };
    void pop_back() { clauses.pop_back(); };
    void insert(formula f2) {
        clauses.insert(clauses.end(), f2.clauses.begin(), f2.clauses.end());
    };
    operator bool() { return true; }
};

formula readDimacsFile                      (string);
void printClauses                           (vector<vector<int> >);
tuple<string, string, int> parseArguments   (int, char*[]);
float vectorMean                            (vector<int>);
int vectorSum                               (vector<int>);

class DavisPutnam {
    string strategy;
    string inputFilePath;
    bool saveFinalAssignments;
    int lefVariable;
    int backtrackCount = 0;
    formula setup                                           (formula);
    string saveOutput                                       ();
    set<int> recursive                                      (formula, set<int>);
    tuple<formula, set<int> > pureLiterals                  (formula, set<int>);
    tuple<formula, set<int> > unitPropagate                 (formula, set<int>);
    formula simplify                                        (formula, int);
    formula removeTautologies                               (formula);
    formula removeItemsByIndices                            (formula, vector<int>);
    int getNextLiteral                                      (formula, set<int>);
    int getNextRandomLiteral                                (formula, set<int>);
    set<int> getLiterals                                    (formula);
    set<int> getVariables                                   (set<int>);
    bool containsEmptyClause                                (formula);
    void printAssignments                                   (set<int>);
    formula attemptFormulaFix                               (formula);
    formula randomShuffle                                (formula);
    
public:
    struct metrics {
        metrics() : nUnsatisfiable(0), nRootVisits(0), nSudokuEdits(0),
        nBacktracks(0), nBacktracksLastTree(0), uniquelySatisfiable(false)  {};
        int nUnsatisfiable;
        int nRootVisits;
        int nSudokuEdits;
        int nBacktracks;
        int nBacktracksLastTree;
        int runtime;
        bool uniquelySatisfiable;
    } stats;
    set<int> finalAssignments;
    set<int> lastAssignments;
    set<int> prevAssignments;
    formula prevFormula;
    
    DavisPutnam                         (string, string, bool);
};

int main(int argc, char* argv[]) {
    string strategy, inputfile;
    int numberOfRuns;
    vector<int> runtimes, backtracks, backtracksLastTree, rootVisits, unsatisfiables, uniquelySatisfiables;
    tie(strategy, inputfile, numberOfRuns) = parseArguments(argc, argv);
    bool saveFinalAssignments = numberOfRuns == 1;
    for (int i = 0; i < numberOfRuns; i++) {
        string inputfilePath;
        if (numberOfRuns == 1) {
            inputfilePath = inputfile;
        } else {
            inputfilePath = inputfile + to_string(i) + ".txt";
        }
        DavisPutnam davisPutnam(strategy, inputfilePath, saveFinalAssignments);
        runtimes.push_back(davisPutnam.stats.runtime);
        backtracks.push_back(davisPutnam.stats.nBacktracks);
        backtracksLastTree.push_back(davisPutnam.stats.nBacktracksLastTree);
        rootVisits.push_back(davisPutnam.stats.nRootVisits);
        unsatisfiables.push_back(davisPutnam.stats.nUnsatisfiable);
        uniquelySatisfiables.push_back(davisPutnam.stats.uniquelySatisfiable);
    }
    float meanRuntime = vectorMean(runtimes);
    double meanBacktracks = vectorMean(backtracks);
    double meanBacktracksLastTree = vectorMean(backtracksLastTree);
    double meanRootVisits = vectorMean(rootVisits);
    double meanUnsatisfiables = vectorMean(unsatisfiables);
    int totalUniquelySatisfiable = vectorSum(uniquelySatisfiables);
    
    cout << "Number of problems (solved or unsolved): " << numberOfRuns << endl;
    cout << "(Average) runtime: " << meanRuntime << " second(s)."<< endl;
    cout << "(Average) number of backtracks: " << meanBacktracks << endl;
    cout << "(Average) number of backtracks in last tree: " << meanBacktracksLastTree << endl;
    cout << "(Average) number of root visits: " << meanRootVisits << endl;
    cout << "(Average) number of unsatisfiables: " << meanUnsatisfiables << endl;
    cout << "Number of problems that are uniquely satisfiable: " << totalUniquelySatisfiable << endl;
    return 0;
}

/**
 *
 */
DavisPutnam::DavisPutnam(string strategy, string inputFilePath, bool saveFinalAssignments)
: strategy(strategy), inputFilePath(inputFilePath), saveFinalAssignments(saveFinalAssignments) {
    // Initialize the recursive Davis Putnam algorithm with an empty set
    // of assignments.
    set<int> assignments;
    time_t tstart, tend;
    struct formula formula, newFormula;
    // Reset metrics
    tstart = time(0);
    formula = readDimacsFile(inputFilePath);
    if (strategy == "-S3") {
        struct formula xSudokuRules = readDimacsFile("resources/x-sudoku-rules.txt");
        formula.insert(xSudokuRules);
    }
    newFormula = setup(formula);
   do {
        finalAssignments = recursive(newFormula, assignments);    
   } while (finalAssignments.empty() && ++stats.nUnsatisfiable &&
            !!(newFormula = attemptFormulaFix(newFormula)));
    tend = time(0);
    stats.runtime = difftime(tend, tstart);
    
    vector<int> clause;
    for (auto assignment : finalAssignments) {
        if (assignment > 0) clause.push_back(-assignment);
    }
    newFormula.clauses.push_back(clause);
    set<int> newAssignments = recursive(newFormula, assignments);
    stats.uniquelySatisfiable = newAssignments.empty();
    
    saveOutput();
}

/**
 * Changes the given formula to containing rules that will hopefully create a satisfiable
 * formula.
 */
formula DavisPutnam::attemptFormulaFix(formula formula) {
    cout << "trying to fix the formula" << endl;
    // If the current formula performed better, we want to keep the current
    // formula. If the prev formula performed better, we want to reset it.
    cout << "prev: " << prevAssignments.size() << ", last: " << lastAssignments.size() << endl;
    if (prevAssignments.size() > lastAssignments.size()) {
        formula = prevFormula;
    }
    prevFormula = formula;
    prevAssignments = lastAssignments;
    formula = randomShuffle(formula);
    return formula;
}

/**
 * Erases assignments from the given formula for a given probability.
 */
formula DavisPutnam::randomShuffle(formula formula) {
//    int nRemovedClauses = 0;
    struct formula newFormula = formula;
    for (int i = 0; i < formula.clauses.size(); i++) {
        auto const& clause = formula.clauses.at(i);
        // We have found a position assignment. Delete this assignment with a probability
        // of 0.5.
        int const random = (rand() % (8 + 1 - 1)) + 1;
        if (clause.size() == 1 && random == 1) {
            string const literal = to_string(clause.at(0));
//            char const& ypos = literal.at(0);
//            char const& xpos = literal.at(1);
            char const& val = literal.at(2);
            // if diagonal change val/pos
//            if (ypos == xpos) {
            int const randomx = (rand() % (9 + 1 - 1)) + 1;
            int const randomy = (rand() % (9 + 1 - 1)) + 1;
//                int const randomv = (rand() % (9 + 1 - 1)) + 1;
            int newLiteral = stoi(to_string(randomy) + to_string(randomx) + val);
            cout << "was: " << literal << " , now is: " << newLiteral << endl;
            newFormula.clauses[i] = { newLiteral };
//            }
//            goto end;
        }
    }
//    end:
//    cout << "Removed " << nRemovedClauses << " assignments" << endl;
    return newFormula;
}

/**
 * Perform essential steps before starting the recursive Davis Putnam algorithm,
 * such as removing tautologies from the initial Formula.
 */
formula DavisPutnam::setup(formula formula) {
    formula = removeTautologies(formula);
    return formula;
}

/**
 * Save outputs to file.
 */
string DavisPutnam::saveOutput() {
    if (!saveFinalAssignments) return "";
    printAssignments(finalAssignments);
    string filename = inputFilePath + ".out";
    ofstream outputfile;
    outputfile.open(filename);
    outputfile << "p cnf " << finalAssignments.size() << " " << finalAssignments.size() << endl;
    for (int assignment : finalAssignments) {
        outputfile << assignment << " 0" << endl;
    }
    outputfile.close();
    cout << "Assignments written to file: " << filename << endl;
    return filename;
}

/**
 *
 */
set<int> DavisPutnam::recursive(formula formula, set<int> assignments) {
    if (assignments.empty()) { stats.nRootVisits++; stats.nBacktracksLastTree = 0; }
    struct formula newFormula = formula;
    tie(newFormula, assignments) = pureLiterals(newFormula, assignments);
    tie(newFormula, assignments) = unitPropagate(newFormula, assignments);
    // When the set of clauses contains an empty clause, the problem is unsatisfiable.
    if (containsEmptyClause(newFormula)) { lastAssignments = assignments; return {}; };
    // We have found a successfull assignment when we have no clauses left.
    if (newFormula.clauses.empty()) return assignments;
    // We perform the branching step by picking a literal that is not yet included
    // in our partial assignment.
    int literal = getNextLiteral(newFormula, getVariables(assignments));
    // Split into the TRUE value for the new variable.
    newFormula.push_back({ literal });
    assignments.insert(literal);
    set<int> leftSplitAssignments = recursive(newFormula, assignments);
    if (!leftSplitAssignments.empty()) return leftSplitAssignments;
    stats.nBacktracks++;
    stats.nBacktracksLastTree++;
    // If the TRUE value branching step did not yield a successfull assignment,
    // we try the FALSE value for the same variable.
    newFormula.pop_back();
    newFormula.push_back({ -literal });
    assignments.erase(literal);
    assignments.insert(-literal);
    return recursive(newFormula, assignments);
}

/**
 * If a literal has no counter part, set this literal in our assignments and simplify
 * the formula for this literal.
 */
tuple<formula, set<int> > DavisPutnam::pureLiterals(formula formula, set<int> assignments) {
    struct formula new_formula = formula;
    set<int> literals = getLiterals(formula);
    for (auto const& literal : literals) {
        if (find(literals.begin(), literals.end(), -literal) == literals.end()) {
            new_formula = simplify(new_formula, literal);
            assignments.insert(literal);
        }
    }
    return make_tuple(new_formula, assignments);
}

/**
 * For each unit clause in the Formula, set the literal from that clause to TRUE,
 * and simplify the Formula.
 */
tuple<formula, set<int> > DavisPutnam::unitPropagate(formula formula, set<int> assignments) {
    struct formula new_formula = formula;
    for (auto const& clause : formula.clauses) {
        if (clause.size() == 1) {
            int const literal = clause[0];
            // Simplify the Formula by removing all clauses containing the `literal`
            // and by removing the literal from a clause where it is `-literal`.
            new_formula = simplify(new_formula, literal);
            assignments.insert(literal);
        }
    }
    if (strategy == "-S2" && !new_formula.clauses.empty()) {
        for (auto it = new_formula.clauses.rbegin(); it != new_formula.clauses.rend(); ++it) {
            if ((*it).empty()) continue;
            if ((*it).back() == lefVariable) continue;
            lefVariable = (*it).back();
            break;
        }
    }
    return make_tuple(new_formula, assignments);
}

/**
 * Simplify the Formula by removing all clauses containing the `literal`
 * and by removing the literal from a clause where it is `-literal`.
 */
formula DavisPutnam::simplify(formula formula, int subjectLiteral) {
    struct formula new_formula = formula;
    int numberOfRemovedClauses = 0;
    for (int i = 0; i < formula.clauses.size(); i++) {
        vector<int> const& clause = formula.clauses[i];
        int numberOfRemovedLiterals = 0;
        for (int j = 0; j < clause.size(); j++) {
            int const& literal = clause[j];
            // Remove the clause if it contains the `subjectLiteral`, since this means
            // the clause is TRUE.
            if (literal == subjectLiteral) {
                // Since the index `i` is based on the original Formula, erase the clause at
                // index `i` adjusted by the number of already removed clauses.
                new_formula.clauses.erase(new_formula.clauses.begin() + i - numberOfRemovedClauses);
                numberOfRemovedClauses++;
                continue;
                // Remove the `literal` from its clause if it contains the opposite of the
                // `subjectLiteral`, since this means the `literal` is FALSE.
            } else if (literal == -subjectLiteral) {
                // Since the index `j` is based on the original Clause, erase the literal
                // at index `j` adjusted by the number of already removed literals within
                // this clause.
                vector<int> *newClause = &new_formula.clauses.at(i - numberOfRemovedClauses);
                newClause->erase(newClause->begin() + j - numberOfRemovedLiterals);
                numberOfRemovedLiterals++;
                continue;
            }
        }
    }
    return new_formula;
}

/**
 *
 */
formula DavisPutnam::removeTautologies(formula formula) {
    vector<int> removeIndices;
    for (int i = 0; i < formula.clauses.size(); i++) {
        vector<int> const& clause = formula.clauses[i];
        for (int const& literal : clause) {
            if (find(clause.begin(), clause.end(), -literal) != clause.end()) {
                removeIndices.push_back(i);
            }
        }
    }
    return removeItemsByIndices(formula, removeIndices);
}

formula DavisPutnam::removeItemsByIndices(formula formula, vector<int> removeIndices) {
    struct formula new_formula = formula;
    // Sort the indices such that we can remove the highest indices first
    sort(removeIndices.begin(), removeIndices.end(), greater<int>());
    // Remove the clauses from the formula list
    for (auto const& index : removeIndices) {
        new_formula.clauses.erase(new_formula.clauses.begin() + index);
    }
    return new_formula;
}

/**
 * Based on the set heuristic, pick the next (TRUE) literal to branch into.
 * Iterate through the current formula (clauses set), and find the first variable (TRUE literal)
 * that is not yet already included in our current set of variables.
 */
int DavisPutnam::getNextLiteral(formula formula, set<int> currentVariables) {
    if (strategy == "-S1") {
        int nextLiteral = NULL;
        for (auto const& clause : formula.clauses) {
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
    } else if (strategy == "-S2" || strategy == "-S3") {
        if (currentVariables.find(abs(lefVariable)) == currentVariables.end()) {
            return lefVariable;
        } else {
            return getNextRandomLiteral(formula, currentVariables);
        }
    } else {
        throw "This strategy has no implementation of getNextLiteral()";
    }
    // No valid strategy specified
    return 0;
}

int DavisPutnam::getNextRandomLiteral(formula formula, set<int> currentVariables) {
    set<int> literals = getLiterals(formula);
    int next_literal;
    set<int>::const_iterator it(literals.begin());
    do {
        int rand_idx = rand() % literals.size();
        advance(it, rand_idx);
        next_literal = *it;
    } while(find(currentVariables.begin(), currentVariables.end(), abs(next_literal)) != currentVariables.end());
    return next_literal;
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

set<int> DavisPutnam::getLiterals(formula formula) {
    set<int> ret;
    for(const auto &v: formula.clauses)
        ret.insert(v.begin(), v.end());
    return ret;
}

// Checks whether a given set of clauses contains an empty clause.
bool DavisPutnam::containsEmptyClause(formula F) {
    return find_if(F.clauses.begin(), F.clauses.end(), [](vector<int> const& clause) {
        return clause.empty();
    }) != F.clauses.end();
}

void DavisPutnam::printAssignments(set<int> assignments) {
    cout << "Number of assignments is: " << assignments.size() << endl;
    cout << "The final assignment is: ";
    int count = 0;
    for (auto const& i: assignments) {
        if (i > 0) {
            count++;
            cout << i << " ";
        }
    }
    cout << endl;
    cout << endl << "Number of positive assignments: " << count << endl;
}

/**
 * Parse input arguments as strategy and inputfile.
 */
tuple<string, string, int> parseArguments(int argc, char* argv[]) {
    if (argc < 3) {
        cout << "Usage: SAT <strategy> <inputfile>" << endl;
        exit(1);
    }
    string strategy = argv[1];
    string inputfile = argv[2];
    int numberOfRuns;
    if (argc == 4) {
        numberOfRuns = atoi(argv[3]);
    } else {
        numberOfRuns = 1;
    }
    return make_tuple(strategy, inputfile, numberOfRuns);
}

/**
 * Reads and parses a DIMACS file from a given file location.
 * The DIMACS clauses are stored as a (formula) vector of clause
 * vectors.
 */
formula readDimacsFile(string loc) {
    ifstream dimacsFile;
    dimacsFile.open(loc);
    
    formula formula;
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
                formula.clauses.push_back(clause);
                lastClause = &formula.clauses.back();
            } else {
                lastClause->push_back(literal);
            }
        }
        prevLiteral = literal;
    }
    dimacsFile.close();
    return formula;
}

/**
 * Prints a formula of clauses, where each clause is printed
 * on its individual line.
 */
void printClauses(vector<vector<int> > clauses) {
    for (vector<int> const& clause: clauses) {
        for (int const& literal: clause) {
            cout << literal << " + ";
        }
        cout << endl;
    }
}

float vectorMean(vector<int> v) {
    return accumulate(v.begin(), v.end(), 0.0)/v.size();
}

int vectorSum(vector<int> v) {
    return accumulate(v.begin(), v.end(), 0);
}

