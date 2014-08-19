/*********************************
 * ResourcePackBinaryHeaders.h
 * Connor Hilarides
 * Created 2014/08/18
 *********************************/

#pragma once

#include <chrono>

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
//  [64 bytes | resource_name ]
//  [8 bytes  | updated_at    ]
//  [4 bytes  | resource_size ]
//  [{resource contents}      ]

struct ResPackHeader
{
  unsigned __int64 container_count;
};

// ----------------------------------------------------------------------------

struct ResContainerHeader
{
  unsigned __int32 resource_count;
           __int8  container_name[32];
  unsigned __int64 total_size;
};

// ----------------------------------------------------------------------------

struct ResHeader
{
  typedef std::chrono::system_clock::time_point time_point;

           __int8  resource_name[64];
        time_point updated_at;
  unsigned __int32 resource_size;
};

#pragma pack(pop)

// ----------------------------------------------------------------------------
