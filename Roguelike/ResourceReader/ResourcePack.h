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

    size_t GetSize();
    byte *GetData();
    TempFile GetTempFile();
    std::chrono::system_clock::time_point GetModified();

    std::istream& GetStream();
    void ResetStream();

    PROPERTY(get = GetSize) size_t Size;
    PROPERTY(get = GetData) byte *Data;
    PROPERTY(get = GetModified) std::chrono::system_clock::time_point Modified;
    PROPERTY(get = GetStream) std::istream& Stream;
  };

// ----------------------------------------------------------------------------

  __interface ResourceContainer
  {
    void Release();

    UINT GetResourceCount();
    const char *GetContainerName();
    Resource *GetResource(const std::string& name);

    PROPERTY(get = GetResourceCount) UINT ResourceCount;
    PROPERTY(get = GetContainerName) const char *ContainerName;
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
