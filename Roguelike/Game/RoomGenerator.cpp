/*********************************
* RoomGenerator.cpp
* Connor Hilarides
* Created 2014/10/26
*********************************/

#include "Common.h"
#include "Floor.h"

// ----------------------------------------------------------------------------

static std::random_device rng;
static mrb_int rand_int(mrb_int min, mrb_int max)
{
  std::uniform_int_distribution<mrb_int> dist(min, max);
  return dist(rng);
}

// ----------------------------------------------------------------------------

mrb_value RoomGenerator::Generate(mrb_state *mrb, mrb_value options)
{
  static auto rterr = mrb_class_get(mrb, "RuntimeError");
  static auto width_sym = mrb_symbol_value(mrb_intern_lit(mrb, "width"));
  static auto height_sym = mrb_symbol_value(mrb_intern_lit(mrb, "height"));
  static auto width_range_sym = mrb_symbol_value(mrb_intern_lit(mrb, "width"));
  static auto height_range_sym = mrb_symbol_value(mrb_intern_lit(mrb, "height"));

  if (!mrb_hash_p(options))
    mrb_raise(mrb, rterr, "Generator must be given Hash of options");

  #pragma region Width and Height

  // Get the width
  auto width_v = mrb_hash_get(mrb, options, width_sym);
  if (!mrb_fixnum_p(width_v))
    mrb_raise(mrb, rterr, "Option :width must be specified and a Fixnum");
  Width = mrb_fixnum(width_v);

  // Get the height
  auto height_v = mrb_hash_get(mrb, options, height_sym);
  if (!mrb_fixnum_p(height_v))
    mrb_raise(mrb, rterr, "Option :height must be specified and a Fixnum");
  Height = mrb_fixnum(height_v);

  if (Width < 10 || Height < 10)
    mrb_raise(mrb, rterr, "Width and Height must each be greater than 10");

  #pragma endregion
  
  #pragma region Room Size Range

  // Get the range for room widths (or default it)
  auto width_range = mrb_hash_get(mrb, options, width_range_sym);
  if (mrb_array_p(width_range) && mrb_ary_len(mrb, width_range) == 2)
  {
    auto min_v = mrb_ary_entry(width_range, 0);
    auto max_v = mrb_ary_entry(width_range, 1);
    if (!mrb_fixnum_p(min_v) || !mrb_fixnum_p(max_v))
      mrb_raise(mrb, rterr, "Room size range must be fixnums");

    WidthMin = mrb_fixnum(min_v);
    WidthMax = mrb_fixnum(max_v);
  }
  else
  {
    WidthMin = (mrb_int) (log(Width) * 2);
    WidthMax = (mrb_int) (log2(Width) * 2);
  }
  
  // Get the range for room heights (or default it)
  auto height_range = mrb_hash_get(mrb, options, height_range_sym);
  if (mrb_array_p(height_range) && mrb_ary_len(mrb, height_range) == 2)
  {
    auto min_v = mrb_ary_entry(height_range, 0);
    auto max_v = mrb_ary_entry(height_range, 1);
    if (!mrb_fixnum_p(min_v) || !mrb_fixnum_p(max_v))
      mrb_raise(mrb, rterr, "Room size range must be fixnums");

    HeightMin = mrb_fixnum(min_v);
    HeightMax = mrb_fixnum(max_v);
  }
  else
  {
    HeightMin = (mrb_int) (log(Height) * 2);
    HeightMax = (mrb_int) (log2(Height) * 2);
  }

  #pragma endregion
  
  #pragma region Create Map

  // I can't afford to raise any more exceptions after here so all params are optional
  MapTiles = size_t(Width * Height);
  Map = new mrb_int[MapTiles];
  for (auto& tile : array_iterator(Map, MapTiles))
  {
    // Start by filling in the map
    tile = 1;
  }

  MakeMap();

  #pragma endregion

  #pragma region Convert map into ruby terms and return it

  mrb_value map = mrb_ary_new(mrb);
  for (mrb_int y = 0; y < Height; ++y)
  {
    mrb_value row = mrb_ary_new(mrb);
    for (mrb_int x = 0; x < Width; ++x)
    {
      auto value = mrb_fixnum_value(TileAt(x, y));
      mrb_ary_push(mrb, row, value);
    }
    mrb_ary_push(mrb, map, row);
  }

  mrb_value return_params = mrb_ary_new(mrb);

  mrb_ary_push(mrb, return_params, map);
  mrb_ary_push(mrb, return_params, mrb_fixnum_value(PlayerX));
  mrb_ary_push(mrb, return_params, mrb_fixnum_value(PlayerY));

  return return_params;

  #pragma endregion
}

// ----------------------------------------------------------------------------

