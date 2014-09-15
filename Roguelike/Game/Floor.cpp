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

      if (random(RNG) < DeathChance)
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

void Floor::ChoosePlayerStart(void)
{
  std::uniform_int_distribution<int> randomx(1, Width - 1);
  std::uniform_int_distribution<int> randomy(1, Height - 1);

  int times = 0;

  do
  {
    PlayerX = randomx(RNG);
    PlayerY = randomy(RNG);

  } while (Map[PlayerX][Height - PlayerY - 1] != 0 && times++ < 100);
}

void Floor::GenerateFloor(void)
{
  for (int i = 0; i < Steps; ++i)
  {
    DoStep();
  }

  ChoosePlayerStart();
}

#define MRB_HASH_GET(name) mrb_hash_get(mrb, options, mrb_symbol_value(mrb_intern_lit(mrb, name)))

static mrb_value mrb_floor_generate(mrb_state *mrb, mrb_value)
{
  mrb_value options = mrb_nil_value();
  mrb_get_args(mrb, "|H", &options);
  
  if (mrb_nil_p(options))
    options = mrb_hash_new(mrb);

  Floor f;

  mrb_value rwidth = MRB_HASH_GET("width");
  mrb_value rheight = MRB_HASH_GET("height");
  if (mrb_fixnum_p(rwidth))
    f.Width = (int) mrb_fixnum(rwidth);
  if (mrb_fixnum_p(rheight))
    f.Height = (int) mrb_fixnum(rheight);

  mrb_value rdeath_chance = MRB_HASH_GET("death_chance");
  if (mrb_fixnum_p(rdeath_chance))
    f.DeathChance = (int) mrb_fixnum(rdeath_chance);

  mrb_value rbirth = MRB_HASH_GET("birth");
  mrb_value rdeath = MRB_HASH_GET("death");
  if (mrb_fixnum_p(rbirth))
    f.BirthLim = (int) mrb_fixnum(rbirth);
  if (mrb_fixnum_p(rdeath))
    f.DeathLim = (int) mrb_fixnum(rdeath);

  mrb_value rsteps = MRB_HASH_GET("steps");
  if (mrb_fixnum_p(rsteps))
    f.Steps = (int) mrb_fixnum(rsteps);

  f.InitFloor();
  f.GenerateFloor();

  // Push the floor into a 2D ruby array
  mrb_value rf = mrb_ary_new(mrb);
  for (size_t y = 0; y < f.Height; ++y)
  {
    mrb_value row = mrb_ary_new(mrb);
    for (size_t x = 0; x < f.Width; ++x)
    {
      mrb_ary_push(mrb, row, mrb_fixnum_value(f.Map[x][y]));
    }
    mrb_ary_push(mrb, rf, row);
  }

  // Return values
  mrb_value items = mrb_ary_new(mrb);
  mrb_ary_push(mrb, items, rf);
  mrb_ary_push(mrb, items, mrb_fixnum_value(f.PlayerX));
  mrb_ary_push(mrb, items, mrb_fixnum_value(f.PlayerY));

  return items;
}

extern "C" void mrb_mruby_floor_init(mrb_state *mrb)
{
  auto module = mrb_define_module(mrb, "Floor");

  mrb_define_module_function(mrb, module, "generate", mrb_floor_generate, ARGS_OPT(1));

}