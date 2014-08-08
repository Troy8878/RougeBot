/*********************************
 * ImageResource.cpp
 * Connor Hilarides
 * Created 2014/08/07
 *********************************/

#include "ImageResource.h"
#include "Helpers\FileSystem.h"
#include "Helpers\CriticalSections.h"

void ImageResource::to32BitColor(byte *destination, size_t dest_size)
{
  switch (format)
  {

    case Format::RGB:
      for (size_t i = 0; i < data.size() / 3 && i < dest_size / 4; ++i)
      {
        destination[i * 4 + 0] = data[i * 3 + 0]; // R component
        destination[i * 4 + 1] = data[i * 3 + 1]; // G component
        destination[i * 4 + 2] = data[i * 3 + 2]; // B component
        destination[i * 4 + 3] = 255; // Set alpha channel to white
      }
      break;

    case Format::RGBA:
      memcpy_s(destination, dest_size, data, data.size());
      break;

    case Format::BGR:
      for (size_t i = 0; i < data.size() / 3 && i < dest_size / 4; ++i)
      {
        destination[i * 4 + 0] = data[i * 3 + 2]; // R component
        destination[i * 4 + 1] = data[i * 3 + 1]; // G component
        destination[i * 4 + 2] = data[i * 3 + 0]; // B component
        destination[i * 4 + 3] = 255; // Set alpha channel to white
      }
      break;

    case Format::BGRA:
      for (size_t i = 0; i < data.size() / 4 && i < dest_size / 4; ++i)
      {
        destination[i * 4 + 0] = data[i * 3 + 2]; // R component
        destination[i * 4 + 1] = data[i * 3 + 1]; // G component
        destination[i * 4 + 2] = data[i * 3 + 0]; // B component
        destination[i * 4 + 3] = data[i * 3 + 3]; // A component
      }
      break;
  }
}

static IWICImagingFactory *getImagingFactory()
{
  THREAD_EXCLUSIVE_SCOPE;

  static IWICImagingFactory *factory = nullptr;
  if (factory)
    return factory;
  
  HRESULT hr = CoCreateInstance(
    CLSID_WICImagingFactory,
    nullptr,
    CLSCTX_INPROC_SERVER,
    IID_IWICImagingFactory,
    reinterpret_cast<LPVOID*>(&factory));
  CHECK_HRESULT(hr);

  return factory;
}

ImageResource ImageResource::fromFile(const std::wstring& file)
{
  ImageResource image;

  auto fileData = fs::file_reader::readAllBytes(file);

  IWICImagingFactory *factory = getImagingFactory();
  HRESULT hr;
  
  IWICBitmapDecoder *decoder;
  hr = factory->CreateDecoderFromFilename(
    file.c_str(),
    nullptr, 
    GENERIC_READ, 
    WICDecodeMetadataCacheOnLoad,
    &decoder);
  CHECK_HRESULT(hr);
  RELEASE_AFTER_SCOPE(decoder);

  IWICBitmapFrameDecode *source;
  hr = decoder->GetFrame(0, &source);
  CHECK_HRESULT(hr);
  RELEASE_AFTER_SCOPE(source);

  IWICFormatConverter *converter;
  hr = factory->CreateFormatConverter(&converter);
  CHECK_HRESULT(hr);
  RELEASE_AFTER_SCOPE(converter);

  hr = converter->Initialize(
    source,
    GUID_WICPixelFormat32bppRGBA,
    WICBitmapDitherTypeNone,
    nullptr,
    0.f,
    WICBitmapPaletteTypeMedianCut);
  CHECK_HRESULT(hr);
  
  image.format = Format::RGBA;

  hr = converter->GetSize(&image.width, &image.height);
  CHECK_HRESULT(hr);

  image.data = shared_array<byte>(image.width * image.height * 4);
  hr = converter->CopyPixels(
    nullptr,
    image.width * 4,
    static_cast<UINT>(image.data.size()),
    image.data);
  CHECK_HRESULT(hr);

  return image;
}

