#include "npuzzle.h"
using namespace std;

/*********************************************************************
 *
 * NPuzzle::NPuzzle - Constructor
 *
 *--------------------------------------------------------------------
 * Initializes attributes according to the given starting state.
 *--------------------------------------------------------------------
 * PARAMETERS
 *   vector<int> startState: the starting state of the puzzle
 *--------------------------------------------------------------------
 * PRE-CONDITIONS
 *   Starting state of puzzle and vector/puzzle size must be valid.
 * POST-CONDITIONS
 *   Class attributes initialized to default values. Index position
 *   of the blank square is identified within the state vector.
 *********************************************************************/
NPuzzle::NPuzzle(vector<int> startState)
{
  start = PuzzleState(startState);

  // Find the index position of the blank tile
  for (int i = 0; i < startState.size(); ++i)
  {
    if (startState[i] == 0)
    {
      start.blankIdx = i;
      break;
    }
  }

  // Initialize attributes
  len = startState.size();
  nsz = len - 1;
  dim = sqrt(len);
  expanded = 0;
  maxQueue = 0;
  goalDepth = 0;
  solvable = isSolvable();
}

int NPuzzle::size()
{
  return nsz;
}

int NPuzzle::nodesExpanded()
{
  return expanded;
}

int NPuzzle::maxQueueSize()
{
  return maxQueue;
}

int NPuzzle::goalNodeDepth()
{
  return goalDepth;
}

PuzzleState NPuzzle::startState()
{
  return start;
}

vector<PuzzleState> NPuzzle::solution()
{
  return result;
}

/*********************************************************************
 *
 * NPuzzle::solve - Public Method
 *
 *--------------------------------------------------------------------
 * Runs a graph-search algorithm using the specified heuristic to
 * find an optimal solution to the N-puzzle.
 *
 * Maximizes efficiency by using a priority queue configured as a
 * min-heap to store frontier states based on their current total
 * cost, and by using unordered maps (stored as class attributes) to
 * store explored states as well as states currently in the frontier
 * queue to allow for fast retrieval of states and to efficiently
 * check if a state already exists, in constant time.
 *--------------------------------------------------------------------
 * PARAMETERS
 *   int heuristic: indicates the heuristic function to use
 *                  1 - Uniform Cost Search
 *                  2 - A* with Misplaced Tile heuristic
 *                  3 - A* with Euclidean Distance heuristic
 * RETURNS
 *   The solution to the puzzle in the form of a vector of states
 *   that represent the optimal sequence of blank square operations
 *   leading to the goal state.
 *--------------------------------------------------------------------
 * PRE-CONDITIONS
 *   The integer value indicating the heuristic to use must be valid.
 * POST-CONDITIONS
 *   Stores the solution to the puzzle and the data collected during
 *   the graph-search process in the appropriate class attributes.
 *********************************************************************/
