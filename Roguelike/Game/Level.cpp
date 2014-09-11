/*********************************
* Level.cpp
* Avi Whitten-Vile
* Created 2014/09/08
*********************************/

#include <stdio.h>      /* printf, scanf, puts, NULL */
#include <stdlib.h>     /* srand, rand */
#include <random>
#include "Level.h"

static std::random_device RNG;


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


  std::uniform_int_distribution<int> random(0, CHUNK_SIZE);

  //random(RNG);


  mid = (CHUNK_SIZE - 1) / 2;
  x = (random(RNG));
  y = (random(RNG));

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
      std::cout << Tiles[i][j];
    }
  }
}

