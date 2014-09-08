/*********************************
 * Level.cpp
 * Connor Hilarides
 * Created 2014/07/07
 *********************************/

#include "Common.h"
#include "Level.h"
#include "LevelDef.h"
#include "Helpers\BucketAllocator.h"

// ----------------------------------------------------------------------------

static BucketAllocator levelAllocator{sizeof(Level)};

// ----------------------------------------------------------------------------

Level *Level::CreateLevel(const std::string& def)
{
  auto memory = levelAllocator.Allocate();
  auto level = new (memory) Level;

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
