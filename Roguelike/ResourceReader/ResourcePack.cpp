/*********************************
 * ResourcePack.cpp
 * Connor Hilarides
 * Created 2014/08/08
 *********************************/

#include "ResourcePack.h"
#include "FileMapping.h"
#include "Helpers\Console.h"
#include <vector>
#include <unordered_map>
#include <string>

#include "ResourcePackBinaryHeaders.h"

// ----------------------------------------------------------------------------

using namespace Respack;

template <typename Header>
struct MemMapping
{
  Header header;
  size_t map_offset;
};

typedef MemMapping<ResContainerHeader> MemContainerMapping;
typedef MemMapping<ResHeader> MemResourceMapping;

// ----------------------------------------------------------------------------

struct ResMemoryContainer;

struct ResPackImpl
{
  ResPackImpl(const fs::wpath& path, const fs::wpath& fallback);
  ~ResPackImpl();

  void mapContainerHeads();

  ResourceContainer *getContainer(const std::string& name);
  
  bool memoryContainerExists(const std::string& name);
  ResourceContainer *getMemoryContainer(const std::string& name);
  ResourceContainer *getFallbackContainer(const std::wstring& name);

  FileMapping *packmap = nullptr;
  fs::wpath fallback;

  ResPackHeader packHeader;
  std::vector<MemContainerMapping> memoryContainers;

  std::unordered_map<std::string, ResMemoryContainer *> memContainers;

  NO_COPY_CONSTRUCTOR(ResPackImpl);
  NO_ASSIGNMENT_OPERATOR(ResPackImpl);
};

// ----------------------------------------------------------------------------

struct ResMemoryContainer : public ResourceContainer
{
  ResMemoryContainer(const MemContainerMapping& mapping, FileMapping& file, const fs::wpath& fallback);

  void Release() override;

  void mapResources();

  UINT getResourceCount() override;
  const char *getContainerName() override;
  Resource *getResource(const std::string& name) override;

  bool memoryResourceExists(const std::string& name);
  bool fileResourceExists(const std::wstring& name);
  Resource *getMemoryResource(const std::string& name);
  Resource *getFileResource(const std::wstring& name);

  MemContainerMapping mapping;
  FileMapping& file;
  fs::wpath fallback;

  std::vector<MemResourceMapping> memoryResources;

  NO_COPY_CONSTRUCTOR(ResMemoryContainer);
  NO_ASSIGNMENT_OPERATOR(ResMemoryContainer);
};

// ----------------------------------------------------------------------------

struct MemoryResource : public Resource
{
  MemoryResource(const MemResourceMapping& mapping, FileMapping& file);

  void Release() override;

  size_t getSize() override;
  byte *getData() override;
  TempFile getTempFile() override;
  std::chrono::system_clock::time_point getModified() override;

  MemResourceMapping mapping;
  FileMappingView view;

  NO_COPY_CONSTRUCTOR(MemoryResource);
  NO_ASSIGNMENT_OPERATOR(MemoryResource);
};

// ----------------------------------------------------------------------------

struct ResFallbackContainer : public ResourceContainer
{
  ResFallbackContainer(const fs::wpath& folder, const std::wstring& name);

  void Release() override;

  UINT getResourceCount() override;
  const char *getContainerName() override;
  Resource *getResource(const std::string& name) override;

  std::string name;
  fs::wpath folder;
  int itemCount = -1;

  NO_COPY_CONSTRUCTOR(ResFallbackContainer);
  NO_ASSIGNMENT_OPERATOR(ResFallbackContainer);
};

// ----------------------------------------------------------------------------

struct FileResource : public Resource
{
  FileResource(const fs::wpath& path);

  void Release() override;

  size_t getSize() override;
  byte *getData() override;
  TempFile getTempFile() override;
  std::chrono::system_clock::time_point getModified() override;

  fs::wpath path;
  bool loaded = false;
  shared_array<byte> data;

  NO_COPY_CONSTRUCTOR(FileResource);
  NO_ASSIGNMENT_OPERATOR(FileResource);
};

// ----------------------------------------------------------------------------

ResourcePack::ResourcePack(const fs::wpath& path, const fs::wpath& fallbackFolder)
  : impl(std::make_shared<ResPackImpl>(path, fallbackFolder))
{
}

