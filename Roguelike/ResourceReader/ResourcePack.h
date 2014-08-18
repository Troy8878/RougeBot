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

struct ResPackImpl;

namespace Respack
{

// ----------------------------------------------------------------------------

  __interface Resource
  {
    void Release();

    size_t getSize();
    byte *getData();
    TempFile getTempFile();
    std::chrono::system_clock::time_point getModified();
  };

// ----------------------------------------------------------------------------

  __interface ResourceContainer
  {
    void Release();

    UINT getResourceCount();
    const char *getContainerName();
    Resource *getResource(const std::string& name);
  };

// ----------------------------------------------------------------------------

  class ResourcePack
  {
  public:
    ResourcePack(const fs::wpath& file, const fs::wpath& fallbackFolder);
  
    ResourceContainer *operator[](const std::string& container);

  private:
    std::shared_ptr<ResPackImpl> impl;
  };

// ----------------------------------------------------------------------------

}
