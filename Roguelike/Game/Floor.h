/*********************************
* Floor.h
* Avi Whitten-Vile
* Created 2014/09/08
*********************************/

#pragma once

#include <vector>

// The generator pattern is designed for ruby interfaces.
__interface Generator
{
  mrb_value Generate(mrb_state *mrb, mrb_value options);
};

class CaveGenerator
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

class RoomGenerator : public Generator
{
public:
  mrb_value Generate(mrb_state *mrb, mrb_value options) override;

private:
  struct GEN_RECT
  {
    size_t left, top, right, bottom;
    PROPERTY(get = _Area) size_t area;

    GEN_RECT RandomSubrect(double approxarea);

    template <typename TileAtFunc>
    void CarveMap(TileAtFunc&& TileAt);

    size_t _Area() const { return (right - left) * (bottom - top); }
  };

  mrb_int Width;
  mrb_int Height;
  
  mrb_int WidthMin, WidthMax;
  mrb_int HeightMin, HeightMax;
  
  PROPERTY(get = _RandWidth) size_t RandWidth;
  PROPERTY(get = _RandHeight) size_t RandHeight;

  size_t MapTiles;
  mrb_int *Map;
  inline mrb_int& TileAt(size_t x, size_t y)
  {
    return *(Map + (y * Width) + x);
  }

  mrb_int PlayerX, PlayerY;
  mrb_bool MapBroken = false;

  void MakeMap();
  void MakeRow(const GEN_RECT rect);
  void MakeRoom(const GEN_RECT rect);

  void MakeHorizHalls(const GEN_RECT rect);
  void MakeVertHalls();

  bool MakeHorizHall(size_t y);
  bool MakeVertHall(size_t x);

  void FillUnaccessable();

  void RandomizePlayer();
  void RandomizeItems();

  size_t _RandWidth();
  size_t _RandHeight();
};

