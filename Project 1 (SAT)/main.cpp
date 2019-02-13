/*
 * main.cpp
 * Knowledge Representation: Project 1 (SAT)
 *
 * Studentnames:     Max Bos & Erik Stammes
 * Studentnumbers:   10669027 & ...
 */

#include <fstream>
#include <iostream>
#include <cstdlib>
#include <string>
#include <vector>

using namespace std;

class DavisPutnam {
  string strategy;
  string inputFilePath;
  pair<vector<vector<int>>, vector<int>> unitPropagate (vector<vector<int>>, vector<int>);
public:
  DavisPutnam                         (string strategy, string inputFilePath);
  void recursive                      (vector<vector<int>>, vector<int>);
  void simplify                       ();
};

int main() {
  DavisPutnam davisPutnam("S1", "./iets");
  
  return 0;
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

void DavisPutnam::recursive(vector<vector<int>> F, vector<int> partialAssignments) {
  simplify();
  // split();
  // backtrack();
}

void DavisPutnam::simplify() {

}