vector<PuzzleState> NPuzzle::solve(int heuristic)
{
  priority_queue<PuzzleState, vector<PuzzleState>, CompareCost> frontierQueue;
  vector<PuzzleState> children;  // children states generated from the current state
  PuzzleState current;           // the current state being expanded
  string currentKey = "";        // key of the current state
  string childKey = "";          // key of a child state

  if (!solvable)
  {
    result.clear();
    return result;
  }

  // Initialize the cost values of the starting state using the specified heuristic
  start.g = 0;
  start.h = getHeuristicCost(start, heuristic);
  start.f = start.g + start.h;

  // Place the starting state into the frontier queue
  currentKey = getKey(start);
  frontierQueue.push(start);
  frontierStates[currentKey] = start;

  // Continue expanding states until the queue becomes empty or a goal state is reached
  while (!frontierQueue.empty())
  {
    // Set the current state to the state on top of the frontier queue, which is the
    // frontier state with the least total cost
    current = frontierQueue.top();

    // Remove the current state from the frontier
    currentKey = getKey(current);
    frontierQueue.pop();
    frontierStates[currentKey] = PuzzleState();

    // If the goal state is reached, obtain the solution path and output the time and
    // space resources used by the search algorithm
    if (isGoal(current))
    {
      result = retracePath(current);
      goalDepth = result.size();
      break;
    }

    // Check if the current state already exists as an explored state and whether it
    // should be expanded
    if (exploredStates[currentKey].state.empty())
    {
      // Increment the nodes-expanded counter, add the current state to the list of
      // explored states, and generate a list of children states
      expanded++;
      exploredStates[currentKey] = current;
      children = generateChildren(current);

      // Initialize the attributes of each child state to correct values
      for (int i = 0; i < children.size(); ++i)
      {
        // Get the key of the child state to check if it already exists as a frontier or
        // explored state and whether it should be added to the frontier queue
        childKey = getKey(children[i]);
        if (!frontierStates[childKey].state.empty() ||
            !exploredStates[childKey].state.empty())
          continue;

        // The cost g(n) of the child state is the g(n) of the current state plus 1, and
        // the heuristic cost h(n) is calculated using the specified heuristic
        children[i].g = current.g + 1;
        children[i].h = getHeuristicCost(children[i], heuristic);
        children[i].f = children[i].g + children[i].h;
        // The parent key property of the child state is the key of the current state
        children[i].parentKey = currentKey;

        // Add the child state to the frontier
        frontierQueue.push(children[i]);
        frontierStates[childKey] = children[i];
      }

      // Update the maximum recorded number of nodes in the queue if necessary
      if (frontierQueue.size() > maxQueue)
        maxQueue = frontierQueue.size();
    }
  }

  return result;
}

/*********************************************************************
 *
 * NPuzzle::solveVerbose - Public Method
 *
 *--------------------------------------------------------------------
 * Same as the solve() function but with console output.
 *
 * Outputs relevant information at each step of the solving process
 * and, upon puzzle completion, displays a concluding statement that
 * provides insight into the time and space management of the search
 * algorithm used.
 *--------------------------------------------------------------------
 * PARAMETERS
 *   int heuristic: indicates the heuristic function to use
 *                  1 - Uniform Cost Search
 *                  2 - A* with Misplaced Tile heuristic
 *                  3 - A* with Euclidean Distance heuristic
 * RETURNS
 *   The solution to the puzzle in the form of a vector of states
 *   that represent the optimal sequence of blank square operations
 *   leading to the goal state.
 *--------------------------------------------------------------------
 * PRE-CONDITIONS
 *   The integer value indicating the heuristic to use must be valid.
 * POST-CONDITIONS
 *   Stores the solution to the puzzle and the data collected during
 *   the graph-search process in the appropriate class attributes.
 *********************************************************************/
