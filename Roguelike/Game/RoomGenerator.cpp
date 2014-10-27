/*********************************
* RoomGenerator.cpp
* Connor Hilarides
* Created 2014/10/26
*********************************/

#include "Common.h"
#include "Floor.h"

// ----------------------------------------------------------------------------

#define EMPTY_VALUE 0
#define WALL_VALUE 1
#define UNCHECKED_VALUE -2
#define INVALID_VALUE -1

static std::random_device rng;
static mrb_int rand_int(mrb_int min, mrb_int max)
{
  std::uniform_int_distribution<mrb_int> dist(min, max);
  return dist(rng);
}

// ----------------------------------------------------------------------------

auto RoomGenerator::GEN_RECT::RandomSubrect(double) -> GEN_RECT
{
  size_t height = bottom - top;
  size_t width = right - left;
  size_t rand_width, rand_height;

  rand_width = (size_t) rand_int((mrb_int)(width / 4.0f), (mrb_int)(width / 1.5));
  rand_height = (size_t) rand_int((mrb_int)(height / 4.0f), (mrb_int)(height / 1.5));

  std::uniform_int_distribution<size_t> xdist(0, width - 1 - rand_width);
  std::uniform_int_distribution<size_t> ydist(0, height - 1 - rand_height);

  size_t x = left + xdist(rng);
  size_t y = top + ydist(rng);

  GEN_RECT new_rect;
  new_rect.left = x;
  new_rect.right = x + rand_width;
  new_rect.top = y;
  new_rect.bottom = y + rand_height;

  return new_rect;
}

// ----------------------------------------------------------------------------

template <typename TileAtFunc>
void RoomGenerator::GEN_RECT::CarveMap(TileAtFunc&& TileAt)
{
  for (size_t y = top; y <= bottom; ++y)
  {
    for (size_t x = left; x <= right; ++x)
    {
      TileAt(x, y) = UNCHECKED_VALUE;
    }
  }
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
    WidthMax = (mrb_int) (log2(Width) * 3);
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
    HeightMax = (mrb_int) (log2(Height) * 2.5f);
  }

  #pragma endregion
  
  #pragma region Create Map

  // I can't afford to raise any more exceptions after here so all params are optional
  MapTiles = size_t(Width * Height);
  Map = new mrb_int[MapTiles];
  for (auto& tile : array_iterator(Map, MapTiles))
  {
    // Start by filling in the map
    tile = WALL_VALUE;
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

  delete[] Map;

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
  RandomizePlayer();
  FillUnaccessable();
  RandomizeItems();
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
  GEN_RECT inner_rect = rect;
  inner_rect.left++;
  inner_rect.right--;
  inner_rect.top++;
  inner_rect.bottom--;

  mrb_int num_rects = rand_int(2, 4);
  double area = 0.5 / num_rects;
  for (mrb_int i = 0; i < num_rects; ++i)
  {
    auto subrect = inner_rect.RandomSubrect(area);
    subrect.CarveMap([this](size_t x, size_t y) -> mrb_int&
    {
      return this->TileAt(x, y);
    });
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

  auto check_tile = [this, y](size_t x) -> bool
  {
    return TileAt(x, y) == WALL_VALUE && TileAt(x, y + 1) == WALL_VALUE;
  };

  size_t left = 0;
  while (left < width && check_tile(left) == 1)
    left++;
  if (left == width)
    return false;

  size_t right = width - 1;
  while (right > 0 && check_tile(right) == 1)
    right--;
  if (right == 0)
    return false;

  // Carve out the hallway. May add some variations later on
  for (size_t x = left; x <= right; ++x)
  {
    TileAt(x, y) = UNCHECKED_VALUE;
    TileAt(x, y + 1) = UNCHECKED_VALUE;
  }

  return true;
}

// ----------------------------------------------------------------------------

bool RoomGenerator::MakeVertHall(size_t x)
{
  size_t height = size_t(Height);

  size_t top = 0;
  while (top < height && TileAt(x, top) == WALL_VALUE)
    top++;
  if (top == height)
    return false;

  size_t bottom = height - 1;
  while (bottom > 0 && TileAt(x, bottom) == WALL_VALUE)
    bottom--;
  if (bottom == 0)
    return false;

  // Carve out the hallway
  for (size_t y = top; y <= bottom; ++y)
  {
    TileAt(x, y) = UNCHECKED_VALUE;

    // make random little bubbles
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

  } while (TileAt(PlayerX, Height - 1 - PlayerY) != UNCHECKED_VALUE && ++i < max_jumps);

  // Find a position manually if jumping failed
  if (i == max_jumps)
  {
    size_t i;
    for (i = 0; i < MapTiles; ++i)
    {
      if (Map[i] == UNCHECKED_VALUE)
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

template <typename Func>
static void FloodFill(mrb_int x, mrb_int y, mrb_int oldv, mrb_int newv, Func&& TileAt)
{
  auto& pixel = TileAt(x, y);
  if (pixel == oldv)
  {
    pixel = newv;
    FloodFill(x + 1, y, oldv, newv, TileAt);
    FloodFill(x - 1, y, oldv, newv, TileAt);
    FloodFill(x, y + 1, oldv, newv, TileAt);
    FloodFill(x, y - 1, oldv, newv, TileAt);
  }
}

// ----------------------------------------------------------------------------

void RoomGenerator::FillUnaccessable()
{
  mrb_int deadpixel = INVALID_VALUE;
  FloodFill(PlayerX, Height - 1 - PlayerY, 
            UNCHECKED_VALUE, EMPTY_VALUE,
  [&](mrb_int x, mrb_int y) -> mrb_int&
  {
    if (x < 0 || y < 0 || x >= Width || y >= Height)
      return deadpixel;

    return TileAt(x, y);
  });

  // Purge inaccessible tiles
  for (auto& tile : array_iterator(Map, MapTiles))
  {
    if (tile == UNCHECKED_VALUE)
      tile = WALL_VALUE;
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
