/*********************************
* Floor.cpp
* Avi Whitten-Vile
* Created 2014/09/08
*********************************/

#include "Common.h"
#include "Floor.h"

// 1 = WALL

static std::random_device RNG;

std::uniform_int_distribution<int> random(1, 100);

void Floor::InitFloor(void)
{
  OldMap.clear();
  OldMap.resize(Width);
  Map.clear();
  Map.resize(Width);

  for (int x = 0; x < Width; ++x)
  {
    OldMap[x].resize(Height);
    Map[x].resize(Height);

    for (int y = 0; y < Height; ++y)
    {

      Map[x][y] = 0;

      if (random(RNG) < AliveChance)
      {
        OldMap[x][y] = 1;
      }

      else
      {
        OldMap[x][y] = 0;
      }
    }
  }
}

void Floor::PrintFloor(void)
{
  for (int x = 0; x < Width; ++x)
  {
    for (int y = 0; y < Height; ++y)
    {
      if (OldMap[x][y] == 1)
        std::cout << ".";
      else
        std::cout << "O";
    }

    std::cout << std::endl;
  }

  std::cout << std::endl;
  std::cout << std::endl;
  std::cout << std::endl;
  std::cout << std::endl;
}

int Floor::CountNeighbors(int x, int y)
{

  int alive = 0;

  for (int i = -1; i < 2; ++i)
  {
    for (int j = -1; j < 2; ++j)
    {
      int nb_x = x + i;
      int nb_y = y + j;

      if (i == 0 && j == 0)
        continue;

      else if (nb_x < 0 || nb_x > Width - 1 || nb_y < 0 || nb_y > Height - 1)
        alive++;

      else if (OldMap[nb_x][nb_y])
        alive++;

    }
  }

  return alive;

}

void Floor::DoStep()
{

  for (int x = 0; x < Width; ++x)
  {
    for (int y = 0; y < Height; ++y)
    {
      int nbs = CountNeighbors(x, y);

      if (OldMap[x][y])
      {

        if (nbs < DeathLim)
          Map[x][y] = 0;

        else
          Map[x][y] = 1;
      }

      else
      {
        if (nbs > BirthLim)
          Map[x][y] = 1;

        else
          Map[x][y] = 0;
      }
    }
  }

  for (int x = 0; x < Width; ++x)
  {
    for (int y = 0; y < Height; ++y)
    {
      OldMap[x][y] = Map[x][y];
    }
  }

}

void Floor::GenerateFloor(void)
{
  for (int i = 0; i < Steps; ++i)
  {
    DoStep();
  }
}


void mrb_mruby_floor_init(mrb_state* mrb)
{
  auto module = mrb_inst->define_module("Floor");
}