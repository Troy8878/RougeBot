/*********************************
* Level.h
* Avi Whitten-Vile
* Created 2014/09/08
*********************************/

#include "common.h"

#define FLOOR_SIZE 75
#define CHUNK_SIZE 25

int FloorNum = 0;

class Chunk
{
public:
  Chunk();
  ~Chunk();

  void CreateChunk();
  void ResetChunk();
  short ChunkTiles[CHUNK_SIZE][CHUNK_SIZE];
};


class Floor
{
public:

  Floor();
  ~Floor();

  void GenerateFloor();
  void PrintFloor();

private:

  short Tiles[FLOOR_SIZE][FLOOR_SIZE];
  int FloorID;

};


