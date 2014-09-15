/*********************************
* Floor.h
* Avi Whitten-Vile
* Created 2014/09/08
*********************************/

#pragma once

#include <vector>

void mrb_mruby_floor_init(mrb_state* mrb);

/*class Grid
{
short ID;

};*/

class Floor
{
public:

  int Width = 50;
  int Height = 50;
  int AliveChance = 45;
  int DeathLim = 4;
  int BirthLim = 4;
  int Steps = 2;

  void InitFloor(void);
  int CountNeighbors(int x, int y);
  void DoStep();
  void GenerateFloor();
  void PrintFloor(void);


  std::vector<std::vector<short>> OldMap;
  std::vector<std::vector<short>> Map;
};