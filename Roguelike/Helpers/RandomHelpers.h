/*********************************
 * RandomHelpers.h
 * Connor Hilarides
 * Created 2014/08/20
 *********************************/

#pragma once

#include "FixedWindows.h"
#include <unordered_map>
#include <chrono>
#include "json/json.h"

// ----------------------------------------------------------------------------

typedef _com_error COMError;

// ----------------------------------------------------------------------------

inline std::wstring widen(const std::string& narrow_string)
{
  std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
  return converter.from_bytes(narrow_string.c_str());
}

// ----------------------------------------------------------------------------

inline std::string narrow(const std::wstring& wide_string)
{
  std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
  return converter.to_bytes(wide_string);
}

// ----------------------------------------------------------------------------

#define CSTR_WIDEN(x) widen(x).c_str()
#define CSTR_NARROW(x) narrow(x).c_str()

// ----------------------------------------------------------------------------

#ifdef _DEBUG
#define IFDEBUG(x) x
#define IFNDEBUG(x) 
#else
#define IFDEBUG(x) 
#define IFNDEBUG(x) x
#endif

// ----------------------------------------------------------------------------

// just a nice little helper to turn GetLastError() into a readable message
inline std::string GetLastErrorString()
{
  LPSTR pBuffer = 0;

  FormatMessage(
    FORMAT_MESSAGE_ALLOCATE_BUFFER |
    FORMAT_MESSAGE_FROM_SYSTEM |
    FORMAT_MESSAGE_IGNORE_INSERTS,
    NULL,
    GetLastError(),
    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
    (LPTSTR) &pBuffer,
    0, NULL);

  std::string result = pBuffer;
  LocalFree(pBuffer);

  return result;
}

// ----------------------------------------------------------------------------

class DXFatalError : public std::exception
{
public:

  DXFatalError(HRESULT hr)
    : _error(hr)
  {
  }

  const char *what() const override
  {
    return reinterpret_cast<const char *>(_error.ErrorMessage());
  }

  COMError& error()
  {
    return _error;
  }

  const COMError& error() const
  {
    return _error;
  }

private:
  COMError _error;
};

// ----------------------------------------------------------------------------

class string_exception : public std::exception
{
  std::string message;

public:
  string_exception(const std::string& message)
    : message(message)
  {
  }

  const char *what() const override
  {
    return message.c_str();
  }
};

// ----------------------------------------------------------------------------

class win32_exception : public string_exception
{
public:
  win32_exception()
    : string_exception(GetLastErrorString())
  {
  }
};

// ----------------------------------------------------------------------------

template <class Interface>
/** Safely releases DirectX interfaces */
inline void ReleaseDXInterface(Interface *& interfaceToRelease)
{
  if (interfaceToRelease)
  {
    interfaceToRelease->Release();
    interfaceToRelease = nullptr;
  }
}

// ----------------------------------------------------------------------------

template <class Interface>
class DXReleaser
{
public:
  DXReleaser(Interface *& itr)
    : itr(itr)
  {
  }

  ~DXReleaser()
  {
    ReleaseDXInterface(itr);
  }

  DXReleaser& operator=(const DXReleaser&) = delete;

private:
  Interface *& itr;
};

// ----------------------------------------------------------------------------

#define RELEASE_AFTER_SCOPE(itr) \
  DXReleaser<std::remove_reference<decltype(*(itr))>::type> \
  __##itr##__releaser{(itr)}

// ----------------------------------------------------------------------------

#define CHECK_HRESULT(hr) CheckHRESULT(hr)

// ----------------------------------------------------------------------------

inline void CheckHRESULT(HRESULT hr)
{
  if (FAILED(hr)) 
    throw DXFatalError(hr);
}

// ----------------------------------------------------------------------------

template <typename Interface>
void setDXDebugName(Interface *object, const std::wstring& name)
{
  setDXDebugName(object, narrow(name));
}

// ----------------------------------------------------------------------------