// ----------------------------------------------------------------------------

ResourceContainer *ResourcePack::operator[](const std::string& containerName)
{
  return impl->getContainer(containerName);
}

// ----------------------------------------------------------------------------

ResPackImpl::ResPackImpl(const fs::wpath& path, const fs::wpath& fallback)
  : fallback(fallback)
{
  try
  {
    packmap = new FileMapping(path);
    mapContainerHeads();
  }

  catch (std::exception& e)

  {
    std::cerr << console::fg::yellow << "[WARN] "
              << "Resource pack could not be loaded. "
              << "Reading files in fallback mode" << std::endl;
    std::cerr << "  " << e.what() << std::endl;

    // run in fallback mode, only reading from
    // the fallback folder
    packmap = nullptr;
  }
}

// ----------------------------------------------------------------------------

ResPackImpl::~ResPackImpl()
{
  delete packmap;
}

// ----------------------------------------------------------------------------

void ResPackImpl::mapContainerHeads()
{
  if (!packmap)
    return;

  packHeader = packmap->readStruct<ResPackHeader>(0);

  size_t offset = sizeof(ResPackHeader);
  for (unsigned i = 0; i < packHeader.container_count; ++i)
  {
    MemContainerMapping mapping;
    mapping.map_offset = offset + sizeof(ResContainerHeader);
    mapping.header = packmap->readStruct<ResContainerHeader>(offset);
    mapping.header.container_name[sizeof(mapping.header.container_name) - 1] = 0;
    
    offset += sizeof(ResContainerHeader);
    offset += mapping.header.total_size;

    memoryContainers.push_back(mapping);
  }
}

// ----------------------------------------------------------------------------

bool ResPackImpl::memoryContainerExists(const std::string& name)
{
  for (auto& containermap : memoryContainers)
  {
    if (strcmpi(containermap.header.container_name, name.c_str()) == 0)
    {
      return true;
    }
  }

  return false;
}

// ----------------------------------------------------------------------------

ResourceContainer *ResPackImpl::getContainer(const std::string& name)
{
  if (memoryContainerExists(name))
  {
    return getMemoryContainer(name);
  }
  else
  {
    return getFallbackContainer(widen(name));
  }
}

// ----------------------------------------------------------------------------

ResourceContainer *ResPackImpl::getMemoryContainer(const std::string& name)
{
  auto it = memContainers.find(name);
  if (it != memContainers.end())
    return it->second;

  MemContainerMapping *mapping = nullptr;
  for (auto& containermap : memoryContainers)
  {
    if (strcmpi(containermap.header.container_name, name.c_str()) == 0)
    {
      mapping = &containermap;
      break;
    }
  }

  if (mapping == nullptr)
    return nullptr;

  auto *container = new ResMemoryContainer{*mapping, *packmap, fallback / widen(name)};
  memContainers[name] = container;

  return container;
}

// ----------------------------------------------------------------------------

ResourceContainer *ResPackImpl::getFallbackContainer(const std::wstring& name)
{
  return new ResFallbackContainer(fallback, name);
}

// ----------------------------------------------------------------------------

ResMemoryContainer::ResMemoryContainer(const MemContainerMapping& mapping, 
                                       FileMapping& file, const fs::wpath& fallback)
  : mapping(mapping), file(file), fallback(fallback)
{
  mapResources();
}

// ----------------------------------------------------------------------------

void ResMemoryContainer::Release()
{
  // Do nothing because we want to cache them inside that map
  // they won't be destroyed until the pack is destructed, and
  // that release logic is handled there. Users of this API should
  // still call this function in case behavior changes.
}

// ----------------------------------------------------------------------------

void ResMemoryContainer::mapResources()
{
  auto offset = this->mapping.map_offset;
  for (unsigned i = 0; i < this->mapping.header.resource_count; ++i)
  {
    MemResourceMapping mapping;
    mapping.map_offset = offset + sizeof(ResHeader);
    mapping.header = file.readStruct<ResHeader>(offset);
    mapping.header.resource_name[sizeof(mapping.header.resource_name) - 1] = 0;

    memoryResources.push_back(mapping);

    offset += sizeof(ResHeader);
    offset += mapping.header.resource_size;

    memoryResources.push_back(mapping);
  }
}

