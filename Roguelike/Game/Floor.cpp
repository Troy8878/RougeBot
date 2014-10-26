/*********************************
* Floor.cpp
* Avi Whitten-Vile
* Created 2014/09/08
*********************************/

#include "Common.h"
#include "Floor.h"
#include "mruby/array.h"

// 1 = WALL

static std::random_device RNG;

#define MRB_HASH_GET(name) mrb_hash_get(mrb, options, mrb_symbol_value(mrb_intern_lit(mrb, name)))

#pragma region Old Generator

std::uniform_int_distribution<int> cave_random(1, 100);

// ----------------------------------------------------------------------------

void CaveGenerator::InitFloor(void)
{
  Fills = 1;
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

      if (cave_random(RNG) < AliveChance)
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

// ----------------------------------------------------------------------------

void CaveGenerator::PrintFloor(void)
{
  for (int x = 0; x < Width; ++x)
  {
    for (int y = 0; y < Height; ++y)
    {
      if (Map[x][y] == 1)
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

// ----------------------------------------------------------------------------

void CaveGenerator::PrintFlood(void)
{
  for (int x = 0; x < Width; ++x)
  {
    for (int y = 0; y < Height; ++y)
    {
      std::cout << Map[x][y];
    }

    std::cout << std::endl;
  }

  std::cout << std::endl;
  std::cout << std::endl;
  std::cout << std::endl;
  std::cout << std::endl;
}

// ----------------------------------------------------------------------------

int CaveGenerator::CountNeighbors(int x, int y)
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

// ----------------------------------------------------------------------------

void CaveGenerator::DoStep()
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

// ----------------------------------------------------------------------------

void CaveGenerator::FloodFill(int x, int y, int target, short replacement)
{
  if (x >= Width && y >= Height && y < 0 && x < 0)
  {
    return;
  }
  // 1. If target - num is equal to replacement - num, return.
  if (Map[x][y] == replacement)
  {
    return;
  }
  // 2. If the num of node is not equal to target - num, return.
  if (Map[x][y] != target)
  {
    return;
  }
  // 3. Set the num of node to replacement - num.
  Map[x][y] = replacement;
  FloodSteps++;
  // 4. Perform Flood - fill(one step to the west of node, target - num, replacement - num).
  if (x > 0)
    FloodFill(x - 1, y, 0, Fills);
  // Perform Flood - fill(one step to the east of node, target - num, replacement - num).
  if (x < Width - 1)
    FloodFill(x + 1, y, 0, Fills);
  // Perform Flood - fill(one step to the north of node, target - num, replacement - num).
  if (y > 0)
    FloodFill(x, y - 1, 0, Fills);
  // Perform Flood - fill(one step to the south of node, target - num, replacement - num).
  if (y < Height - 1)
    FloodFill(x, y + 1, 0, Fills);
  // 5. Return.

  return;
}

// ----------------------------------------------------------------------------

void CaveGenerator::RemoveAll(int target)
{
  for (int i = 0; i < Width; ++i)
  {
    for (int j = 0; j < Height; ++j)
    {
      if (Map[i][j] == target)
      {
        Map[i][j] = 1;
      }
    }
  }
}

// ----------------------------------------------------------------------------

void CaveGenerator::CarveFloor(void)
{
  for (int i = 0; i < Width; ++i)
  {
    for (int j = 0; j < Height; ++j)
    {
      if (Map[i][j] == 0)
      {
        FloodSteps = 0;
        Fills++;
        FloodFill(i, j, 0, Fills);
      }
    }
  }

  short RoomSize;

  for (int r = Fills; r > 1; --r)
  {
    RoomSize = 0;
    for (int i = 0; i < Width; ++i)
    {
      for (int j = 0; j < Height; ++j)
      {
        if (Map[i][j] == r)
        {
          RoomSize++;
        }

      }
    }

    if (RoomSize < MinRoomSize)
    {
      RemoveAll(r);
    }
  }

  for (int x = 0; x < Width; ++x)
  {
    for (int y = 0; y < Height; ++y)
    {
      if (Map[x][y] != 1)
        Map[x][y] = 0;
    }
  }

}

// ----------------------------------------------------------------------------

void CaveGenerator::PlaceItem(void)
{
  for (int x = 0; x < Width; ++x)
  {
    for (int y = 0; y < Height; ++y)
    {
      if (Map[x][y] != 1)
      {
        int nbs = CountNeighbors(x, y);
        if (nbs >= ItemLimit)
        {
          Map[x][y] = 8;
        }
      }
    }
  }
}

// ----------------------------------------------------------------------------

void CaveGenerator::ChoosePlayerStart(void)
{ 
  std::uniform_int_distribution<int> randomx(0, Width - 1); 
  std::uniform_int_distribution<int> randomy(0, Height - 1); 

  do
  {
    PlayerX = randomx(RNG);
    PlayerY = randomy(RNG);

  } while (Map[PlayerX][Height - 1 - PlayerY]);
}

// ----------------------------------------------------------------------------

void CaveGenerator::GenerateFloor(void)
{

  for (int i = 0; i < Steps; ++i)
  {
    DoStep();
  }

  CarveFloor();
  ChoosePlayerStart();
  //PlaceItem();

}

// ----------------------------------------------------------------------------

#pragma endregion

static mrb_value mrb_floor_generate(mrb_state *mrb, mrb_value)
{
  mrb_value options;
  mrb_get_args(mrb, "H", &options);
  
  return RoomGenerator().Generate(mrb, options);
}

// ----------------------------------------------------------------------------

extern "C" void mrb_mruby_floor_init(mrb_state *mrb)
{
  auto module = mrb_define_module(mrb, "Floor");

  mrb_define_module_function(mrb, module, "generate", mrb_floor_generate, ARGS_OPT(1));

}
