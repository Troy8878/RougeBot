/*********************************
 * ResourcePack.cpp
 * Connor Hilarides
 * Created 2014/08/08
 *********************************/

#include "ResourcePack.h"
#include "FileMapping.h"
#include <vector>

// ----------------------------------------------------------------------------

#pragma pack(push, 1)

// File layout
//  [2 bytes  | container_count]
//  [{containers}              ]
//
// Container layout
//  [2 bytes  | resource_count]
//  [32 bytes | container_name]
//  [8 bytes  | total_size    ]
//  [{resources}              ]
//
// Resource layout
//  [32 bytes | resource_name ]
//  [8 bytes  | updated_at    ]
//  [4 bytes  | resource_size ]
//  [{resource contents}      ]

struct ResPackHeader
{
  unsigned __int16 container_count;
};

// ----------------------------------------------------------------------------

const size_t firstResContainerOffset = sizeof(ResPackHeader);

struct ResContainerHeader
{
  unsigned __int16 resource_count;
  signed   __int8  conatiner_name[32];
  unsigned __int64 total_size;
};

// ----------------------------------------------------------------------------

struct ResHeader
{
  unsigned __int16 resource_size;
  char resource_name[32];
  signed __int64 updated_at;
};

#pragma pack(pop)

// ----------------------------------------------------------------------------

struct MemContainerMapping
{
  ResContainerHeader header;
  size_t map_offset;
};

struct ResPackImpl
{
  ResPackImpl(const fs::wpath& path, const fs::wpath& fallback);
  ResourceContainer *getContainer(const std::string& name);

  ResourceContainer *getFallbackContainer(const std::wstring& name);
  ResourceContainer *getMemoryContainer(const std::string& name);
  bool memoryContainerExists(const std::string& name);

  FileMapping packmap;
  fs::wpath fallback;

  std::vector<MemContainerMapping> memoryContainers;
};

// ----------------------------------------------------------------------------

ResourcePack::ResourcePack(const fs::wpath& path, const fs::wpath& fallbackFolder)
  : impl(std::make_shared<ResPackImpl>(path, fallbackFolder))
{
}

// ----------------------------------------------------------------------------

ResourceContainer *ResourcePack::operator[](const std::string& containerName)
{
  return impl->getContainer(containerName);
}

// ----------------------------------------------------------------------------

ResPackImpl::ResPackImpl(const fs::wpath& path, const fs::wpath& fallback)
  : packmap(path), fallback(fallback)
{

}

// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------

