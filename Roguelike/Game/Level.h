/*********************************
* Level.h
* Avi Whitten-Vile
* Created 2014/09/08
*********************************/

#include "common.h"

#define LEVEL_SIZE 75
#define CHUNK_SIZE 25

int LevelNum = 0;

class Chunk
{
public:
  Chunk();
  ~Chunk();

  void CreateChunk();
  void ResetChunk();
  short ChunkTiles[CHUNK_SIZE][CHUNK_SIZE];
};


class Level
{
public:

  Level();
  ~Level();

  void GenerateLevel();
  void PrintLevel();

private:

  short Tiles[LEVEL_SIZE][LEVEL_SIZE];
  int LevelID;

};


