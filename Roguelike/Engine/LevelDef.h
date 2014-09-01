/*********************************
 * LevelDef.h
 * Connor Hilarides
 * Created 2014/08/31
 *********************************/

#pragma once

#include "Common.h"
#include "Level.h"
#include "json/json.h"

// ----------------------------------------------------------------------------

class LevelDef
{
public:
  LevelDef(const std::string& asset);
  void Load(Level& level);

private:
  json::value deftree;

  static entity_factory_data EntityFactoryDataFromJson(json::value jdata);
};

// ----------------------------------------------------------------------------


