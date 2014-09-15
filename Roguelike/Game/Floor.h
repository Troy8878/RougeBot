/*********************************
* Floor.h
* Avi Whitten-Vile
* Created 2014/09/08
*********************************/

#pragma once

#include <vector>

/*class Grid
{
short ID;

};*/

class Floor
{
public:

  int Width = 50;
  int Height = 50;
  int AliveChance = 39;
  int BirthLim = 4;
  int DeathLim = 3;
  int Steps = 2;
  int PlayerX = 1;
  int PlayerY = 1;

  void InitFloor(void);
  int CountNeighbors(int x, int y);
  void DoStep();
  void ChoosePlayerStart();
  void GenerateFloor();
  void PrintFloor(void);

  
  std::vector<std::vector<short>> OldMap;
  std::vector<std::vector<short>> Map;
};