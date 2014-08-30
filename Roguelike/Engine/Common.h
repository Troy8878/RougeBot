/*********************************
 * Common.h
 * Connor Hilarides
 * Created 2014/08/07
 *
 * Precompiled header file for 
 * common resources
 *********************************/

#pragma once

#pragma warning (disable: 4505) // It's okay if a function hasn't been used yet :/

#include <string>
#include <unordered_map>
#include <vector>
#include <stack>
#include <functional>
#include <typeindex>
#include <chrono>
#include <memory>
#include <unordered_map>
#include <random>
#include <unordered_map>

struct component_factory_data : public std::unordered_map<std::string, std::string>
{
  typedef std::unordered_map<std::string, std::string> base;

  component_factory_data() = default;
  component_factory_data(const std::initializer_list<value_type>& list)
    : base(list)
  {
  }
};

#include "Helpers\UseDirectX.h"
#include "Helpers\FlatMap.h"
#include "Helpers\Console.h"
#include "Helpers\StackTrace.h"
#include "Helpers\CriticalSections.h"
#include "Helpers\Allocator.h"

#include "GameTime.h"
#include "Event.h"
#include "EventTypes.h"
#include "Entity.h"
#include "Component.h"
#include "Game.h"

