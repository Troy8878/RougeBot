/*********************************
 * FileMapping.h
 * Connor Hilarides
 * Created 2014/08/17
 * Copyright © 2014 DigiPen Institute of Technology, All Rights Reserved
 *********************************/

#pragma once

#include "Helpers\FileSystem.h"

// ----------------------------------------------------------------------------

class FileMappingView;

class FileMapping
{
public:
  FileMapping(const fs::wpath& path, bool read = true, bool write = false);
  ~FileMapping();

  FileMappingView mapView(size_t offset, size_t size);

  template <typename Struct>
  Struct readStruct(size_t offset)
  {
    auto view = mapView(offset, sizeof(Struct));
    return *view.getAs<Struct>();
  }

  NO_COPY_CONSTRUCTOR(FileMapping);
  NO_ASSIGNMENT_OPERATOR(FileMapping);

private:
  HANDLE fileHandle = INVALID_HANDLE_VALUE;
  HANDLE mapHandle = nullptr;
  DWORD viewAccess = 0;
};

// ----------------------------------------------------------------------------

class FileMappingView
{
  FileMappingView(void *address, size_t offset);
  friend class FileMapping;

public:
  FileMappingView() = default;

  template <typename T>
  inline T *getAs()
  {
    return reinterpret_cast<T *>(reinterpret_cast<byte *>(map->addr) + offset);
  }

  inline operator byte *() 
  { 
    return getAs<byte>(); 
  }

  void Release()
  {
    map = nullptr;
  }

  inline bool isValid()
  {
    return static_cast<bool>(map);
  }

private:
  struct MapWrapper
  {
    void *addr;

    MapWrapper(void *addr) : addr(addr) {};
    ~MapWrapper();

    NO_COPY_CONSTRUCTOR(MapWrapper);
    NO_ASSIGNMENT_OPERATOR(MapWrapper);
  };

  std::shared_ptr<MapWrapper> map;
  size_t offset;
};

// ----------------------------------------------------------------------------
