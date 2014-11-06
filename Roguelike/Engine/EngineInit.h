/*********************************
 * EngineInit.h
 * Connor Hilarides
 * Created 2014/08/07
 *********************************/

#pragma once

#include "Helpers\FileSystem.h"

// ----------------------------------------------------------------------------

struct EngineInitSettings
{
  fs::wpath assetPack;
  fs::wpath assetFolder;

  bool cullTriangles = false;
  bool vsync = true;
};

// ----------------------------------------------------------------------------