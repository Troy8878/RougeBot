#include "Common.h"
#include "Floor.h"

#define var auto
#include <mruby/include/mruby/irep.h>
static std::random_device rng;

static bool TTIsEntity(TileType type)
{
  switch (type)
  {
  case TileType::Enemy:
  case TileType::BorkWall:
  case TileType::ItemSpawn:
    return true;
  default:
    return false;
  }
}

static void two_random_numbers(size_t upper, size_t &x1, size_t &x2)
{
  std::uniform_int_distribution<size_t> random(0, upper);

  x1 = random(rng);
  do
  {
    x2 = random(rng);
  }
  while (x2 == x1);
}

mrb_value PrefabGenerator::Generate(mrb_state *mrb, mrb_value options)
{
  ParseOptions(mrb, options);

  MapTiles = static_cast<size_t>(Width * Height);
  Map = new mrb_int[MapTiles];

  // Clear map
  for (var &tile : array_iterator(Map, MapTiles))
    tile = 0;

  size_t spawnx, spawny;
  size_t stairx, stairy;
  two_random_numbers(3, spawnx, stairx);
  two_random_numbers(3, spawny, stairy);
  MakeSpawn(spawnx, spawny);
  MakeStairs(stairx, stairy);

  MakeBarriers();

  for (size_t y = 0; y < 4; ++y)
  {
    for (size_t x = 0; x < 4; ++x)
    {
      if (x == spawnx && y == spawny ||
        x == stairx && y == stairy)
        continue;

      MakeRoom(x, y);
    }
  }

#pragma region Convert map into ruby terms and return it

  var map = mrb_ary_new(mrb);
  for (mrb_int y = 0; y < Height; ++y)
  {
    var row = mrb_ary_new(mrb);
    for (mrb_int x = 0; x < Width; ++x)
    {
      auto value = mrb_fixnum_value(TileAt(x, y));
      mrb_ary_push(mrb, row, value);
    }
    mrb_ary_push(mrb, map, row);
  }

  var return_params = mrb_ary_new(mrb);
  /* fn generate(opts) -> (
   *   map: [[Fixnum]], 
   *   px: Fixnum,
   *   py: Fixnum,
   *   sx: Fixnum,
   *   sy: Fixnum,
   *   entities: [(
   *     type: Fixnum,
   *     x: Fixnum,
   *     y: Fixnum,
   *     data: enum(nil,Hash,Array,Fixnum,Float,String,Boolean)
   *   )],
   * )
   */

  mrb_ary_push(mrb, return_params, map);
  mrb_ary_push(mrb, return_params, mrb_fixnum_value(PlayerX));
  mrb_ary_push(mrb, return_params, mrb_fixnum_value(PlayerY));
  mrb_ary_push(mrb, return_params, mrb_fixnum_value(StairX));
  mrb_ary_push(mrb, return_params, mrb_fixnum_value(StairY));

  var entities = mrb_ary_new(mrb);
  for (const var &entity : Entities)
  {
    var tuple = mrb_ary_new(mrb);
    {
      mrb_ary_push(mrb, tuple, mrb_fixnum_value(static_cast<mrb_int>(std::get<0>(entity))));
      mrb_ary_push(mrb, tuple, mrb_fixnum_value(static_cast<mrb_int>(std::get<1>(entity))));
      mrb_ary_push(mrb, tuple, mrb_fixnum_value(static_cast<mrb_int>(std::get<2>(entity))));
      mrb_ary_push(mrb, tuple, mrb_inst->json_to_value(std::get<3>(entity)));
    }
    mrb_ary_push(mrb, entities, tuple);
  }
  mrb_ary_push(mrb, return_params, entities);

  delete[] Map;

  return return_params;

#pragma endregion
}

void PrefabGenerator::ParseOptions(mrb_state *mrb, mrb_value options)
{
  static var rterr = mrb_class_get(mrb, "RuntimeError");
  static var width_sym = mrb_symbol_value(mrb_intern_lit(mrb, "width"));
  static var height_sym = mrb_symbol_value(mrb_intern_lit(mrb, "height"));
  static var level_sym = mrb_symbol_value(mrb_intern_lit(mrb, "level"));

  // Get the width
  var width_v = mrb_hash_get(mrb, options, width_sym);
  if (!mrb_fixnum_p(width_v))
    mrb_raise(mrb, rterr, "Option :width must be specified and a Fixnum");
  Width = mrb_fixnum(width_v);

  // Get the height
  var height_v = mrb_hash_get(mrb, options, height_sym);
  if (!mrb_fixnum_p(height_v))
    mrb_raise(mrb, rterr, "Option :height must be specified and a Fixnum");
  Height = mrb_fixnum(height_v);

  var level = mrb_hash_get(mrb, options, level_sym);
  if (mrb_fixnum_p(level))
    Level = mrb_fixnum(level);
}

void PrefabGenerator::MakeSpawn(size_t x, size_t y)
{
  var room = ParseJsonAsset("Prefabs", "Base/SpawnRoom.prefab.json");
  MakeRoom(room, x, y);
}

void PrefabGenerator::MakeStairs(size_t x, size_t y)
{
  var room = ParseJsonAsset("Prefabs", "Base/StairRoom.prefab.json");
  MakeRoom(room, x, y);
}

