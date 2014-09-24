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

  int PlayerX = 1;
  int PlayerY = 1;

  int AliveChance = 40;
  int BirthLim = 4;
  int DeathLim = 3;     // Higher = bigger rooms
  int Steps = 2;


  short Fills = 1;
  short FloodSteps = 0;

  int MinRoomSize = 350;  // Minimum amount of tiles in a room

  int ItemLimit = 4;

  void InitFloor(void);
  int CountNeighbors(int x, int y);
  void DoStep();
  void FloodFill(int x, int y, int target, short replacement);
  void CarveFloor();
  void RemoveAll(int target);
  void PlaceItem();
  void ChoosePlayerStart();
  void GenerateFloor();
  void PrintFloor(void);
  void PrintFlood(void);


  std::vector<std::vector<short>> OldMap;
  std::vector<std::vector<short>> Map;
};