vector<PuzzleState> NPuzzle::solveVerbose(int heuristic)
{
  priority_queue<PuzzleState, vector<PuzzleState>, CompareCost> frontierQueue;
  vector<PuzzleState> children;  // children states generated from the current state
  PuzzleState current;           // the current state being expanded
  string currentKey = "";        // key of the current state
  string childKey = "";          // key of a child state
  bool startExpanded = false;    // indicates whether the starting state has been expanded

  if (!solvable)
  {
    cout << "PUZZLE IS NOT SOLVABLE" << endl;
    result.clear();
    return result;
  }

  cout << "SOLVING PUZZLE..." << endl << endl;

  // Initialize the cost values of the starting state using the specified heuristic
  start.g = 0;
  start.h = getHeuristicCost(start, heuristic);
  start.f = start.g + start.h;

  // Place the starting state into the frontier queue
  currentKey = getKey(start);
  frontierQueue.push(start);
  frontierStates[currentKey] = start;

  // Continue expanding states until the queue becomes empty or a goal state is reached
  while (!frontierQueue.empty())
  {
    // Set the current state to the state on top of the frontier queue, which is the
    // frontier state with the least total cost
    current = frontierQueue.top();

    // Remove the current state from the frontier
    currentKey = getKey(current);
    frontierQueue.pop();
    frontierStates[currentKey] = PuzzleState();

    // If the goal state is reached, obtain the solution path and output the time and
    // space resources used by the search algorithm
    if (isGoal(current))
    {
      displayState(current);
      result = retracePath(current);
      goalDepth = result.size();
      cout << endl << "GOAL" << endl << endl;
      cout << "To solve this problem, the search algorithm expanded a total of ";
      cout << expanded << " nodes." << endl;
      cout << "The maximum number of nodes in the queue at any one time was ";
      cout << maxQueue << "." << endl;
      cout << "The depth of the goal node was " << goalDepth << "." << endl;
      break;
    }

    // Check if the current state already exists as an explored state and whether it
    // should be expanded
    if (exploredStates[currentKey].state.empty())
    {
      if (startExpanded)
      {
        // Display the current state being expanded and its cost values
        cout << "The best state to expand with g(n) = " << current.g;
        cout << " and h(n) = " << current.h << " is..." << endl;
        displayState(current);
        cout << "Expanding this node..." << endl << endl;
      }
      else
      {
        // Cost values are not displayed when expanding the starting state
        cout << "Expanding state" << endl;
        displayState(current);
        cout << endl;
        startExpanded = true;
      }

      // Increment the nodes-expanded counter, add the current state to the list of
      // explored states, and generate a list of children states
      expanded++;
      exploredStates[currentKey] = current;
      children = generateChildren(current);

      // Initialize the attributes of each child state to correct values
      for (int i = 0; i < children.size(); ++i)
      {
        // Get the key of the child state to check if it already exists as a frontier or
        // explored state and whether it should be added to the frontier queue
        childKey = getKey(children[i]);
        if (!frontierStates[childKey].state.empty() ||
            !exploredStates[childKey].state.empty())
          continue;

        // The cost g(n) of the child state is the g(n) of the current state plus 1, and
        // the heuristic cost h(n) is calculated using the specified heuristic
        children[i].g = current.g + 1;
        children[i].h = getHeuristicCost(children[i], heuristic);
        children[i].f = children[i].g + children[i].h;
        // The parent key property of the child state is the key of the current state
        children[i].parentKey = currentKey;

        // Add the child state to the frontier
        frontierQueue.push(children[i]);
        frontierStates[childKey] = children[i];
      }

      // Update the maximum recorded number of nodes in the queue if necessary
      if (frontierQueue.size() > maxQueue)
        maxQueue = frontierQueue.size();
    }
  }

  return result;
}

/*********************************************************************
 *
 * NPuzzle::isSolvable - Private Method
 *
 *--------------------------------------------------------------------
 * Determines whether the puzzle is solvable by counting the number
 * of inversions in the starting state. An inversion is formed when a
 * tile comes before another tile with a lower value.
 *--------------------------------------------------------------------
 * RETURNS
 *   A boolean value that is true if the puzzle is solvable, or false
 *   if it is unsolvable.
 *********************************************************************/
bool NPuzzle::isSolvable()
{
  int inversionCount = 0;  // total number of inversions

  // Count the number of inversions in the puzzle's starting state
  for (int i = 0; i < len; ++i)
  {
    // Skip blank tile
    if (start.state[i] == 0)
      continue;

    for (int j = i + 1; j < len; ++j)
    {
      // Skip blank tile
      if (start.state[j] == 0)
        continue;

      if (start.state[j] < start.state[i])
        inversionCount++;
    }
  }

  if (dim % 2 == 1)
  {
    // If the puzzle dimension is odd, then the puzzle is solvable only if the number of
    // inversions is even
    if (inversionCount % 2 == 0)
      return true;
  }
  else  // dim % 2 == 0
  {
    // If the puzzle dimension is even, then the puzzle is solvable only if either the
    // number of inversions is odd and the blank tile is on an even row from the bottom,
    // or the number of inversions is even and the blank tile is on an odd row from bottom
    if (inversionCount % 2 == 1 && (dim - (start.blankIdx / dim)) % 2 == 0)
      return true;
    if (inversionCount % 2 == 0 && (dim - (start.blankIdx / dim)) % 2 == 1)
      return true;
  }

  return false;
}

