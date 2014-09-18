/*********************************
 * LevelDef.cp
 * Connor Hilarides
 * Created 2014/08/31
 *********************************/

#include "Common.h"
#include "LevelDef.h"

// ----------------------------------------------------------------------------

LevelDef::LevelDef(const std::string& asset)
{
  deftree = ParseJsonAsset("Levels", asset + ".leveldef");
}

// ----------------------------------------------------------------------------

void LevelDef::PopulateEntityChildren(Entity *parent, json::value::object_t entitydef)
{
  static json::value noarch = json::value::string("NoArchetype");
  auto& archetype = map_fetch(entitydef, "archetype", noarch).as_string();
  auto data = EntityFactoryDataFromJson(entitydef["components"]);

  auto entity = EntityFactory::CreateEntity(archetype, data);

  static json::value defname = json::value::string("<UNNAMED>");
  entity->Name = map_fetch(entitydef, "name", defname).as_string();

  parent->AddChild(entity);

  auto childit = entitydef.find("children");
  if (childit != entitydef.end())
  {
    auto children = childit->second.as_array_of<json::value::object_t>();
    for (auto& child : children)
    {
      PopulateEntityChildren(entity, child);
    }
  }
}

void LevelDef::Load(Level& level)
{
  auto& leveldata = deftree.as_object();
  level.Name = leveldata["name"].as_string();
  
  static json::value emptyObject = json::value::object();
  auto data = EntityFactoryDataFromJson(map_fetch(leveldata, "components", emptyObject));
  level.RootEntity = EntityFactory::CreateEntity("NoArchetype", data, 0);

  auto entitydefs = leveldata["entities"].as_array_of<json::value::object_t>();
  for (auto& entitydef : entitydefs)
  {
    PopulateEntityChildren(level.RootEntity, entitydef);
  }

  level.levelEvents.AddListener(level.RootEntity);
}

// ----------------------------------------------------------------------------

async_task<Level *> LevelDef::LoadLevelAsync(const std::string& def)
{
  return run_async<Level *>(&Level::CreateLevel, def);
}

// ----------------------------------------------------------------------------

entity_factory_data LevelDef::EntityFactoryDataFromJson(json::value jdata)
{
  entity_factory_data data;

  if (jdata.is(json::json_type::jobject))
  {
    auto& obj = jdata.as_object();
    for (auto& pair : obj)
    {
      auto& datamap = data[pair.first];

      for (auto& compdata : pair.second.as_object())
      {
        datamap[compdata.first] = compdata.second;
      }
    }
  }

  return data;
}

// ----------------------------------------------------------------------------

