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
#include <map>
#include <cmath>
#include <tuple>

using namespace std;

struct formula {
    vector<vector<int> > clauses;
    void push_back(vector<int> clause) { clauses.push_back(clause); };
    void pop_back() { clauses.pop_back(); };
    void insert(formula f2) {
        clauses.insert(clauses.end(), f2.clauses.begin(), f2.clauses.end());
    };
    operator bool() { return true; }
    vector<vector<int> >::iterator findAssignment(int literal) {
        return find_if(clauses.begin(), clauses.end(), [&literal](auto const& clause) {
            return clause.size() == 1 && clause.at(0) == literal;
        });
    };
    void swapUnitClause(int literal, int newLiteral, int affectedLiteral, int newAffectedLiteral) {
        if (literal != 0) {
            // Find the index of the unit clause for the given `literal`.
            vector<vector<int> >::iterator it_literal = findAssignment(literal);
            // Change the literals.
            clauses[it_literal - clauses.begin()] = {newLiteral};
        }
        if (affectedLiteral != 0) {
            // Find the index of the unit clause for the given `affectedLiteral`.
            vector<vector<int> >::iterator it_affLiteral = findAssignment(affectedLiteral);
            clauses[it_affLiteral - clauses.begin()] = {newAffectedLiteral};
        }
    };
};

formula readDimacsFile                      (string);
void printClauses                           (vector<vector<int> >);
tuple<string, string, int> parseArguments   (int, char*[]);
float vectorMean                            (vector<int>);
int vectorSum                               (vector<int>);
int char2int                                (char);

class DavisPutnam {
    string strategy;
    string inputFilePath;
    bool saveFinalAssignments;
    int lefVariable;
    int backtrackCount = 0;
    formula setup                                           (formula);
    string saveOutput                                       ();
    set<int> solve                                          (formula);
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
    formula randomShuffle                                   (formula);
    tuple<formula, bool> smartShuffle                       (formula);
    tuple<int, int, int> swapAssignmentInBlock              (string, int, vector<int>);
    set<int> getInitialAssignments                          (formula);
    map<int, vector<int> > getAssignmentsInBlocks           (formula);
    int getBlockIndex                                       (string);
    int getDiagonalConflict                                 (int, set<int>);
    char getRandomPositionInBlock                           (char, int);
    
public:
    struct metrics {
        metrics() : nUnsatisfiable(0), nRootVisits(0), nSudokuEdits(0),
        nBacktracks(0), nBacktracksLastTree(0), nRandomNumbers(0),
        smartShuffles(0), randomShuffles(0), solved(false)  {};
        int nUnsatisfiable;
        int nRootVisits;
        int nSudokuEdits;
        int nBacktracks;
        int nBacktracksLastTree;
        int nRandomNumbers;
        int runtime;
        int smartShuffles;
        int randomShuffles;
        bool solved;
        int ymax;
        int blockSize;
    } stats;
    set<int> finalAssignments;
    set<int> lastAssignments;
    set<int> bestAssignments;
    formula bestFormula;
    
    DavisPutnam                         (string, string, bool);
};

