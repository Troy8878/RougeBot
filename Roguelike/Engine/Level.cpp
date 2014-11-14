/*********************************
 * Level.cpp
 * Connor Hilarides
 * Created 2014/07/07
 * Copyright © 2014 DigiPen Institute of Technology, All Rights Reserved
 *********************************/

#include "Common.h"
#include "Level.h"
#include "LevelDef.h"
#include "Helpers\BucketAllocator.h"

// ----------------------------------------------------------------------------

static BucketAllocator levelAllocator{sizeof(Level)};

// ----------------------------------------------------------------------------

Level *Level::CreateLevel(const std::string &def)
{
  auto memory = levelAllocator.Allocate();
  auto level = new(memory) Level;

  mrb_gv_set(*mrb_inst, mrb_intern_lit(*mrb_inst, "GLOBAL_COMP_REGISTER"), mrb_hash_new(*mrb_inst));

  LevelDef definition{def};
  definition.Load(*level);

  return level;
}

// ----------------------------------------------------------------------------

void Level::DestroyLevel(Level *level)
{
  level->~Level();
  levelAllocator.Free(level);
}

// ----------------------------------------------------------------------------

Level::Level()
{
}

// ----------------------------------------------------------------------------

Level::~Level()
{
  EntityFactory::DestroyEntity(RootEntity);
}

// ----------------------------------------------------------------------------