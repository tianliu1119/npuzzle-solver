#ifndef PUZZLES_H
#define PUZZLES_H
#include <vector>

namespace DefaultPuzzle
{
  using namespace std;

  /***************
   ** 8-puzzles **
   ***************/
  vector<int> trivial = {1, 2, 3,
                         4, 5, 6,
                         7, 8, 0};

  vector<int> easy = {1, 2, 0,
                      4, 5, 3,
                      7, 8, 6};

  // DEPTH = 5
  // 1) Euclidean Distance
  //    - nodes expanded: 4
  //    - max queue size: 4
  // 2) Manhattan Distance
  //    - nodes expanded: 4
  //    - max queue size: 4
  // 3) Manhattan Distance + Linear Conflict
  //    - nodes expanded: 4
  //    - max queue size: 4
  vector<int> doable = {0, 1, 2,
                        4, 5, 3,
                        7, 8, 6};

  // DEPTH = 23
  // 1) Euclidean Distance
  //    - nodes expanded: 1027
  //    - max queue size: 576
  // 2) Manhattan Distance
  //    - nodes expanded: 330
  //    - max queue size: 212
  // 3) Manhattan Distance + Linear Conflict
  //    - nodes expanded: 303
  //    - max queue size: 191
  vector<int> ohBoy = {8, 7, 1,
                       6, 0, 2,
                       5, 4, 3};

  // DEPTH = 32
  // 1) Euclidean Distance
  //    - nodes expanded: 37614
  //    - max queue size: 15613
  // 2) Manhattan Distance
  //    - nodes expanded: 6862
  //    - max queue size: 3634
  // 3) Manhattan Distance + Linear Conflict
  //    - nodes expanded: 3645
  //    - max queue size: 2000
  vector<int> waitForIt = {8, 6, 7,
                           2, 5, 4,
                           3, 0, 1};

  vector<int> impossible = {1, 2, 3,
                            4, 5, 6,
                            8, 7, 0};

  /****************
   ** 15-puzzles **
   ****************/
  namespace Fifteen
  {
    vector<int> trivial = {1,  2,  3,  4,
                           5,  6,  7,  8,
                           9,  10, 11, 12,
                           13, 14, 15, 0};

    vector<int> easy = {1,  2,  3,  0,
                        5,  6,  7,  4,
                        9,  10, 11, 8,
                        13, 14, 15, 12};

    // DEPTH = 10
    // 1) Euclidean Distance
    //    - nodes expanded: 9
    //    - max queue size: 14
    // 2) Manhattan Distance
    //    - nodes expanded: 9
    //    - max queue size: 14
    // 3) Manhattan Distance + Linear Conflict
    //    - nodes expanded: 9
    //    - max queue size: 14
    vector<int> doable = {2,  0,  3,  4,
                          1,  10, 6,  8,
                          5,  9,  7,  11,
                          13, 14, 15, 12};

    // DEPTH: 36
    // 1) Euclidean Distance
    //    - nodes expanded: 117205
    //    - max queue size: 108971
    // 2) Manhattan Distance
    //    - nodes expanded: 25369
    //    - max queue size: 22861
    // 3) Manhattan Distance + Linear Conflict
    //    - nodes expanded: 3615
    //    - max queue size: 3642
    vector<int> waitForIt = {1,  10, 15, 4,
                             13, 6,  3,  8,
                             2,  9,  12, 7,
                             14, 5,  0,  11};

    vector<int> impossible = {1,  2,  3,  4,
                              5,  6,  7,  8,
                              9,  10, 11, 12,
                              13, 15, 14, 0};
  }
}

#endif // PUZZLES_H