int main(int argc, char* argv[]) {
    string strategy, inputfile;
    int numberOfRuns;
    vector<int> runtimes, backtracks, backtracksLastTree, rootVisits, unsatisfiables,
        randomNumberGenerations, smartShuffles, randomShuffles, solvedProblems;
    tie(strategy, inputfile, numberOfRuns) = parseArguments(argc, argv);
    bool saveFinalAssignments = numberOfRuns == 1;
    for (int i = 0; i < numberOfRuns; i++) {
        string inputfilePath;
        if (numberOfRuns == 1) {
            inputfilePath = inputfile;
        } else {
            inputfilePath = inputfile + to_string(i) + ".txt";
        }
        cout << "Current problem: " << inputfilePath << endl;
        DavisPutnam davisPutnam(strategy, inputfilePath, saveFinalAssignments);
        runtimes.push_back(davisPutnam.stats.runtime);
        backtracks.push_back(davisPutnam.stats.nBacktracks);
        backtracksLastTree.push_back(davisPutnam.stats.nBacktracksLastTree);
        rootVisits.push_back(davisPutnam.stats.nRootVisits);
        unsatisfiables.push_back(davisPutnam.stats.nUnsatisfiable);
        randomNumberGenerations.push_back(davisPutnam.stats.nRandomNumbers);
        smartShuffles.push_back(davisPutnam.stats.smartShuffles);
        randomShuffles.push_back(davisPutnam.stats.randomShuffles);
        solvedProblems.push_back(davisPutnam.stats.solved);
    }
    
    cout << "Total number of problems: " << numberOfRuns << endl;
    cout << "Total number of solved problems: " << vectorSum(solvedProblems) << endl;
    cout << "(Average) runtime: " << vectorMean(runtimes) << " second(s)."<< endl;
    cout << "(Average) number of backtracks: " << vectorMean(backtracks) << endl;
    cout << "(Average) number of backtracks in last tree: " << vectorMean(backtracksLastTree) << endl;
    cout << "(Average) number of root visits: " << vectorMean(rootVisits) << endl;
    cout << "(Average) number of unsatisfiables: " << vectorMean(unsatisfiables) << endl;
    cout << "(Average) number of random number generations: " << vectorMean(randomNumberGenerations) << endl;
    cout << "(Average) number of smart shuffles: " << vectorMean(smartShuffles) << endl;
    cout << "(Average) number of random shuffles: " << vectorMean(randomShuffles) << endl;
    return 0;
}

/**
 *
 */
DavisPutnam::DavisPutnam(string strategy, string inputFilePath, bool saveFinalAssignments)
: strategy(strategy), inputFilePath(inputFilePath), saveFinalAssignments(saveFinalAssignments) {
    set<int> assignments;
    time_t tstart, tend;
    struct formula formula, newFormula;
    vector<int> clause;
    
    tstart = time(0);
    formula = readDimacsFile(inputFilePath);
    if (strategy == "-S3" || strategy == "-S4") {
        struct formula xSudokuRules = readDimacsFile("resources/x-sudoku-rules.txt");
        if (xSudokuRules.clauses.size() == 0) throw "Could not find/parse resources/x-sudoku-rules.txt file";
        formula.insert(xSudokuRules);
    }
    newFormula = setup(formula);
    finalAssignments = solve(newFormula);
    
    tend = time(0);
    stats.runtime = difftime(tend, tstart);
    stats.solved = finalAssignments.size() != 0;
    
    saveOutput();
}

/**
 *
 */
set<int> DavisPutnam::solve(formula formula) {
    if (stats.nUnsatisfiable >= 100) {
        // TODO decide good number
        return {};
    }
    set<int> assignments;
    set<int> _finalAssignments = recursive(formula, assignments);
    if (!_finalAssignments.empty() || strategy == "-S1" || strategy == "-S2") return _finalAssignments;
    stats.nUnsatisfiable++;
    formula = attemptFormulaFix(formula);
    return solve(formula);
}

/**
 * Changes the given formula to containing rules that will hopefully create a satisfiable
 * formula.
 */
formula DavisPutnam::attemptFormulaFix(formula formula) {
    // If the current formula performed better, we want to keep the current
    // formula. If the prev formula performed better, we want to reset it.
   if (lastAssignments.size() > bestAssignments.size()) {
       bestAssignments = lastAssignments;
       bestFormula = formula;
   } else {
       formula = bestFormula;
   }
    bool changeApplied = false;
    // Only apply smart shuffle if strategy 4 is enabled.
    if (strategy == "-S4") {
        tie(formula, changeApplied) = smartShuffle(formula);
    }
    if (!changeApplied) formula = randomShuffle(formula);
    return formula;
}

