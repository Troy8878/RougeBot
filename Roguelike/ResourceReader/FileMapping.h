/*********************************
 * FileMapping.h
 * Connor Hilarides
 * Created 2014/08/17
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
  FileMappingView(void *address);
  friend class FileMapping;

public:
  template <typename T>
  inline T *getAs()
  {
    return reinterpret_cast<T *>(map->addr);
  }

  inline operator byte *() { return getAs<byte>(); }

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
};

// ----------------------------------------------------------------------------
