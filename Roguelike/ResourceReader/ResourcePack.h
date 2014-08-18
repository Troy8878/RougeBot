/*********************************
 * ResourcePack.h
 * Connor Hilarides
 * Created 2014/08/08
 *********************************/

#pragma once

#include "Helpers\FileSystem.h"
#include "TempFile.h"
#include <string>
#include <chrono>

// ----------------------------------------------------------------------------

__interface Resource
{
  void Release();

  size_t getSize() = 0;
  TempFile getTempFile() = 0;
  shared_array<byte> getData() = 0;
  std::chrono::system_clock getModified() = 0;
};

// ----------------------------------------------------------------------------

__interface ResourceContainer
{
  void Release();

  UINT getResourceCount();
  const char *getContainerName();
  Resource *getResource(const std::string& resource);
};

// ----------------------------------------------------------------------------

struct ResPackImpl;

class ResourcePack
{
public:
  ResourcePack(const fs::wpath& file, const fs::wpath& fallbackFolder);
  
  ResourceContainer *operator[](const std::string& container);

private:
  std::shared_ptr<ResPackImpl> impl;
};

// ----------------------------------------------------------------------------

