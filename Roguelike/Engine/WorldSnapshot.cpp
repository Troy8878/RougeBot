/*********************************
 * WorldSnapshot.cpp
 * Jake Robsahm
 * Created 2014/11/09
 * Copyright © 2014 DigiPen Institute of Technology, All Rights Reserved
 *********************************/

#include "Common.h"
#include "WorldSnapshot.h"


const WorldSnapshot::Tile& WorldSnapshot::GetTile(mrb_int x, mrb_int y)
{
  if (y >= height || y < 0)
    throw basic_exception("Y value out of range");

  if (x >= width || x < 0)
    throw basic_exception("X value out of range");

  return map[(y * width) + x];
}

