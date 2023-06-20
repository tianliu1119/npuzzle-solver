#include <iostream>
#include <string>
#include <vector>
#include "npuzzle.h"
#include "puzzles.h"
using namespace std;

int main() {
  int puzzleChoice = 1;
  int algorithmChoice = 1;
  string puzzleInput = "";
  vector<int> puzzle = DefaultPuzzle::Fifteen::waitForIt;

  cout << "Welcome to Group 26's 8 puzzle solver." << endl;
  cout << "Type \"1\" to use a default puzzle, or \"2\" to enter your own puzzle: ";
  cin >> puzzleChoice;
  cin.ignore();

  if (puzzleChoice != 1 && puzzleChoice != 2) {
    cout << endl << "Invalid input. Exiting..." << endl;
    return 0;
  }

  if (puzzleChoice == 2) {
    cout << endl;
    cout << "Enter your puzzle on one line. Use space between numbers," << endl;
    cout << "and 0 to represent the blank. Press ENTER/RETURN when done." << endl;
    cout << "Enter puzzle: ";
    getline(cin, puzzleInput);
    cin.ignore();

    string intStr = "";
    puzzle.clear();
    for (int i = 0; i < puzzleInput.length(); ++i) {
      if (puzzleInput[i] == ' ') {
        puzzle.push_back(stoi(intStr));
        intStr = "";
        continue;
      }
      intStr += puzzleInput[i];
    }
    puzzle.push_back(stoi(intStr));
  }

  cout << endl;
  cout << "1. Uniform Cost Search" << endl;
  cout << "2. A* with the Misplaced Tile heuristic." << endl;
  cout << "3. A* with the Euclidean distance heuristic." << endl;
  cout << "Enter your choice of algorithm: ";
  cin >> algorithmChoice;
  cin.ignore();

  if (algorithmChoice < 1 || algorithmChoice > 5) {
    cout << endl << "Invalid input. Exiting..." << endl;
    return 0;
  }
  cout << endl;

  NPuzzle thePuzzle(puzzle);
  thePuzzle.solvePuzzle(algorithmChoice);
  thePuzzle.displaySolution();

  return 0;
}
