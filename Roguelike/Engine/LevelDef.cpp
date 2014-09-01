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
  auto *container = GetGame()->Respack["Levels"];
  RELEASE_AFTER_SCOPE(container);

  auto *resource = container->GetResource(asset);
  RELEASE_AFTER_SCOPE(resource);

  deftree = json::value::parse(resource->Stream);
}

// ----------------------------------------------------------------------------

void LevelDef::Load(Level& level)
{
  auto& leveldata = deftree.as_object();

  level.Name = leveldata["name"].as_string();

  auto entitydefs = leveldata["entities"].as_array_of<json::value::object_t>();
  for (auto& entitydef : entitydefs)
  {
    auto& archetype = entitydef["archetype"].as_string();
    auto data = EntityFactoryDataFromJson(entitydef["components"]);

    auto entity = EntityFactory::CreateEntity(archetype + ".entitydef", data);
    level.RootEntity->AddChild(entity);
  }
}

// ----------------------------------------------------------------------------

entity_factory_data LevelDef::EntityFactoryDataFromJson(json::value jdata)
{
  entity_factory_data data;

  for (auto& pair : jdata.as_object())
  {
    auto& datamap = data[pair.first];

    for (auto& compdata : pair.second.as_object())
    {
      if (compdata.second.is(json::json_type::jstring))
      {
        datamap[compdata.first] = compdata.second.as_string();
      }
      else
      {
        datamap[compdata.first] = compdata.second.serialize();
      }
    }
  }

  return data;
}

// ----------------------------------------------------------------------------