/*********************************************************************
 *
 * NPuzzle::isGoal - Private Method
 *
 *--------------------------------------------------------------------
 * Determines whether the given state is the goal state by checking
 * if each number in the puzzle is in its correct position.
 *--------------------------------------------------------------------
 * PARAMETERS
 *   const PuzzleState& current: the state to check if it is the goal
 * RETURNS
 *   A boolean value that is true if the given state is the goal
 *   state, or false if otherwise.
 *********************************************************************/
bool NPuzzle::isGoal(const PuzzleState& current)
{
  // Check every square if it is in the correct position
  for (int i = 0; i < len; ++i)
  {
    if (current.state[i] == 0)
      continue;
    if (current.state[i] != i + 1)
      return false;
  }
  return true;
}

/*********************************************************************
 *
 * NPuzzle::getHeuristicCost - Private Method
 *
 *--------------------------------------------------------------------
 * Invokes the correct, specified heuristic function to calculate the
 * heuristic cost of a given state. If Uniform Cost Search is used,
 * the heuristic cost will be 0.
 *--------------------------------------------------------------------
 * PARAMETERS
 *   int heuristic: indicates the heuristic function to use
 *                  1 - Uniform Cost Search
 *                  2 - A* with Misplaced Tile heuristic
 *                  3 - A* with Euclidean Distance heuristic
 * RETURNS
 *   The heuristic cost of the given state.
 *--------------------------------------------------------------------
 * PRE-CONDITION
 *   The integer value indicating the heuristic to use must be valid.
 *   Otherwise, a default cost of 0 will be returned.
 *********************************************************************/
float NPuzzle::getHeuristicCost(const PuzzleState& current, int heuristic)
{
  // Get the heuristic cost calculated using the specified heuristic function
  if (heuristic == 2)       // A* with Misplaced Tile heuristic
    return misplacedTile(current);
  else if (heuristic == 3)  // A* with Euclidean Distance heuristic
    return euclideanDist(current);
  else if (heuristic == 4)  // A* with Manhattan Distance heuristic
    return manhattanDist(current);
  else if (heuristic == 5)  // A* with Manhattan Distance and Linear Conflict
    return manhattanDistLinearConflict(current);
  else  // heuristic == 1  --> Uniform Cost Search
    return 0;
}

/*********************************************************************
 *
 * NPuzzle::misplacedTile - Private Method
 *
 *--------------------------------------------------------------------
 * Calculates the heuristic cost of a given state by using the
 * Misplaced Tile heuristic. The cost is the number of tiles that are
 * not in their correct positions.
 *--------------------------------------------------------------------
 * PARAMETERS
 *   const PuzzleState& current: the state to calculate the cost for
 * RETURNS
 *   The Misplaced Tile heuristic cost of the given state.
 *********************************************************************/
float NPuzzle::misplacedTile(const PuzzleState& current)
{
  float cost = 0;  // Misplaced Tile heuristic cost

  // Count how many tiles are in incorrect positions
  for (int i = 0; i < len; ++i)
  {
    // Skip blank tile
    if (current.state[i] == 0)
      continue;

    if (current.state[i] != i + 1)
      cost++;
  }

  return cost;
}

/*********************************************************************
 *
 * NPuzzle::euclideanDist - Private Method
 *
 *--------------------------------------------------------------------
 * Calculates the heuristic cost of a given state by using the
 * Euclidean Distance heuristic. The cost is the sum of the Euclidean
 * distances of the tiles from their correct positions. The Euclidean
 * distance of a tile is calculated with the following formula:
 * sqrt((CurrentRow - GoalRow)^2 + (CurrentColumn - GoalColumn)^2).
 *--------------------------------------------------------------------
 * PARAMETERS
 *   const PuzzleState& current: the state to calculate the cost for
 * RETURNS
 *   The Euclidean Distance heuristic cost of the given state.
 *********************************************************************/
