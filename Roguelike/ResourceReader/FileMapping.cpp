/*********************************
 * FileMapping.cpp
 * Connor Hilarides
 * Created 2014/08/17
 *********************************/

#include "FileMapping.h"

// ----------------------------------------------------------------------------

FileMapping::FileMapping(const fs::wpath& path, bool read, bool write)
{
  const DWORD access = (read ? GENERIC_READ : 0) | (write ? GENERIC_WRITE : 0);
  DWORD creation = 0;
  if (read)
    creation = OPEN_EXISTING;
  else if (write)
    creation = CREATE_ALWAYS;

  fileHandle = CreateFileW(path.file_string().c_str(), 
                           access,
                           write ? 0 : FILE_SHARE_READ,
                           nullptr,
                           creation,
                           FILE_ATTRIBUTE_NORMAL,
                           nullptr);
  if (fileHandle == INVALID_HANDLE_VALUE)
    throw fs::file_not_found_exception{"Couldn't open file " + narrow(path.file_string())};

  DWORD protections = 0;
  if (write)
    protections = PAGE_READWRITE;
  else
    protections = PAGE_READONLY;

  mapHandle = CreateFileMappingW(fileHandle,
                                 nullptr,
                                 protections,
                                 0, 0,
                                 nullptr);
  if (mapHandle == nullptr)
    throw std::exception("Failed to create file mapping");

  if (read)
    viewAccess |= FILE_MAP_READ;
  if (write)
    viewAccess |= FILE_MAP_WRITE;
}

// ----------------------------------------------------------------------------

FileMapping::~FileMapping()
{
  if (fileHandle != INVALID_HANDLE_VALUE)
    CloseHandle(fileHandle);

  if (mapHandle != nullptr)
    CloseHandle(mapHandle);
}

// ----------------------------------------------------------------------------

FileMappingView FileMapping::mapView(size_t offset, size_t size)
{
  void *addr = MapViewOfFile(mapHandle,
                             viewAccess,
                             HIDWORD(offset),
                             LODWORD(offset),
                             size);

  if (addr == nullptr)
    throw win32_exception();
  
  return FileMappingView{addr};
}

// ----------------------------------------------------------------------------

FileMappingView::FileMappingView(void *address)
  : map(std::make_shared<MapWrapper>(address))
{
}

// ----------------------------------------------------------------------------

FileMappingView::MapWrapper::~MapWrapper()
{
  UnmapViewOfFile(addr);
}

// ----------------------------------------------------------------------------
