// ResourcePacker.cpp : main project file.

#include "stdafx.h"
#include <cassert>

using namespace System;

std::string RefStringToStdString(String ^str)
{
  return msclr::interop::marshal_as<std::string>(str);
}

ref class Asset
{
public:
  String  ^path;
  String  ^name;
  unsigned size;
  DateTime modified;
};

std::chrono::system_clock::time_point GetFileTime(DateTime fileTime)
{
  time_t ctime = (time_t)(fileTime - DateTime(1970, 1, 1)).TotalMilliseconds;

  return std::chrono::system_clock::from_time_t(ctime);
}

void AddSubassets(Collections::Generic::IList<Asset ^> ^list, IO::DirectoryInfo ^dir, String ^prefix)
{
  for each (auto file in dir->GetFiles())
  {
    if (file->Length > UINT32_MAX)
      continue;

    auto asset = gcnew Asset();

    asset->path = file->FullName;
    asset->name = prefix + file->Name;
    asset->size = (unsigned) file->Length;
    asset->modified = file->LastWriteTimeUtc;

    list->Add(asset);
  }

  prefix += "/" + dir->Name;

  for each (auto subdir in dir->GetDirectories())
  {
    AddSubassets(list, subdir, prefix);
  }
}

void WriteAsset(FILE *file, Asset ^asset)
{
  auto cpppath = RefStringToStdString(asset->path);
  auto data = fs::file_reader::readAllBytes(widen(cpppath)).data;

  assert(data.size() == asset->size);

  ResHeader header;
  header.resource_size = (unsigned) data.size();
  header.updated_at = GetFileTime(asset->modified);
  
  auto cppname = RefStringToStdString(asset->name);
  auto strnum = std::min(cppname.size(), sizeof(header.resource_name) - 1);
  strncpy_s(header.resource_name, cppname.c_str(), strnum);
  header.resource_name[sizeof(header.resource_name) - 1] = 0;

  fwrite(&header, sizeof(header), 1, file);
  fwrite(data, 1, data.size(), file);
}

void WriteAssetDir(FILE *file, IO::DirectoryInfo ^dir)
{
  using namespace Collections::Generic;
  List<Asset ^> ^assets = gcnew List<Asset ^>();
  AddSubassets(assets, dir, "");

  ResContainerHeader header;
  header.resource_count = assets->Count;
  header.total_size = 0;
  for each (auto asset in assets)
  {
    header.total_size += sizeof(ResHeader);
    header.total_size += asset->size;
  }

  auto cppname = RefStringToStdString(dir->Name);
  auto strnum = std::min(cppname.size(), sizeof(header.container_name) - 1);
  strncpy_s(header.container_name, cppname.c_str(), strnum);
  header.container_name[sizeof(header.container_name) - 1] = 0;
  
  fwrite(&header, sizeof(header), 1, file);

  for each (auto asset in assets)
  {
    WriteAsset(file, asset);
  }
}

int main(array<System::String ^> ^args)
{
  Console::WriteLine("Current Directory is " + Environment::CurrentDirectory);
  Console::Write("What folder do you want to pack up? ");
  auto folder = Console::ReadLine();
  auto folderInfo = gcnew IO::DirectoryInfo(folder);
  auto outfile = RefStringToStdString(folderInfo->Name + ".respack");

  auto subDirs = folderInfo->GetDirectories();

  FILE *file;
  if (fopen_s(&file, outfile.c_str(), "wb"))
  {
    Console::WriteLine("Failed to open file");
    return 1;
  }

  ResPackHeader packheader;
  packheader.container_count = (USHORT) subDirs->Length;
  fwrite(&packheader, sizeof(packheader), 1, file);

  for each (auto subDir in subDirs)
  {
    WriteAssetDir(file, subDir);
  }

  fclose(file);

  return 0;
}
