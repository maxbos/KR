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

using namespace std;

class DavisPutnam {
  string strategy;
  string inputFilePath;
public:
  DavisPutnam           (string strategy, string inputFilePath);
  void simplify         ();
};

int main() {
  DavisPutnam davisPutnam("S1", "./iets");
  
  return 0;
}

DavisPutnam::DavisPutnam(string strategy, string inputFilePath)
  : strategy(strategy), inputFilePath(inputFilePath) {

}

void DavisPutnam::simplify() {

}
