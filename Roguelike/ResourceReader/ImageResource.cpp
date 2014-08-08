/*********************************
 * ImageResource.cpp
 * Connor Hilarides
 * Created 2014/08/07
 *********************************/

#include "ImageResource.h"

void ImageResource::Dispose()
{
  delete[] data;
}

