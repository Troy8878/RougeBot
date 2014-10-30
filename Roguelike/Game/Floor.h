/*********************************
* Floor.h
* Avi Whitten-Vile
* Created 2014/09/08
*********************************/

#pragma once

#include "Engine/Common.h"

// The generator pattern is designed for ruby interfaces.
__interface Generator
{
  mrb_value Generate(mrb_state *mrb, mrb_value options);
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
  
  void VerifyMap();

  size_t _RandWidth();
  size_t _RandHeight();
};

class FloorDoOverException : public basic_exception
{
public:
  FloorDoOverException()
    : basic_exception("YOU BAD MAP GENERATOR! DO IT AGAIN!")
  {
  }
};