tuple<formula, bool> DavisPutnam::smartShuffle(formula formula) {
    struct formula newFormula = formula;
    set<int> literals = getInitialAssignments(formula);
    map<int, vector<int> > blocks = getAssignmentsInBlocks(formula);
    for (auto const& block: blocks) {
        int const& blockIdx = block.first;
        auto const& blockAssignments = block.second;
        for (auto const& literal : blockAssignments) {
            int conflict = getDiagonalConflict(literal, literals);
            if (conflict) {
                int newLiteral, affectedLiteral, newAffectedLiteral;
                tie(newLiteral, affectedLiteral, newAffectedLiteral) = swapAssignmentInBlock(to_string(literal), blockIdx, blockAssignments);
                newFormula.swapUnitClause(literal, newLiteral, affectedLiteral, newAffectedLiteral);
                // Only fix one conflict at the time
                ++stats.smartShuffles;
                return make_tuple(newFormula, true);
            }
        }
    }
    return make_tuple(formula, false);
}

/**
 * Checks if there are any conflicts on the diagonals
 * Returns the first conflicting literal, if any
 */
int DavisPutnam::getDiagonalConflict(int lit, set<int> literals) {
    string literal = to_string(lit);
    char y = literal.at(0);
    char x = literal.at(1);
    char v = literal.at(2);
    if (char2int(x) + char2int(y) == stats.ymax + 1) {
        // Literal is on the bottom-left <-> top-right diagonal
        for (int n = 1; n <= stats.ymax; n++) {
            if (n == char2int(y)) continue;
            int check = stoi(to_string(n) + to_string(stats.ymax+1-n) + string(1, v));
            if (find(literals.begin(), literals.end(), check) != literals.end()) {
                // Conflict found
                return check;
            }
         }
    }
    if (char2int(x) == char2int(y)) {
        // Literal is on the top-left <-> bottom-right diagonal
        for (int n = 1; n <= stats.ymax; n++) {
            if (n == char2int(x)) continue;
            int check = stoi(to_string(n) + to_string(n) + string(1, v));
            if (find(literals.begin(), literals.end(), check) != literals.end()) {
                // Conflict found
                return check;
            }
        }
    }
    // Literal is not on the diagonal or no conflict found
    return 0;
}

/**
 * Swaps assignments within sudoku blocks.
 *
 * Heuristics:
 * -> for choosing within how many blocks literals to swap
 * -> for choosing which literal(s) to swap
 * -> for how many blocks, how many literals to swap?
 */
formula DavisPutnam::randomShuffle(formula formula) {
    struct formula newFormula = formula;
    map<int, vector<int> > blocks = getAssignmentsInBlocks(formula);
    for (auto const& block : blocks) {
        int const randnum = (rand() % (9 + 1 - 1)) + 1;
        if (randnum < 4) continue;
        int const& blockIdx = block.first;
        auto const& blockAssignments = block.second;
        // Within this block we swap neighboring literals.
        // First, randomly pick one literal from our set of assignments.
        int const randidx = (rand() % blockAssignments.size());
        int const literal = blockAssignments.at(randidx);
        // Second, change the position of this literal 1 step left,
        // right, bottom, up, within its own block.
        // Last, if the new position was already occupied, change the old
        // occupant to the previous position from our changed literal.
        int newLiteral, affectedLiteral, newAffectedLiteral;
        tie(newLiteral, affectedLiteral, newAffectedLiteral) = swapAssignmentInBlock(to_string(literal), blockIdx, blockAssignments);
        newFormula.swapUnitClause(literal, newLiteral, affectedLiteral, newAffectedLiteral);
        ++stats.randomShuffles;
    }
    return newFormula;
}

/**
 *
 */
