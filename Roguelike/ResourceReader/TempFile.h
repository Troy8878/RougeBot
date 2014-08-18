/*********************************
 * TempFile.h
 * Connor Hilarides
 * Created 2014/08/08
 *********************************/

#pragma once

#include "Helpers\FileSystem.h"

class TempFile
{
public:
  static TempFile create(byte *data, size_t size);
  static TempFile wrapNonTemp(const fs::wpath& path);
  static fs::wpath getTempPath();

  fs::wpath getPath() { return handle->path; }

private:
  TempFile(const fs::wpath& file);
  
  struct TempFileInternal
  {
    bool istemp = true;

    TempFileInternal(const fs::wpath& path);
    ~TempFileInternal();

    fs::wpath path;
  };

  std::shared_ptr<TempFileInternal> handle;
};
