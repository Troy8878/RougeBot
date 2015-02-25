/*********************************
 * Floor.h
 * Avi Whitten-Vile
 * Created 2014/09/08
 * Copyright © 2014 DigiPen Institute of Technology, All Rights Reserved
 *********************************/

#pragma once

#include "Engine/Common.h"

enum class TileType
{
  Floor = 0,
  Wall = 1,
  Enemy = 2,
  BorkWall = 3,
  PlayerStart = 4,
  ItemSpawn = 5,
  Stairs = 6,
};

// The generator pattern is designed for ruby interfaces.
__interface Generator
{
  mrb_value Generate(mrb_state *mrb, mrb_value options);
};

class RoomGenerator final : public Generator
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
  
  void VerifyMap();

  size_t _RandWidth();
  size_t _RandHeight();
};

class PrefabGenerator final : public Generator
{
public:
  mrb_value Generate(mrb_state *mrb, mrb_value options) override;

private:
  mrb_int Level;

  mrb_int Width, Height;
  mrb_int PlayerX, PlayerY;
  mrb_int StairX, StairY;

  size_t MapTiles;
  mrb_int *Map;

  std::vector<std::tuple<TileType, size_t, size_t, json::value>> Entities;

  inline mrb_int& TileAt(size_t x, size_t y)
  {
    return *(Map + (y * Width) + x);
  }

  void ParseOptions(mrb_state *mrb, mrb_value options);

  void MakeSpawn(size_t x, size_t y);
  void MakeStairs(size_t x, size_t y);
  void MakeRoom(size_t x, size_t y);
  void MakeRoom(json::value room, size_t x, size_t y);

  void MakeBarriers();

  mrb_value MakeContext(size_t x, size_t y);
};

class FloorDoOverException : public basic_exception
{
public:
  FloorDoOverException()
    : basic_exception("YOU BAD MAP GENERATOR! DO IT AGAIN!")
  {
  }
};

