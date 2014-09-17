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

  int AliveChance = 40;
  int BirthLim = 4;
  int DeathLim = 3;     // Higher = bigger rooms
  int Steps = 15;


  int Fills = 1;
  int FloodSteps = 0;
  int MinRoomSize = 250;

  int ItemLimit = 4;

  void InitFloor(void);
  int CountNeighbors(int x, int y);
  void DoStep();
  void FloodFill(int x, int y, short target, short replacement);
  void CarveFloor();
  void RemoveAll(short target);
  void PlaceItem();
  void GenerateFloor();
  void PrintFloor(void);
  void PrintFlood(void);


  std::vector<std::vector<short>> OldMap;
  std::vector<std::vector<short>> Map;
};
