/*********************************
 * FileSystem.cpp
 * Connor Hilarides
 * Created 2014/07/07
 * Copyright © 2014 DigiPen Institute of Technology, All Rights Reserved
 *********************************/

#include "FileSystem.h"

namespace std { namespace tr2 { namespace sys
{

  size_t fileSize(const wpath& file)
  {
    HANDLE hfile = CreateFileW(file.file_string().c_str(),
                              GENERIC_READ,
                              FILE_SHARE_READ,
                              nullptr,
                              OPEN_EXISTING,
                              FILE_ATTRIBUTE_NORMAL,
                              nullptr);

    if (hfile == INVALID_HANDLE_VALUE)
      return INVALID_FILE_SIZE;

    return GetFileSize(hfile, NULL);
  }

}}}