float NPuzzle::euclideanDist(const PuzzleState& current)
{
  int rowDist = 0;  // row distance component of Euclidean distance
  int colDist = 0;  // column distance component of Euclidean distance
  float cost = 0;   // Euclidean Distance heuristic cost

  // Calculate and sum up the Euclidean distance of each tile
  for (int i = 0; i < len; ++i)
  {
    // Skip blank tile
    if (current.state[i] == 0)
      continue;

    // RowDistance = CurrentRow - GoalRow
    rowDist = (i / dim) - ((current.state[i] - 1) / dim);
    // ColumnDistance = CurrentColumn - GoalColumn
    colDist = (i % dim) - ((current.state[i] - 1) % dim);
    // EuclideanDistance = sqrt(RowDistance^2 + ColumnDistance^2)
    cost += sqrt((rowDist * rowDist) + (colDist * colDist));
  }

  return cost;
}

/*********************************************************************
 *
 * NPuzzle::manhattanDist - Private Method
 *
 *--------------------------------------------------------------------
 * Calculates the heuristic cost of a given state by using the
 * Manhattan Distance heuristic. The cost is the sum of the Manhattan
 * distances of the tiles from their correct positions. The Manhattan
 * distance of a tile is calculated with the following formula:
 * |GoalRow - CurrentRow| + |GoalColumn - CurrentColumn|
 *--------------------------------------------------------------------
 * PARAMETERS
 *   const PuzzleState& current: the state to calculate the cost for
 * RETURNS
 *   The Manhattan Distance heuristic cost of the given state.
 *********************************************************************/
float NPuzzle::manhattanDist(const PuzzleState &current)
{
  int rowDiff = 0;
  int colDiff = 0;
  float cost = 0;

  for (int i = 0; i < len; ++i)
  {
    // Skip blank tile
    if (current.state[i] == 0)
      continue;

    // RowDifference = GoalRow - CurrentRow
    rowDiff = ((current.state[i] - 1) / dim) - (i / dim);
    // ColumnDifference = GoalColumn - CurrentColumn
    colDiff = ((current.state[i] - 1) % dim) - (i % dim);
    // ManhattanDistance = |RowDifference| + |ColumnDifference|
    cost += abs(rowDiff) + abs(colDiff);
  }

  return cost;
}

/*********************************************************************
 *
 * NPuzzle::manhattanDistLinearConflict - Private Method
 *
 *--------------------------------------------------------------------
 * Calculates the heuristic cost of a given state by using the
 * Manhattan Distance heuristic combined with the linear conflict
 * heuristic. The cost is the Manhattan Distance cost plus the linear
 * conflict cost. A linear conflict occurs between a pair of tiles
 * in the same row if the row is the goal row of both tiles, but the
 * tile with the higher value is preceding the other. A linear
 * conflict occurs between tiles within a column in the same way.
 * Each linear conflict, row or column, increases the cost by 2.
 *--------------------------------------------------------------------
 * PARAMETERS
 *   const PuzzleState& current: the state to calculate the cost for
 * RETURNS
 *   The calculated heuristic cost of the given state.
 *********************************************************************/
// Manhattan Distance heuristic combined with the linear conflict
// heuristic to provide cost adjustments for improved cost estimates
// (This heuristic combination remains admissible)
float NPuzzle::manhattanDistLinearConflict(const PuzzleState &current)
{
  int rowDiff = 0;
  int colDiff = 0;
  float cost = 0;

  for (int i = 0; i < len; ++i)
  {
    // Skip blank tile
    if (current.state[i] == 0)
      continue;

    // Linear conflict - count the number of row conflicts
    if ((current.state[i] - 1) / dim == i / dim)
    {
      for (int j = i + 1; j < ((i / dim) * dim) + dim; ++j)
      {
        if (current.state[j] == 0)
          continue;
        if (((current.state[j] - 1) / dim) != i / dim)
          continue;
        if (current.state[j] < current.state[i])
          cost += 2;
      }
    }

    // Linear conflict - count the number of column conflicts
    if ((current.state[i] - 1) % dim == i % dim)
    {
      for (int j = i + dim; j < len; j += dim)
      {
        if (current.state[j] == 0)
          continue;
        if ((current.state[j] - 1) % dim != i % dim)
          continue;
        if (current.state[j] < current.state[i])
          cost += 2;
      }
    }

    // RowDifference = GoalRow - CurrentRow
    rowDiff = ((current.state[i] - 1) / dim) - (i / dim);
    // ColumnDifference = GoalColumn - CurrentColumn
    colDiff = ((current.state[i] - 1) % dim) - (i % dim);
    // Manhattan Distance = |RowDifference| + |ColumnDifference|
    cost += abs(rowDiff) + abs(colDiff);
  }

  return cost;
}

