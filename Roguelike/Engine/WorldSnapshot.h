/*********************************
 * WorldSnapshot.h
 * Jake Robsahm
 * Created 2014/11/09
 * Copyright © 2014 DigiPen Institute of Technology, All Rights Reserved
 *********************************/

#pragma once

#include "AIBehaviour.h"

class WorldSnapshot
{
public:
  struct Tile
  {
    // Enum for different types of Actors that can be stored.
    enum ActorType
    {
      Empty,
      Player,
      Enemy,
      Neutral,
      Destructable
    };

    // Coordinates of Tile
    mrb_int x, y;
    // The type of tile
    mrb_int type_id;
    // Store the type of Actor.
    ActorType actor;
    // Various checks.
    bool has_projectile;
    bool has_item;
    bool is_solid;
  };

  const Tile &GetTile(mrb_int x, mrb_int y);

  bool CanMove(mrb_int ox, mrb_int oy, mrb_int dx, mrb_int dy);

private:
  Tile *map;

  mrb_int width;
  mrb_int height;
};
