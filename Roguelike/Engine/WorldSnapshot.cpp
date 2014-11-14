/*********************************
 * WorldSnapshot.cpp
 * Jake Robsahm
 * Created 2014/11/09
 * Copyright © 2014 DigiPen Institute of Technology, All Rights Reserved
 *********************************/

#include <cmath>

#include "Common.h"
#include "WorldSnapshot.h"

// ----------------------------------------------------------------------------

WorldSnapshot::WorldSnapshot()
{
  mrb_state *mrb = *mrb_inst;
  mrb_value floor = mrb_funcall_argv(mrb, mrb_obj_value(mrb->top_self), 
                                     mrb_intern_lit(mrb, "current_floor"), 0, nullptr);

  mrb_value rows = mrb_convert_type(mrb, floor, MRB_TT_ARRAY, "Array", "to_a");
  height = mrb_ary_len(mrb, rows);
}

// ----------------------------------------------------------------------------

const WorldSnapshot::Tile& WorldSnapshot::GetTile(mrb_int x, mrb_int y)
{
  if (y >= height || y < 0)
    throw basic_exception("Y value out of range");

  if (x >= width || x < 0)
    throw basic_exception("X value out of range");

  return map[(y * width) + x];
}

// ----------------------------------------------------------------------------

WorldSnapshot::BlockedReason WorldSnapshot::CanMove(mrb_int ox, mrb_int oy, mrb_int dx, mrb_int dy)
{
  // Making sure you aren't trying to move more than one block at a time.
  if (std::abs(dx) > 1.5 || std::abs(dy) > 1.5)
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