// ----------------------------------------------------------------------------

UINT ResMemoryContainer::getResourceCount()
{
  return mapping.header.resource_count;
}

// ----------------------------------------------------------------------------

const char *ResMemoryContainer::getContainerName()
{
  return mapping.header.container_name;
}

// ----------------------------------------------------------------------------

Resource *ResMemoryContainer::getResource(const std::string& name)
{
  auto wname = widen(name);

  if (fileResourceExists(wname))
  {
    return getFileResource(wname);
  }
  else if (memoryResourceExists(name))
  {
    return getMemoryResource(name);
  }
  else
  {
    return nullptr;
  }
}

// ----------------------------------------------------------------------------

bool ResMemoryContainer::memoryResourceExists(const std::string& name)
{
  for (auto& resmap : memoryResources)
  {
    if (strcmpi(resmap.header.resource_name, name.c_str()) == 0)
    {
      return true;
    }
  }

  return false;
}

// ----------------------------------------------------------------------------

bool ResMemoryContainer::fileResourceExists(const std::wstring& name)
{
  return fs::exists(fallback / name);
}

// ----------------------------------------------------------------------------

Resource *ResMemoryContainer::getMemoryResource(const std::string& name)
{
  for (auto& resmap : memoryResources)
  {
    if (strcmpi(resmap.header.resource_name, name.c_str()) == 0)
      return new MemoryResource(resmap, file);
  }

  return nullptr;
}

// ----------------------------------------------------------------------------

Resource *ResMemoryContainer::getFileResource(const std::wstring& name)
{
  return new FileResource(fallback / name);
}

// ----------------------------------------------------------------------------

ResFallbackContainer::ResFallbackContainer(const fs::wpath& folder, const std::wstring& name)
  : folder(folder / name), name(narrow(name))
{
}

// ----------------------------------------------------------------------------

void ResFallbackContainer::Release()
{
  delete this;
}

// ----------------------------------------------------------------------------

UINT ResFallbackContainer::getResourceCount()
{
  if (itemCount != -1)
    return itemCount;

  fs::directory_contents contents{folder, L"*.*", fs::directory_contents::files};
  return itemCount = contents.itemCount();
}

// ----------------------------------------------------------------------------

const char *ResFallbackContainer::getContainerName()
{
  return name.c_str();
}

// ----------------------------------------------------------------------------

Resource *ResFallbackContainer::getResource(const std::string& name)
{
  return new FileResource(folder / widen(name));
}

// ----------------------------------------------------------------------------

MemoryResource::MemoryResource(const MemResourceMapping& mapping, FileMapping& file)
  : mapping(mapping), view(file.mapView(mapping.map_offset, mapping.header.resource_size))
{
}

// ----------------------------------------------------------------------------

void MemoryResource::Release()
{
  delete this;
}

// ----------------------------------------------------------------------------

size_t MemoryResource::getSize()
{
  return mapping.header.resource_size;
}

// ----------------------------------------------------------------------------

byte *MemoryResource::getData()
{
  return view;
}

// ----------------------------------------------------------------------------

TempFile MemoryResource::getTempFile()
{
  return TempFile::create(view, getSize());
}

// ----------------------------------------------------------------------------

std::chrono::system_clock::time_point MemoryResource::getModified()
{
  return mapping.header.updated_at;
}

// ----------------------------------------------------------------------------

FileResource::FileResource(const fs::wpath& path)
  : path(path)
{
}

// ----------------------------------------------------------------------------

void FileResource::Release()
{
  delete this;
}

// ----------------------------------------------------------------------------

size_t FileResource::getSize()
{
  return fs::file_size(path);
}

// ----------------------------------------------------------------------------

byte *FileResource::getData()
{
  if (!loaded)
  {
    data = fs::file_reader::readAllBytes(path).data;
  }

  return data;
}

// ----------------------------------------------------------------------------

TempFile FileResource::getTempFile()
{
  return TempFile::wrapNonTemp(path);
}

// ----------------------------------------------------------------------------

std::chrono::system_clock::time_point FileResource::getModified()
{
  return std::chrono::system_clock::from_time_t(fs::last_write_time(path));
}

// ----------------------------------------------------------------------------

