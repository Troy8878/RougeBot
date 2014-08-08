/*********************************
 * PNGReader.h
 * Connor Hilarides
 * Created 2014/08/07
 *********************************/

#pragma once

#include "ImageResource.h"
#include <memory>
#include <string>

ImageResource LoadPNGFromFile(const std::wstring& path);
ImageResource LoadPNGFromBinary(ImageResource::byte *data, size_t length);