void RoomGenerator::MakeMap()
{
  size_t max_height = size_t(Height);
  size_t max_start = max_height - size_t(HeightMin);
  size_t y = 0;

  std::vector<size_t> row_sizes;

  while (y <= max_start)
  {
    size_t available = max_height - y - 1;
    size_t room_height = std::min(RandHeight, available);

    row_sizes.push_back(room_height);

    GEN_RECT row_rect = { 0, y, size_t(Width), y + room_height };
    row_rect.left = 0;
    row_rect.right = size_t(Width);
    row_rect.top = y;
    row_rect.bottom = y + room_height;

    MakeRow(row_rect);

    y += room_height;
  }

  MakeVertHalls();
  
  RandomizeItems();
  RandomizePlayer();
}

// ----------------------------------------------------------------------------

void RoomGenerator::MakeRow(const GEN_RECT rect)
{
  size_t max_width = rect.right - rect.left;
  size_t max_start = max_width - size_t(WidthMin);
  size_t x = 0;

  while (x <= max_start)
  {
    size_t available = max_width - x - 1;
    size_t room_width = std::min(RandWidth, available);

    GEN_RECT col_rect = rect;
    col_rect.left = x;
    col_rect.right = x + room_width;

    MakeRoom(col_rect);

    x += room_width;
  }

  MakeHorizHalls(rect);
}

// ----------------------------------------------------------------------------

void RoomGenerator::MakeRoom(GEN_RECT rect)
{
  // For now, just carve out an inner area
  for (size_t y = rect.top + 2; y < rect.bottom - 2; ++y)
  {
    for (size_t x = rect.left + 2; x < rect.right - 2; ++x)
    {
      TileAt(x, y) = 0;
    }
  }
}

// ----------------------------------------------------------------------------

void RoomGenerator::MakeHorizHalls(GEN_RECT rect)
{
  size_t pos = rand_int(rect.top + 1, rect.bottom - 2);
  if (!MakeHorizHall(pos))
  {
    MakeHorizHall((rect.top + rect.bottom) / 2);
  }
}

// ----------------------------------------------------------------------------

void RoomGenerator::MakeVertHalls()
{
  size_t max_width = size_t(WidthMax);
  size_t min_rooms = size_t(Width) / max_width;

  for (size_t i = 0; i < min_rooms; ++i)
  {
    size_t base_pos = i * max_width;
    size_t offset = (size_t) rand_int(0, max_width - 1);

    MakeVertHall(base_pos + offset);
  }
}

// ----------------------------------------------------------------------------

bool RoomGenerator::MakeHorizHall(size_t y)
{
  size_t width = size_t(Width);

  size_t left = 0;
  while (left < width && TileAt(left, y) == 1)
    left++;
  if (left == width)
    return false;

  size_t right = width - 1;
  while (right > 0 && TileAt(right, y) == 1)
    right--;
  if (right == 0)
    return false;

  // Carve out the hallway. May add some variations later on
  for (size_t x = left; x <= right; ++x)
  {
    TileAt(x, y) = 0;
  }

  return true;
}

// ----------------------------------------------------------------------------

bool RoomGenerator::MakeVertHall(size_t x)
{
  size_t height = size_t(Height);

  size_t top = 0;
  while (top < height && TileAt(x, top) == 1)
    top++;
  if (top == height)
    return false;

  size_t bottom = height - 1;
  while (bottom > 0 && TileAt(x, bottom) == 1)
    bottom--;
  if (bottom == 0)
    return false;

  // Carve out the hallway
  for (size_t y = top; y <= bottom; ++y)
  {
    TileAt(x, y) = 0;
  }

  return true;
}

// ----------------------------------------------------------------------------

void RoomGenerator::RandomizePlayer()
{
  const size_t max_jumps = 200;
  size_t i = 0;

  do
  {
    PlayerX = rand_int(0, Width - 1);
    PlayerY = rand_int(0, Height - 1);

  } while (TileAt(PlayerX, Height - 1 - PlayerY) != 0 && ++i < max_jumps);

  // Find a position manually if jumping failed
  if (i == max_jumps)
  {
    size_t i;
    for (i = 0; i < MapTiles; ++i)
    {
      if (Map[i] == 0)
      {
        PlayerX = i % Width;
        PlayerY = i / Width;
        break;
      }
    }

    // Dafuq, must be a broken map;
    if (i == MapTiles)
      MapBroken = true;
  }
}

// ----------------------------------------------------------------------------

void RoomGenerator::RandomizeItems()
{
  // TODO
}

// ----------------------------------------------------------------------------

size_t RoomGenerator::_RandWidth()
{
  return (size_t) rand_int(WidthMin, WidthMax);
}

// ----------------------------------------------------------------------------

size_t RoomGenerator::_RandHeight()
{
  return (size_t) rand_int(HeightMin, HeightMax);
}

// ----------------------------------------------------------------------------
