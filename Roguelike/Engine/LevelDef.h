/*********************************
 * LevelDef.h
 * Connor Hilarides
 * Created 2014/08/31
 * Copyright © 2014 DigiPen Institute of Technology, All Rights Reserved
 *********************************/

#pragma once

#include "Common.h"
#include "Level.h"
#include "json/json.h"

// ----------------------------------------------------------------------------

class LevelDef
{
public:
  LevelDef(const std::string &asset);
  void Load(Level &level);

  static async_task<Level *> LoadLevelAsync(const std::string &name);

private:
  json::value deftree;

  void LevelDef::PopulateEntityChildren(Entity *parent, json::value::object_t entitydef);
  static entity_factory_data EntityFactoryDataFromJson(json::value jdata);
};

// ----------------------------------------------------------------------------