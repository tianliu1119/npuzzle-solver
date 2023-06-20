#ifndef NPUZZLE_H
#define NPUZZLE_H

#include <cmath>
#include <cstdlib>
#include <iostream>
#include <queue>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

/*********************************************************************
 *
 * NPUZZLE
 *
 *--------------------------------------------------------------------
 * File Contents
 *   struct PuzzleState: represents a particular state of an N-puzzle
 *   struct CompareCost: defines a comparator for PuzzleState objects
 *   class NPuzzle: solves an 8-puzzle of any size, or an N-puzzle
 *********************************************************************/

/*********************************************************************
 * PuzzleState (struct)
 *   Manages data related to a particular puzzle state, representing
 *   puzzle numbers as a vector of integers.
 *********************************************************************/
struct PuzzleState
{
  std::vector<int> state;  // puzzle numbers stored in the order they appear in the puzzle
  int blankIdx;            // index of blank square within the vector
  int g;                   // cost from initial state (operations from starting state)
  float h;                 // heuristic cost (estimated operations to achieve goal state)
  float f;                 // total cost (g + h)
  int move;                // previous blank square move (1-UP, 2-DOWN, 3-LEFT, 4-RIGHT)
  std::string parentKey;   // key of parent state (indexes an unordered_map of states)

  // CONSTRUCTORS
  PuzzleState()
    : state({}), blankIdx(0), g(0), h(0), f(0), move(0), parentKey("") {}
  PuzzleState(std::vector<int> state)
    : state(state), blankIdx(0), g(0), h(0), f(0), move(0), parentKey("") {}

  // Overloaded equality operator
  bool operator==(const PuzzleState& s)
  {
    // PuzzleState objects are equal if their puzzle vectors are the same
    return state == s.state;
  }
};

/*********************************************************************
 * CompareCost (struct)
 *   Defines a comparator for PuzzleState objects in a container to
 *   be ordered in terms of increasing total cost.
 *********************************************************************/
struct CompareCost
{
  bool operator()(const PuzzleState& p1, const PuzzleState& p2) const
  {
    return p1.f > p2.f;
  }
};

/*********************************************************************
 * NPuzzle Class
 *   Solves a square N-puzzle using a specified search algorithm,
 *   presenting the solution as a sequence of blank square operations.
 *   Employs one of three search techniques:
 *   1) Uniform Cost Search
 *   2) A* with Misplaced Tile heuristic
 *   3) A* with Euclidean Distance heuristic
 *********************************************************************/
class NPuzzle
{
  public:
    // CONSTRUCTOR
    NPuzzle(std::vector<int> startState);

    // PUBLIC METHODS
    int size();
    int nodesExpanded();
    int maxQueueSize();
    int goalNodeDepth();
    PuzzleState startState();
    std::vector<PuzzleState> solution();
    std::vector<PuzzleState> solve(int heuristic);
    std::vector<PuzzleState> solveVerbose(int heuristic);
    void displaySolution();

  private:
    // PRIVATE METHODS
    bool isSolvable();
    bool isGoal(const PuzzleState& current);
    float getHeuristicCost(const PuzzleState& current, int heuristic);
    float misplacedTile(const PuzzleState& current);
    float euclideanDist(const PuzzleState& current);
    float manhattanDist(const PuzzleState& current);
    float manhattanDistLinearConflict(const PuzzleState& current);
    std::vector<PuzzleState> generateChildren(const PuzzleState& current);
    std::vector<PuzzleState> retracePath(const PuzzleState& current);
    std::string getKey(const PuzzleState& current);
    void displayState(const PuzzleState& current);

    // ATTRIBUTES
    int nsz;            // size N of the N-puzzle
    int len;            // length of puzzle vector, or puzzle size plus blank tile (n+1)
    int dim;            // dimension or side length of the square puzzle (sqrt(len))
    int expanded;       // total number of nodes expanded
    int maxQueue;       // maximum number of nodes in the frontier queue at any moment
    int goalDepth;      // length of path to solution including initial state
    bool solvable;      // true if puzzle is solvable from initial state
    PuzzleState start;  // initial puzzle state
    std::vector<PuzzleState> result;  // sequence of states constituting path to solution
    std::unordered_map<std::string, PuzzleState> frontierStates;// current frontier states
    std::unordered_map<std::string, PuzzleState> exploredStates;// current explored states
};

#endif // NPUZZLE_H
