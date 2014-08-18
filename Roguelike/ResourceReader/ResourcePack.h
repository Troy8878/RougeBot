/*********************************
 * ResourcePack.h
 * Connor Hilarides
 * Created 2014/08/08
 *********************************/

#pragma once

#include "Helpers\FileSystem.h"
#include "TempFile.h"
#include <string>
#include <chrono>

class Resource abstract
{
public:
  virtual void Release();

  virtual size_t getSize() = 0;
  virtual TempFile getTempFile() = 0;
  virtual shared_array<byte> getData() = 0;
  virtual std::chrono::system_clock getModified() = 0;

protected:
  virtual ~Resource() {}
};

class ResourceContainer abstract
{
public:
  virtual void Release();

  virtual Resource *operator[](const std::string& resource) = 0;

protected:
  virtual ~ResourceContainer() {}
};

class ResPackImpl;

class ResourcePack
{
public:
  ResourcePack(const fs::wpath& file);
  ResourcePack(shared_array<byte> data);
  ~ResourcePack();
  
  ResourceContainer *operator[](const std::string& container);

private:
  std::shared_ptr<ResPackImpl> impl;
};


template <typename T>
class TempFileResLoader
{
  template <typename... Args>
  static T load(Resource *res, Args... args)
  {
    auto data = res->getData();
    auto file = TempFile::create(data, data.size());

    return T(args..., file.getPath());
  }
};

template <typename T>
class BinaryResLoader
{
  template <typename... Args>
  static T load(Resource *res);
};

