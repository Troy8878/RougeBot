/*********************************
* Level.h
* Avi Whitten-Vile
* Created 2014/09/08
*********************************/

#include "common.h"
#include <cstdlib>


class Level
{
public:

  Level();
  ~Level();

  void GenerateLevel();



private:

  short Tiles[50][50];
};

