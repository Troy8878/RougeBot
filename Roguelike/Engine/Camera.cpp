/*********************************
 * Camera.cpp
 * Connor Hilarides
 * Created 2014/09/06
 *********************************/

#include "Common.h"
#include "Camera.h"

// ----------------------------------------------------------------------------

void HUDCamera::LoadFromData(const component_factory_data& data)
{
  auto jpos = json::value::parse(map_fetch(data, "position", "[0,0,0,1]"));
  auto jsize = json::value::parse(map_fetch(data, "size", "[1,1]"));

  position = math::Vector::VectorFromJson(jpos);
  size = math::Vector::VectorFromJson(jsize).get();
}

// ----------------------------------------------------------------------------
