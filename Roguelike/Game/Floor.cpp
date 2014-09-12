/*********************************
* Floor.cpp
* Avi Whitten-Vile
* Created 2014/09/08
*********************************/

#include "common.h"
#include <stdio.h>      /* printf, scanf, puts, NULL */
#include <stdlib.h>     /* srand, rand */
#include <random>
#include "Floor.h"

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
  for (int i = 0; i < FLOOR_SIZE; ++i)
  {
    for (int j = 0; j < FLOOR_SIZE; ++j)
    {
      ChunkTiles[i][j] = 0;
    }
  }
}

void Chunk::CreateChunk(void)
{
  int x, y, mid;


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


Floor::Floor(void)
{
  FloorID = FloorNum++;

  for (int i = 0; i < FLOOR_SIZE; ++i)
  {
    for (int j = 0; j < FLOOR_SIZE; ++j)
    {
      Tiles[i][j] = 0;
    }
  }
}

Floor::~Floor(void)
{
}

void Floor::GenerateFloor(void)
{
  Chunk ch;
  ch.ResetChunk();

  for (int Floorx = 0; Floorx < FLOOR_SIZE / CHUNK_SIZE; ++Floorx)
  {
    for (int Floory = 0; Floory < FLOOR_SIZE / CHUNK_SIZE; ++Floory)
    {
      ch.CreateChunk();
      for (int chunkx = 0; chunkx < CHUNK_SIZE; chunkx++)
      {
        for (int chunky = 0; chunky < CHUNK_SIZE; chunky++)
        {
          Tiles[(CHUNK_SIZE * Floorx) + chunkx][(CHUNK_SIZE * Floory) + chunky] = ch.ChunkTiles[chunkx][chunky];
        }
      }

    }
  }
}

void Floor::PrintFloor(void)
{
  for (int i = 0; i < FLOOR_SIZE; ++i)
  {
    for (int j = 0; j < FLOOR_SIZE; ++j)
    {
      std::cout << Tiles[i][j];
    }
  }
}

