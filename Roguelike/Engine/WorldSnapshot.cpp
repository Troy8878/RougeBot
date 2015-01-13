/*********************************
 * WorldSnapshot.cpp
 * Claire Robsahm
 * Created 2014/11/09
 * Copyright © 2014 DigiPen Institute of Technology, All Rights Reserved
 *********************************/

#include <cmath>

#include "Common.h"
#include "WorldSnapshot.h"

// ----------------------------------------------------------------------------

static WorldSnapshot::Tile::ActorType GetActorType(mrb_state *mrb, mrb_value actor)
{
  if (mrb_nil_p(actor))
    return WorldSnapshot::Tile::Empty;

  Entity *actorEntity = ruby::data_get<Entity>(mrb, actor);

  auto &actorHostility = actorEntity->Metadata["hostility"];
  static struct
  {
    const char *name; 
    WorldSnapshot::Tile::ActorType stat;
  } const hostilities[] =
  {
    {"player", WorldSnapshot::Tile::Player},
    {"friendly", WorldSnapshot::Tile::Friendly},
    {"neutral", WorldSnapshot::Tile::Neutral},
    {"enemy", WorldSnapshot::Tile::Enemy}
  };

  for (auto &hostility : hostilities)
  {
    if (actorHostility == hostility.name)
      return hostility.stat;
  }
  
  return WorldSnapshot::Tile::Neutral;
}

// ----------------------------------------------------------------------------

WorldSnapshot::WorldSnapshot()
{
  mrb_state *mrb = *mrb_inst;
  mrb_value floor = mrb_funcall_argv(mrb, mrb_obj_value(mrb->top_self), 
                                     mrb_intern_lit(mrb, "current_floor"), 0, nullptr);

  mrb_value rows = mrb_convert_type(mrb, floor, MRB_TT_ARRAY, "Array", "to_a");
  height = mrb_ary_len(mrb, rows);
  width = mrb_ary_len(
    mrb, 
    mrb_convert_type(mrb, mrb_ary_entry(rows, 0), 
    MRB_TT_ARRAY, "Array", "to_a"));

  map = new Tile[width * height];

  // Ruby funcalls
  mrb_sym solid_p = mrb_intern_lit(mrb, "solid?");
  mrb_sym item_p = mrb_intern_lit(mrb, "item?");
  mrb_sym actor_p = mrb_intern_lit(mrb, "actor?");
  mrb_sym type_id = mrb_intern_lit(mrb, "type_id");
  mrb_sym get_actor = mrb_intern_lit(mrb, "actor");

  mrb_int y = 0;
  for (auto row : ruby::array_each(mrb, rows))
  {
    mrb_int x = 0;
    row = mrb_convert_type(mrb, row, MRB_TT_ARRAY, "Array", "to_a");

    for (auto mrbtile : ruby::array_each(mrb, row))
    {
      auto &tile = GetTile(x, y);

      tile.x = x;
      tile.y = y;
      tile.isSolid = mrb_bool(mrb_funcall_argv(mrb, mrbtile, solid_p, 0, nullptr));
      tile.hasItem = mrb_bool(mrb_funcall_argv(mrb, mrbtile, item_p, 0, nullptr));
      tile.type_id = mrb_fixnum(mrb_funcall_argv(mrb, mrbtile, type_id, 0, nullptr));
      tile.hasProjectile = false; // TODO: Come up with Projectile check

      if (mrb_bool(mrb_funcall_argv(mrb, mrbtile, actor_p, 0, nullptr)))
      {
        mrb_value actor = mrb_funcall_argv(mrb, mrbtile, get_actor, 0, nullptr);
        tile.actor = GetActorType(mrb, actor);
      }
      else
      {
        tile.actor = Tile::Empty;
      }

      x++;
    }

    y++;
  }
}

// ----------------------------------------------------------------------------

WorldSnapshot::Tile& WorldSnapshot::GetTile(mrb_int x, mrb_int y)
{
  if (y >= height || y < 0)
    throw basic_exception("Y value out of range");

  if (x >= width || x < 0)
    throw basic_exception("X value out of range");

  return map[(y * width) + x];
}

const WorldSnapshot::Tile& WorldSnapshot::GetTile(mrb_int x, mrb_int y) const
{
  if (y >= height || y < 0)
    throw basic_exception("Y value out of range");

  if (x >= width || x < 0)
    throw basic_exception("X value out of range");

  return map[(y * width) + x];
}

// ----------------------------------------------------------------------------

WorldSnapshot::BlockedReason WorldSnapshot::CanMove(mrb_int ox, mrb_int oy, mrb_int dx, mrb_int dy) const
{
  // Making sure you aren't trying to move more than one block at a time.
  if (abs(dx) > 1.5 || abs(dy) > 1.5)
  {
    return BlockedByGreatDistance;
  }

  // Make sure the player isn't trying to move out of bounds.
  if (ox + dx < 0 || ox + dx >= width)
    return OutOfBounds;
  if (oy + dy < 0 || oy + dy >= height)
    return OutOfBounds;

  // Get the tile they are trying to move to.
  auto &tile = GetTile(ox + dx, oy + dy);

  // Make sure the player isn't trying to move into another Actor.
  if (tile.actor != Tile::Empty)
    return BlockedByActor;

  // Make sure the player isn't trying to move into a wall.
  if (tile.isSolid)
    return BlockedByWall;

  // All good. The player can move!
  return NotBlocked;
}