/*********************************************************************
 *
 * NPuzzle::generateChildren - Private Method
 *
 *--------------------------------------------------------------------
 * Generates a list of children states that can result from the given
 * state depending on the possible blank square operations that can
 * be performed.
 *--------------------------------------------------------------------
 * PARAMETERS
 *   const PuzzleState& current: the state from which to generate
 *                               children states
 * RETURNS
 *   A vector of possible children states that can result from the
 *   given state.
 * --------------------------------------------------------------------
 * POST-CONDITION
 *   Only initializes child state attributes that relate to the
 *   child's state vector, blank square index, and blank square move.
 *********************************************************************/
vector<PuzzleState> NPuzzle::generateChildren(const PuzzleState& current)
{
  vector<PuzzleState> children;             // list of children states
  int currentRow = current.blankIdx / dim;  // row position of blank square
  int currentCol = current.blankIdx % dim;  // column position of blank square

  // Add child if blank square can move UP
  if (currentRow > 0)
  {
    PuzzleState child = current;

    // Move blank square and update child state's properties
    child.state[child.blankIdx] = child.state[child.blankIdx - dim];
    child.state[child.blankIdx - dim] = 0;
    child.blankIdx -= dim;
    child.move = 1;

    // Add child to list of children states
    children.push_back(child);
  }

  // Add child if blank square can move DOWN
  if (currentRow < dim - 1)
  {
    PuzzleState child = current;

    child.state[child.blankIdx] = child.state[child.blankIdx + dim];
    child.state[child.blankIdx + dim] = 0;
    child.blankIdx += dim;
    child.move = 2;

    children.push_back(child);
  }

  // Add child if blank square can move LEFT
  if (currentCol > 0)
  {
    PuzzleState child = current;

    child.state[child.blankIdx] = child.state[child.blankIdx - 1];
    child.state[child.blankIdx - 1] = 0;
    child.blankIdx -= 1;
    child.move = 3;

    children.push_back(child);
  }

  // Add child if blank square can move RIGHT
  if (currentCol < dim - 1)
  {
    PuzzleState child = current;

    child.state[child.blankIdx] = child.state[child.blankIdx + 1];
    child.state[child.blankIdx + 1] = 0;
    child.blankIdx += 1;
    child.move = 4;

    children.push_back(child);
  }

  return children;
}

/*********************************************************************
 *
 * NPuzzle::retracePath - Private Method
 *
 *--------------------------------------------------------------------
 * Generates the sequence of intermediate states leading up to the
 * given state from the starting state. Uses the parent key property
 * of each intermediate state to retrieve the parent state from the
 * unordered map of explored states, which can be accomplished in
 * constant time.
 *--------------------------------------------------------------------
 * PARAMETERS
 *   const PuzzleState& current: the state to generate the path to
 * RETURNS
 *   A vector of intermediate states that lead to the given state
 *   from the starting state.
 *********************************************************************/
vector<PuzzleState> NPuzzle::retracePath(const PuzzleState& current)
{
  deque<PuzzleState> path;  // sequence of states leading to current state
  PuzzleState parent;       // parent state of the current intermediate state
  string parentKey = "";    // key of parent to retrieve it from list of explored states

  // Add the current state to the path and get the key of the parent state
  path.push_front(current);
  parentKey = current.parentKey;

  // Continue adding states to the path until an empty parent key is encountered, which
  // indicates that the starting state has been reached
  while (parentKey != "")
  {
    // Retrieve the parent state from the list of explored states using the parent's key
    parent = exploredStates[parentKey];
    // Add the parent state to the front of the sequence
    path.push_front(parent);
    parentKey = parent.parentKey;
  }

  return vector<PuzzleState>(path.begin(), path.end());
}

