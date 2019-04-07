/*
 * preprocess.cpp
 * Knowledge Representation: Project 1 (SAT)
 *
 * This program expects DIMACS filepaths as arguments, and
 * concatenates these files into one DIMACS file.
 * The last argument is the output file name.
 * 
 * Studentnames:     Max Bos & Erik Stammes
 * Studentnumbers:   10669027 & 10559736
 */

#include <fstream>
#include <iostream>
#include <cstdlib>
#include <string>
#include <map>
#include <regex>
#include <cmath>

using namespace std;

map<string, string> getCommandValues          (int, char**);
vector<vector<string> > parseSudokusFromFile  (string);

// Command line input: ./SAT -r [RULES_FILE_PATH] -s [SUDOKUS_FILE_PATH] -o [OUTPUT_FOLDER]
int main(int argc, char** argv) {
  map<string, string> commandValues = getCommandValues(argc, argv);
  vector<vector<string> > sudokus = parseSudokusFromFile(commandValues.at("-s"));
  // Create an output file for each sudoku.
  for (int i = 0; i < sudokus.size(); i++) {
    vector<string> const& sudoku = sudokus[i];
    // Open the output file for this sudoku.
    ofstream ofile(commandValues.at("-o") + '/' + to_string(i) + ".txt");
    // Open the rules file.
    ifstream ifile(commandValues.at("-r"));
    // Add the rules to the output file.
    ofile << ifile.rdbuf();
    // Add the sudoku to the output file.
    for (string const& literal : sudoku) {
      ofile << literal << " 0" << endl;
    }
  }
  return 0;
}

map<string, string> getCommandValues(int argc, char** argv) {
  string key;
  map<string, string> commandValues;
  const regex flag_regex("-.");
  for (int i = 1; i < argc; i++) {
    string val = argv[i];
    // if has a - for a flag is is a key
    if (regex_match(val, flag_regex)) {
      key = val;
    } else if (!key.empty()) {
      commandValues[key] = argv[i];
    }
  }
  return commandValues;
}

vector<vector<string> > parseSudokusFromFile(string sudokusFilePath) {
  ifstream ifile(sudokusFilePath);
  string gameState;
  vector<vector<string> > sudokus;
  vector<string> sudoku;
  while (ifile >> gameState) {
    for (int i = 0; i < 81; i++) {
      char pos = gameState[i];
      if (pos == '.') continue;
      int const val = pos - '0';
      int row = ceil((double) (i+1) / 9);
      int col = (i % 9) + 1;
      sudoku.push_back(to_string(row) + to_string(col) + to_string(val));
    }
    sudokus.push_back(sudoku);
    sudoku = {};
  }
  return sudokus;
}
