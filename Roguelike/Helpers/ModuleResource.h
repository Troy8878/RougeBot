/*********************************
 * ModuleResource.h
 * Connor Hilarides
 * Created 2014/05/29
 *********************************/

#include "FixedWindows.h"

#include <string>

class ModuleResource final
{
public:
  inline ModuleResource(int id, const char *type)
  {
    auto module = GetModuleHandle(NULL);

    resource = FindResource(module, MAKEINTRESOURCE(id), (type));
    resHandle = LoadResource(module, resource);
    size = SizeofResource(module, resource);
    data = LockResource(resHandle);
  }

  inline ~ModuleResource()
  {
    UnlockResource(resHandle);
  }

  template <typename T>
  inline T *dataAs()
  {
    return reinterpret_cast<T *>(data);
  }

  inline size_t dataSize()
  {
    return size;
  }

  inline std::string readAsString()
  {
    auto copy = new char[size + 1];
    memcpy_s(copy, size, data, size);
    copy[size] = 0;

    std::string value(copy);
    delete[] copy;

    return value;
  }

private:
  HRSRC resource;
  HANDLE resHandle;
  size_t size;
  void *data;
};

