/*********************************
* Level.cpp
* Avi Whitten-Vile
* Created 2014/09/08
*********************************/

#include <stdio.h>      /* printf, scanf, puts, NULL */
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include "Level.h"


#define CHUNKS 9

Chunk::Chunk(void)
{
}

Chunk::~Chunk(void)
{
}

void Chunk::ResetChunk(void)
{
  for (int i = 0; i < LEVEL_SIZE; ++i)
  {
    for (int j = 0; j < LEVEL_SIZE; ++j)
    {
      ChunkTiles[i][j] = 0;
    }
  }
}

void Chunk::CreateChunk(void)
{
  int x, xstart, y, ystart, mid;
  srand(time(NULL));

  mid = (CHUNK_SIZE - 1) / 2;
  x = (rand() % (CHUNK_SIZE) + 1);
  y = (rand() % (CHUNK_SIZE) + 1);

  for (int i = 0; i < y; ++i)
  {
    for (int j = 0; j < x; ++j)
    {
      ChunkTiles[(mid - (x / 2)) + j][(mid - (y / 2)) + i] = 1;
    }
  }
  

}


Level::Level(void)
{
  LevelID = LevelNum++;

  for (int i = 0; i < LEVEL_SIZE; ++i)
  {
    for (int j = 0; j < LEVEL_SIZE; ++j)
    {
      Tiles[i][j] = 0;
    }
  }
}

Level::~Level(void)
{
}

void Level::GenerateLevel(void)
{
  Chunk ch;
  ch.ResetChunk();

  for (int levelx = 0; levelx < LEVEL_SIZE / CHUNK_SIZE; ++levelx)
  {
    for (int levely = 0; levely < LEVEL_SIZE / CHUNK_SIZE; ++levely)
    {
      ch.CreateChunk();
      for (int chunkx = 0; chunkx < CHUNK_SIZE; chunkx++)
      {
        for (int chunky = 0; chunky < CHUNK_SIZE; chunky++)
        {
          Tiles[(CHUNK_SIZE * levelx) + chunkx][(CHUNK_SIZE * levely) + chunky] = ch.ChunkTiles[chunkx][chunky];
        }
      }

    }
  }
}

void Level::PrintLevel(void)
{
  for (int i = 0; i < LEVEL_SIZE; ++i)
  {
    for (int j = 0; j < LEVEL_SIZE; ++j)
    {
      printf("%i", Tiles[i][j]);
    }
  }
}