tuple<int, int, int> DavisPutnam::swapAssignmentInBlock(string literal, int blockIdx, vector<int> blockAssignments) {
    string newLiteral;
    char newY, newX;
    vector<int> const& poss = {-1, 0, 1};
    char y = literal.at(0);
    char x = literal.at(1);
    char v = literal.at(2);
//    cout << "current literal: " << literal;
    do {
        newLiteral = "";
        // Calculate new position for literal.
        newY = y + poss.at(rand() % 3);
        newX = x + poss.at(rand() % 3);
//        newY = getRandomPositionInBlock(y, -1);
//        newX = getRandomPositionInBlock(x, -1);
        newLiteral += newY;
        newLiteral += newX;
        newLiteral += v;
    // Try again if the new literal is not in the same block as previous literal.
    } while (getBlockIndex(newLiteral) != blockIdx || newLiteral == literal);
    // Find a possibly affected literal at position (newY, newX).
//    cout << ", new literal: " << newLiteral << endl;
    vector<int>::iterator it = find_if(blockAssignments.begin(), blockAssignments.end(), [newY, newX](int const& ass) {
        return to_string(ass).at(0) == newY && to_string(ass).at(1) == newX;
    });
    string newAffectedLiteral;
    int newAffectedLiteralInt = 0;
    if (it != blockAssignments.end()) {
        newAffectedLiteral = "";
        newAffectedLiteral += y;
        newAffectedLiteral += x;
        newAffectedLiteral += to_string(*it).at(2);
        newAffectedLiteralInt = stoi(newAffectedLiteral);
    }
    int affectedLiteral = 0;
    if (it != blockAssignments.end()) affectedLiteral = *it;
    return make_tuple(stoi(newLiteral), affectedLiteral, newAffectedLiteralInt);
}

/**
 * For each unit clause in the formula, we assume it to be a literal assignment.
 * Add this literal assignment to its own sudoku block.
 */
map<int, vector<int> > DavisPutnam::getAssignmentsInBlocks(formula formula) {
    map<int, vector<int> > blocks;
    for (auto const& clause : formula.clauses) {
        if (clause.size() == 1) {
            int const literal = clause.at(0);
            int const idx = getBlockIndex(to_string(literal));
            blocks[idx].push_back(literal);
        }
    }
    return blocks;
}

/**
 * Returns the index of the block within the sudoku in which the given
 * literal assignment exists.
 */
int DavisPutnam::getBlockIndex(string literal) {
    double const y = (double) literal.at(0) - '0';
    double const x = (double) literal.at(1) - '0';
    int blocksPerRow = stats.ymax/stats.blockSize;
    int rows = ceil((double)y/(double)stats.blockSize) - 1;
    int idx = rows * blocksPerRow + ceil((double)x/(double)stats.blockSize);
    return idx;
}

/**
 *
 */
char DavisPutnam::getRandomPositionInBlock(char currentPosition, int presetDoAddition) {
    int availableSpace;
    // First pick if subtract or add
    int doAddition = presetDoAddition != -1 ? presetDoAddition : rand() % 2;
    // Calculate the current position (in the range) relative to its block
    int currPosWithinBlock = ((currentPosition-1) % stats.blockSize)+1;
    // Calculate the available space within the block according to the
    // randomly picked operator (subtraction or addition).
    if (doAddition == 0) {
        availableSpace = currPosWithinBlock - 1;
    } else {
        availableSpace = stats.blockSize - currPosWithinBlock;
    }
    // If there is no space, we want to try picking a position again, hoping
    // we get the opposite operator this time that has space.
    if (availableSpace == 0) {
        doAddition = doAddition == 0 ? 1 : 0;
        return getRandomPositionInBlock(currentPosition, doAddition);
    }
    // The mutation should be between 0 and the available step space.
    int randomMutation = rand() % (availableSpace + 1);
    return currentPosition + pow(-1, (1+doAddition))*randomMutation;
}

/**
 * Perform essential steps before starting the recursive Davis Putnam algorithm,
 * such as removing tautologies from the initial Formula.
 */
formula DavisPutnam::setup(formula formula) {
    formula = removeTautologies(formula);
    stats.ymax = to_string(*getLiterals(formula).rbegin()).at(0) - '0';
    stats.blockSize = sqrt(stats.ymax);
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
    if (literal == 0) return {};
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
    if (strategy != "-S1" && !new_formula.clauses.empty()) {
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
        return getNextRandomLiteral(formula, currentVariables);
    } else if (strategy == "-S2" || strategy == "-S3" || strategy == "-S4") {
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
        ++stats.nRandomNumbers;
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

set<int> DavisPutnam::getInitialAssignments(formula formula) {
    set<int> assignments;
    for (const auto& clause : formula.clauses) {
        if (clause.size() == 1) {
            assignments.insert(clause[0]);
        }
    }
    return assignments;
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

int char2int(char n) {
    return n - '0';
}

