/*********************************
 * ResourcePack.cpp
 * Connor Hilarides
 * Created 2014/08/08
 *********************************/

#include "ResourcePack.h"

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

struct ResContainerHeader
{
  unsigned __int16 resource_count;
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

struct ResPackImpl
{
  ResPackImpl(const fs::wpath& path);
};

// ----------------------------------------------------------------------------

ResourcePack::ResourcePack(const fs::wpath& path)
  : impl(std::make_shared<ResPackImpl>(path))
{
}

// ----------------------------------------------------------------------------

