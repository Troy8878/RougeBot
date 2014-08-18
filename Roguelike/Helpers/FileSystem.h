/*********************************
 * FileSystem.h
 * Connor Hilarides
 * Created 2014/05/29
 *********************************/

#pragma once

#include <filesystem>

#include "FixedWindows.h"
#include "SharedArray.h"

namespace fs = std::tr2::sys;

template <typename String, typename Traits>
inline fs::basic_path<String, Traits> operator/(const fs::basic_path<String, Traits>& path, const String& append)
{
  fs::basic_path<String, Traits> copy(path);
  copy /= append;
  return copy;
}

namespace std { namespace tr2 { namespace sys
{
  class directory_contents final
  {
  public:
    enum mode
    {
      all_entries,
      directories,
      files
    };

  private:
    std::wstring fullpath;
    std::wstring mask;
    mode smode;
    int _itemCount = -1;

  public:
    class iterator final
    {
      const directory_contents *parent;
      fs::wpath _pathValue;

      HANDLE hFind;
      WIN32_FIND_DATAW findData;

    public:
      typedef fs::wpath value_type;
      typedef const value_type *pointer;

      iterator(const iterator&) = delete;
      iterator& operator=(const iterator&) = delete;

      iterator(iterator&& other)
        : parent(other.parent), _pathValue(other._pathValue),
          hFind(other.hFind), findData(other.findData)
      {
        other.hFind = INVALID_HANDLE_VALUE;
      }

      iterator& operator=(iterator&& other)
      {
        this->~iterator();
        new (this) iterator(std::move(other));
      }

      ~iterator()
      {
        if (hFind == INVALID_HANDLE_VALUE)
          return;

        FindClose(hFind);
      }

      iterator(const directory_contents *parent)
        : parent(parent)
      {
        hFind = FindFirstFileW((parent->fullpath + L"\\" + parent->mask).c_str(), &findData);
        
        initializePath();
      }

      value_type& operator*()
      {
        return _pathValue;
      }

      const value_type& operator*() const
      {
        return _pathValue;
      }

      pointer operator->() const
      {
        return &_pathValue;
      }

      iterator& operator++()
      {
        if (!FindNextFileW(hFind, &findData))
        {
          FindClose(hFind);
          hFind = INVALID_HANDLE_VALUE;
          return *this;
        }

        switch (parent->smode)
        {
          case directories:
            if (is_file())
              return ++*this;
            break;

          case files:
            if (is_directory())
              return ++*this;
            break;
        }

        initializePath();

        return *this;
      }

      bool operator==(const iterator& right)
      {
        if (hFind == INVALID_HANDLE_VALUE)
          return right.hFind == INVALID_HANDLE_VALUE;

        return _pathValue == right._pathValue;
      }

      inline bool operator!=(const iterator& right)
      {
        return !(*this == right);
      }

      bool is_directory()
      {
        return !!(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY);
      }

      bool is_file()
      {
        return !is_directory();
      }

    private:

      iterator(HANDLE hFind)
        : hFind(hFind), parent(nullptr)
      {
      }

      void initializePath()
      {
        if (hFind == INVALID_HANDLE_VALUE)
          return;

        _pathValue = fs::wpath{parent->fullpath} / fs::wpath{findData.cFileName};

        if (_pathValue.filename() == L"." || _pathValue.filename() == L"..")
          ++*this;
      }

      friend class directory_contents;
    };

    directory_contents(const fs::path& path, const std::string& mask = "*.*", mode smode = all_entries)
      : fullpath(widen(path.directory_string())), mask(widen(mask)), smode(smode)
    {
    }

    directory_contents(const fs::wpath& path, const std::wstring& mask = L"*.*", mode smode = all_entries)
      : fullpath(path.directory_string()), mask(mask), smode(smode)
    {
    }

    iterator begin() const
    {
      return iterator{this};
    }

    iterator end() const
    {
      return iterator{INVALID_HANDLE_VALUE};
    }

    int itemCount()
    {
      if (_itemCount != -1)
        return _itemCount;

      _itemCount = 0;
      for (auto it = begin(); it != end(); ++it)
        _itemCount++;

      return _itemCount;
    }
  };

  struct binary_file_data
  {
    size_t size;
    shared_array<byte> data;

    operator byte *() { return data; }
  };

  class file_reader
  {
  public:
    static binary_file_data readAllBytes(const wpath& file)
    {
      auto filename = file.file_string();
      auto size = file_size(file);
      if (size == INVALID_FILE_SIZE)
        return {0, nullptr};

      shared_array<byte> buffer(size);

      FILE *pfile;
      if (_wfopen_s(&pfile, filename.c_str(), L"rb") != 0)
        return {0, nullptr};

      if (fread_s(buffer, size, sizeof(byte), size, pfile) != size)
        return {0, nullptr};

      return {size, buffer};
    }
  };

  class file_not_found_exception : public string_exception
  {
  public:
    file_not_found_exception(const std::string& message)
      : string_exception(message)
    {
    }
  };

}}}