void PrefabGenerator::MakeRoom(size_t x, size_t y)
{
  var container = GetGame()->Respack["Prefabs"];
  RELEASE_AFTER_SCOPE(container);

  var randlevels = std::vector<std::string>();
  var pattern = std::regex("Random/(.*).prefab.json", std::regex::icase);
  var resources = container->GetResources();

  for (var &res : resources)
  {
    if (regex_match(res, pattern))
    {
      randlevels.push_back(res);
    }
  }

  var rand_level = random_item(randlevels);
  MakeRoom(ParseJsonAsset("Prefabs", rand_level), x, y);
}

void PrefabGenerator::MakeRoom(json::value room, size_t rx, size_t ry)
{
  static mrb_state *mrb = *mrb_inst;
  mrb_full_gc(mrb);
  ruby::ruby_gc_guard{mrb};

  var ctx_sym = mrb_intern_lit(mrb, "$ctx");
  mrb_gv_set(mrb, ctx_sym, MakeContext(rx, ry));

  var tiles = room["tiles"];
  for (size_t y = 0; y < 10; ++y)
  {
    var py = ry * 11 + 1 + y;
    for (size_t x = 0; x < 10; ++x)
    {
      var px = rx * 11 + 1 + x;

      var id = static_cast<int>(tiles[y][x]["id"].as_number());
      var meta = "begin;" + tiles[y][x]["meta"].as_string() + ";rescue Exception => e;e;end";
      var metares = mrb_load_nstring(mrb, meta.c_str(), static_cast<int>(meta.size()));
      var metajson = json::value::null();

      try
      {
        metajson = mrb_inst->value_to_json(metares);
      }
      catch (...)
      {
      }

      var type = static_cast<TileType>(id);
      switch (type)
      {
      case TileType::Floor: 
          TileAt(px, py) = static_cast<mrb_int>(TileType::Floor);
          break;

      case TileType::Wall:
        if (mrb_symbol_p(metares) && strcmp(mrb_sym2name(mrb, mrb_symbol(metares)), "empty") == 0)
          TileAt(px, py) = static_cast<mrb_int>(TileType::Floor);
        else
          TileAt(px, py) = static_cast<mrb_int>(TileType::Wall);
        break;
        
      case TileType::ItemSpawn:
      case TileType::BorkWall:
      case TileType::Enemy:
        Entities.push_back(std::make_tuple(type, px, py, metajson));
        break;

      case TileType::PlayerStart:
        PlayerX = px;
        PlayerY = Height - 1 - py;
        break;

      case TileType::Stairs:
        StairX = px;
        StairY = Height - 1 - py;
        break;
      }
    }
  }
}

void PrefabGenerator::MakeBarriers()
{
  // Left and right walls
  for (var x = 0; x < Width; ++x)
    TileAt(x, 0) = static_cast<mrb_int>(TileType::Wall);
  for (var y = 0; y < Height; ++y)
    TileAt(0, y) = static_cast<mrb_int>(TileType::Wall);

  for (size_t y = 0; y < 4; ++y)
  {
    for (size_t x = 0; x < 4; ++x)
    {
      var startx = x * 11;
      var starty = y * 11;

      for (size_t i = 0; i < 11; ++i)
      {
        if (i == 5 || i == 6)
          continue;

        TileAt(startx + i, starty) = static_cast<mrb_int>(TileType::Wall);
        TileAt(startx, starty + i) = static_cast<mrb_int>(TileType::Wall);
      }
    }
  }

  // Fill the empty space
  var bottomy = 4 * 11;
  for (mrb_int y = bottomy; y < Height; ++y)
  {
    for (mrb_int x = 0; x < Width; ++x)
    {
      TileAt(x, y) = static_cast<mrb_int>(TileType::Wall);
    }
  }

  var rightx = 4 * 11;
  for (mrb_int y = 0; y < Height; ++y)
  {
    for (mrb_int x = rightx; x < Width; ++x)
    {
      TileAt(x, y) = static_cast<mrb_int>(TileType::Wall);
    }
  }
}

mrb_value PrefabGenerator::MakeContext(size_t x, size_t y)
{
  static mrb_state *mrb = *mrb_inst;
  static var door_klass = mrb_class_get(mrb, "PrefabDoorContext");

  var context = mrb_hash_new(mrb);

  mrb_hash_set(mrb, context, mrb_symbol_value(mrb_intern_lit(mrb, "level")), mrb_fixnum_value(Level));

  var door_data = mrb_hash_new(mrb);
  {
    var door_left = mrb_funcall(mrb, mrb_obj_value(door_klass), "new", 1, mrb_bool_value(x == 0));
    var door_right = mrb_funcall(mrb, mrb_obj_value(door_klass), "new", 1, mrb_bool_value(x == 3));
    var door_top = mrb_funcall(mrb, mrb_obj_value(door_klass), "new", 1, mrb_bool_value(y == 0));
    var door_bottom = mrb_funcall(mrb, mrb_obj_value(door_klass), "new", 1, mrb_bool_value(y == 3));
    mrb_hash_set(mrb, door_data, mrb_symbol_value(mrb_intern_lit(mrb, "left")), door_left);
    mrb_hash_set(mrb, door_data, mrb_symbol_value(mrb_intern_lit(mrb, "right")), door_right);
    mrb_hash_set(mrb, door_data, mrb_symbol_value(mrb_intern_lit(mrb, "top")), door_top);
    mrb_hash_set(mrb, door_data, mrb_symbol_value(mrb_intern_lit(mrb, "bottom")), door_bottom);
  }
  mrb_hash_set(mrb, context, mrb_symbol_value(mrb_intern_lit(mrb, "door")), door_data);

  return context;
}