template <typename Interface>
void setDXDebugName(Interface *object, const std::string& name)
{
  HRESULT result = object->SetPrivateData(WKPDID_D3DDebugObjectName,
                                          (UINT) name.length(),
                                          name.c_str());
  CHECK_HRESULT(result);
}

// ----------------------------------------------------------------------------

template <typename Container>
void variadic_push_container(Container&)
{
}

// ----------------------------------------------------------------------------

template <typename Container, typename Arg>
void variadic_push_container(Container& container, const Arg& param)
{
  container.push_back(param);
}

// ----------------------------------------------------------------------------

template <typename Container, typename Arg, typename... Args>
void variadic_push_container(Container& containter, const Arg& param, 
                             const Args&&... params)
{
  container.push_back(param);
  variadic_push_container(container, params...);
}

// ----------------------------------------------------------------------------

template <typename T>
void variadic_push_array(T [], size_t)
{
}

// ----------------------------------------------------------------------------

template <typename T, typename First, typename... Rest>
void variadic_push_array(T array[], size_t index, 
                         const First& param, const Rest&... params)
{
  array[index] = param;
  variadic_push_array(array, index + 1, params...);
}

// ----------------------------------------------------------------------------

#define strcmpi _strcmpi

// ----------------------------------------------------------------------------

#define NO_COPY_CONSTRUCTOR(type) type(type const&) = delete
#define NO_ASSIGNMENT_OPERATOR(type) type& operator=(type const&) = delete

// ----------------------------------------------------------------------------

#define _PROPERTY_GET(pType, pName)                 \
  virtual pType& _PropGet ## pName() {              \
    return this->_##pName;                          \
  }                                                 \
  virtual pType const & _PropGet ## pName() const { \
    return this->_##pName;                          \
  }

#define _PROPERTY_SET(pType, pName)              \
  virtual void _PropSet ## pName(pType const& value) {  \
    this->_##pName = value;                      \
  }

#define _IPROPERTY_GET(pType, pName)               \
  inline pType& _PropGet ## pName() {              \
    return this->_##pName;                         \
  }                                                \
  inline pType const & _PropGet ## pName() const { \
    return this->_##pName;                         \
  }

#define _IPROPERTY_SET(pType, pName)            \
  inline void _PropSet ## pName(pType const& value) {  \
    this->_##pName = value;                     \
  }

#define _PROPERTY_STORAGE(pType, pName) \
  private: pType _##pName; public:

