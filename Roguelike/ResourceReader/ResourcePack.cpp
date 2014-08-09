/*********************************
 * ResourcePack.cpp
 * Connor Hilarides
 * Created 2014/08/08
 *********************************/

#include "ResourcePack.h"

#pragma pack(push, 1)

// File layout
//  [2 bytes | container_count]
//  [{containers}             ]
//
// Container layout
//  [2 bytes | resource_count]
//  [{resources}             ]
//
// Resource layout
//  [2 bytes  | resource_size]
//  [32 bytes | resource_name]
//  [8 bytes  | updated_at   ]
//  [{resource contents}     ]

struct ResPackHeader
{
  unsigned __int16 container_count;
};

struct ResContainerHeader
{
  unsigned __int16 resource_count;
};

struct ResHeader
{
  unsigned __int16 resource_size;
  char resource_name[32];
  signed __int64 updated_at;
};

#pragma pack(pop)

