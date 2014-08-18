/*********************************
 * TempFile.cpp
 * Connor Hilarides
 * Created 2014/08/08
 *********************************/

#include "TempFile.h"
#include <fstream>

// ----------------------------------------------------------------------------

TempFile TempFile::create(byte *data, size_t size)
{
  auto path = getTempPath().file_string();

  std::ofstream file{path.c_str(), std::ios::out | std::ios::binary};
  file.write(reinterpret_cast<char *>(data), size);
  file.close();

  return TempFile{path};
}

// ----------------------------------------------------------------------------

TempFile TempFile::wrapNonTemp(const fs::wpath& path)
{
  TempFile file{path};
  file.handle->istemp = false;
  return file;
}

// ----------------------------------------------------------------------------

fs::wpath TempFile::getTempPath()
{
  wchar_t temp_folder[MAX_PATH];
  wchar_t temp_file[MAX_PATH];

  GetTempPathW(ARRAYSIZE(temp_folder), temp_folder);
  GetTempFileNameW(temp_folder, L"rgl", 0, temp_file);

  return temp_file;
}

// ----------------------------------------------------------------------------

TempFile::TempFile(const fs::wpath& path)
  : handle(std::make_shared<TempFileInternal>(path))
{
}

// ----------------------------------------------------------------------------

TempFile::TempFileInternal::TempFileInternal(const fs::wpath& path)
  : path(path)
{
}

// ----------------------------------------------------------------------------

TempFile::TempFileInternal::~TempFileInternal()
{
  if (istemp)
    DeleteFileW(path.file_string().c_str());
}

// ----------------------------------------------------------------------------

