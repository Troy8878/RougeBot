/*********************************
 * Common.h
 * Connor Hilarides
 * Created 2014/08/07
 *
 * Precompiled header file for 
 * common resources
 *********************************/

#pragma once

// It's okay if a function hasn't been used yet :/
#pragma warning (disable: 4505) // http://msdn.microsoft.com/en-us/library/z85eyax0.aspx

// http://msdn.microsoft.com/en-us/library/6t66728h(v=vs.90).aspx
// It's okay, we don't have a braindead compiler and our code only runs on windows
#pragma warning(disable : 4127)

#pragma warning(push)

#include <string>
#include <unordered_map>
#include <vector>
#include <stack>
#include <functional>
#include <typeindex>
#include <chrono>
#include <memory>
#include <random>
#include <limits>
#include <regex>
#include <sstream>
#include <algorithm>
#include <cstdint>
#include <unordered_set>

#include "Helpers\FixedWindows.h"
#include "Helpers\UseDirectX.h"
#include "Helpers\FlatMap.h"
#include "Helpers\Console.h"
#include "Helpers\StackTrace.h"
#include "Helpers\CriticalSections.h"
#include "Helpers\Allocator.h"
#include "Helpers\VisualizePerformance.h"

#include "GameTime.h"
#include "Event.h"
#include "EventTypes.h"
#include "Entity.h"
#include "Component.h"
#include "Game.h"
#include "Texture.h"
//#include "HttpClient.h"

#pragma warning(pop)