#define W_PROPERTY(pType, pName)  \
  _PROPERTY_STORAGE(pType, pName) \
  private:                        \
  _PROPERTY_GET(pType, pName)     \
  public:                         \
  _PROPERTY_SET(pType, pName)     \
  __declspec(property(            \
    get = _PropGet ## pName,      \
    put = _PropSet ## pName)      \
  ) pType pName

#define R_PROPERTY(pType, pName)  \
  _PROPERTY_STORAGE(pType, pName) \
  _PROPERTY_GET(pType, pName)     \
  private:                        \
  _PROPERTY_SET(pType, pName)     \
  public:                         \
  __declspec(property(            \
    get = _PropGet ## pName,      \
    put = _PropSet ## pName)      \
  ) pType pName

#define RW_PROPERTY(pType, pName) \
  _PROPERTY_STORAGE(pType, pName) \
  _PROPERTY_GET(pType, pName)     \
  _PROPERTY_SET(pType, pName)     \
  __declspec(property(             \
    get = _PropGet ## pName,       \
    put = _PropSet ## pName)       \
  ) pType pName

#define IW_PROPERTY(pType, pName) \
  _PROPERTY_STORAGE(pType, pName) \
  private:                        \
  _IPROPERTY_GET(pType, pName)    \
  public:                         \
  _IPROPERTY_SET(pType, pName)    \
  __declspec(property(             \
    get = _PropGet ## pName,       \
    put = _PropSet ## pName)       \
  ) pType pName

#define IR_PROPERTY(pType, pName) \
  _PROPERTY_STORAGE(pType, pName) \
  _IPROPERTY_GET(pType, pName)    \
  private:                        \
  _IPROPERTY_SET(pType, pName)    \
  public:                         \
  __declspec(property(             \
    get = _PropGet ## pName,       \
    put = _PropSet ## pName)       \
  ) pType pName

#define IRW_PROPERTY(pType, pName) \
  _PROPERTY_STORAGE(pType, pName)  \
  _IPROPERTY_GET(pType, pName)     \
  _IPROPERTY_SET(pType, pName)     \
  __declspec(property(             \
    get = _PropGet ## pName,       \
    put = _PropSet ## pName)       \
  ) pType pName

// ----------------------------------------------------------------------------

#define PROPERTY(...) __declspec(property(__VA_ARGS__))

// ----------------------------------------------------------------------------

struct component_factory_data : public std::unordered_map<std::string, std::string>
{
  typedef std::unordered_map<std::string, std::string> base;

  component_factory_data() = default;
  component_factory_data(const std::initializer_list<value_type>& list)
    : base(list)
  {
  }
};

// ----------------------------------------------------------------------------

template <typename CharT, typename TraitsT = std::char_traits<CharT>>
class ibufferstream : public std::basic_streambuf<CharT, TraitsT>
{
public:
  ibufferstream(CharT *start, size_t size)
  {
    setg(start, start, start + size);
  }
};

// ----------------------------------------------------------------------------

template <typename Map>
typename Map::mapped_type
map_fetch(const Map& map,
          const typename Map::key_type& key,
          const typename Map::mapped_type& def)
{
  auto it = map.find(key);
  if (it != map.end())
    return it->second;

  return def;
}

// ----------------------------------------------------------------------------

template <typename FwIt, typename CharT, typename CharTraits = std::char_traits<CharT>>
void svtprintf(std::basic_ostream<CharT, CharTraits>& out, FwIt first, FwIt last)
{
  while (first != last)
  {
    out.put(*first);
    ++first;
  }
}

// ----------------------------------------------------------------------------

template <typename FwIt, typename CharT, typename CharTraits = std::char_traits<CharT>, 
          typename Arg, typename... Args>
void svtprintf(std::basic_ostream<CharT, CharTraits>& out, 
               FwIt first, FwIt last, const Arg& value, const Args&... rest)
{
  while (first != last)
  {
    CharT c = *first;
    ++first;

    if (c == '%')
    {
      CharT n = *first;
      ++first;

      if (n == 'v')
      {
        out << value;
        svtprintf(out, first, last, rest...);
        return;
      }
      else if (n != '%')
      {
        throw std::logic_error("An unknown format specifier was encountered (not % or v)");
      }
    }

    out.put(c);
  }
}

// ----------------------------------------------------------------------------

template <typename... Args>
void vtprintf(const char *str, Args... args)
{
  svtprintf(std::cout, str, str + strlen(str), args...);
}

// ----------------------------------------------------------------------------

template <typename... Args>
void vtprintf(const std::string& str, Args... args)
{
  svtprintf(std::cout, str.begin(), str.end(), args...);
}

// ----------------------------------------------------------------------------

template <typename... Args>
void vtprintf(const wchar_t *str, Args... args)
{
  svtprintf(std::wcout, str, str + lstrlenW(str), args...);
}

// ----------------------------------------------------------------------------

template <typename... Args>
void vtprintf(const std::wstring& str, Args... args)
{
  svtprintf(std::wcout, str.begin(), str.end(), args...);
}

// ----------------------------------------------------------------------------

json::value ParseJsonAsset(const std::string& containerName, const std::string& asset);

// ----------------------------------------------------------------------------

/**
  Despite this function's name, it does not perform the operation
  asyncronously, but is safe to call from not-the-main-thread
  (as long as it's the only one trying), if you think the main
  thread may end before the function returns.
*/
bool getline_async(std::string& str,
                   std::chrono::system_clock::duration timeout);
extern bool stop_getline_async;

// ----------------------------------------------------------------------------

