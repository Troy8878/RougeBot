/*********************************
 * PNGReader.cpp
 * Connor Hilarides
 * Created 2014/08/07
 *********************************/

#include "PNGReader.h"
#include "Helpers\FileSystem.h"

ImageResource LoadPNGFromFile(const std::wstring& path)
{
  auto data = fs::file_reader::readAllBytes(widen(path));
  return LoadPNGFromBinary(data.data, data.size);
}

ImageResource LoadPNGFromBinary(ImageResource::byte *data, size_t length)
{
  return ImageResource{};
}