/*********************************************************************
 *
 * NPuzzle::getKey - Private Method
 *
 *--------------------------------------------------------------------
 * Generates a unique key that identifies the given state in a
 * container by creating a string representing the concatenation of
 * all of the numbers in the puzzle.
 *--------------------------------------------------------------------
 * PARAMETERS
 *   const PuzzleState& current: the state to generate a key for
 * RETURNS
 *   A string representing the unique key of the given state.
 *********************************************************************/
string NPuzzle::getKey(const PuzzleState& current)
{
  string key = "";  // key of the current state

  // Concatenate all puzzle numbers to form key
  for (int i = 0; i < len; ++i)
  {
    key += to_string(current.state[i]);
  }

  return key;
}

/*********************************************************************
 *
 * NPuzzle::displayState - Private Method
 *
 *--------------------------------------------------------------------
 * Displays a given state of the puzzle by formatting the output of
 * numbers to ensure proper alignment, regardless of puzzle size.
 *--------------------------------------------------------------------
 * PARAMETERS
 *   const PuzzleState& current: the state to display
 *--------------------------------------------------------------------
 * PRE-CONDITION
 *   The given puzzle state must be valid, and its vector size must
 *   be equal to the starting state's vector size.
 *********************************************************************/
void NPuzzle::displayState(const PuzzleState& current)
{
  int paddingSize = to_string(nsz).length();  // number of spaces to pad after a number
  string defaultPadding(paddingSize, ' ');    // default padding of spaces
  string currentPadding(paddingSize, ' ');    // padding of spaces for current number

  // Display the properly formatted puzzle state
  for (int i = 0; i < len; ++i)
  {
    cout << current.state[i];
    paddingSize = defaultPadding.length() - to_string(current.state[i]).length() + 1;
    currentPadding = string(paddingSize, ' ');
    cout << currentPadding;

    if (i % dim == dim - 1)
      cout << endl;
  }
}

/*********************************************************************
 *
 * NPuzzle::displaySolution - Public Method
 *
 *--------------------------------------------------------------------
 * Displays the solution to the puzzle by showing each intermediate
 * state and operation. Ensures puzzle outputs have proper formatting
 * and alignment regardless of puzzle size.
 *--------------------------------------------------------------------
 * PRE-CONDITION
 *   The solvePuzzle function must have been called in order for this
 *   function to display the correct solution.
 *********************************************************************/
void NPuzzle::displaySolution()
{
  int paddingSize = to_string(nsz).length();  // number of spaces to pad after a number
  string defaultPadding(paddingSize, ' ');    // default padding of spaces
  string currentPadding(paddingSize, ' ');    // padding of spaces for current number

  cout << endl << "*************** SOLUTION ****************" << endl << endl;

  // If the result vector is empty, a solution could not be found for the puzzle
  if (result.empty())
    cout << "-- NO SOLUTION --" << endl << endl;

  // Display each intermediate puzzle state and operation leading to the goal state
  for (int i = 0; i < result.size(); ++i)
  {
    // Output the blank square operation performed to achieve the current state
    switch (result[i].move)
    {
      case 1:
        cout << "-- " << i << ": MOVE UP -----" << endl;
        break;
      case 2:
        cout << "-- " << i << ": MOVE DOWN ---" << endl;
        break;
      case 3:
        cout << "-- " << i << ": MOVE LEFT ---" << endl;
        break;
      case 4:
        cout << "-- " << i << ": MOVE RIGHT --" << endl;
        break;
      default:
        cout << "------ START ------" << endl;
        break;
    }

    // Output the properly formatted puzzle state resulting from the operation
    for (int j = 0; j < len; ++j)
    {
      cout << result[i].state[j];
      paddingSize = defaultPadding.length() - to_string(result[i].state[j]).length() + 1;
      currentPadding = string(paddingSize, ' ');
      cout << currentPadding;

      if (j % dim == dim - 1)
        cout << endl;
    }
    cout << endl;
  }

  cout << "*****************************************" << endl << endl;
}
