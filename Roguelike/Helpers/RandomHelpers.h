/*********************************
 * RandomHelpers.h
 * Connor Hilarides
 * Created 2014/08/20
 *********************************/

#pragma once

#include "FixedWindows.h"
#include "StackTrace.h"
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

template <typename Elem>
inline std::basic_string<Elem> chomp(std::basic_string<Elem> str)
{
  if (str.empty())
    return str;

  std::locale loc("en-US");
  size_t pos = 0;

  while (pos < str.size() && std::isspace(str[pos], loc))
    pos++;

  str = str.substr(pos);

  while (!str.empty() && std::isspace(*str.rbegin(), loc))
    str.pop_back();

  return str;
}

// ----------------------------------------------------------------------------

template <typename Elem, typename Delim>
inline std::vector<std::basic_string<Elem>> 
split(const std::basic_string<Elem>& str, Delim&& delim)
{
  std::vector<std::basic_string<Elem>> items;
  size_t pos = 0, next;

  while ((next = str.find_first_of(delim, pos)) != str.npos)
  {
    items.push_back(str.substr(pos, next - pos));

    if (next != str.npos)
      pos = str.find_first_not_of(delim, next);
  }

  items.push_back(str.substr(pos));

  return items;
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
inline std::string GetLastErrorString(DWORD error = 0)
{
  LPSTR pBuffer = 0;

  if (error == 0)
    error = GetLastError();

  FormatMessage(
    FORMAT_MESSAGE_ALLOCATE_BUFFER |
    FORMAT_MESSAGE_FROM_SYSTEM |
    FORMAT_MESSAGE_IGNORE_INSERTS,
    NULL,
    error,
    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
    (LPTSTR) &pBuffer,
    0, NULL);

  if (pBuffer == 0)
    return "UNKNOWN ERROR";

  std::string result = pBuffer;
  LocalFree(pBuffer);

  return result;
}

// ----------------------------------------------------------------------------

class basic_exception : public std::exception
{
  stack_trace trace;

public:
  basic_exception()
    : trace(stack_trace::create_trace())
  {
  }

  basic_exception(const char *what)
    : std::exception(what), trace(stack_trace::create_trace())
  {
  }

  void print_trace(std::ostream& out) const
  {
    trace.print(out);
  }
};

// ----------------------------------------------------------------------------

class DXFatalError : public basic_exception
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

class string_exception : public basic_exception
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

#define _SPROPERTY_GET(pType, pName)               \
  inline pType& _PropGet ## pName() {              \
    critical_section::guard guard(this->lock);     \
    return this->_##pName;                         \
  }                                                \
  inline pType const & _PropGet ## pName() const { \
    critical_section::guard guard(this->lock);     \
    return this->_##pName;                         \
  }

#define _SPROPERTY_SET(pType, pName)                  \
  inline void _PropSet ## pName(pType const& value) { \
    critical_section::guard guard(this->lock);        \
    this->_##pName = value;                           \
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

#define SRW_PROPERTY(pType, pName) \
  _PROPERTY_STORAGE(pType, pName)  \
  _SPROPERTY_GET(pType, pName)     \
  _SPROPERTY_SET(pType, pName)     \
  __declspec(property(             \
    get = _PropGet ## pName,       \
    put = _PropSet ## pName)       \
  ) pType pName

// ----------------------------------------------------------------------------

#define PROPERTY(...) __declspec(property(__VA_ARGS__))

// ----------------------------------------------------------------------------

#define COMPLEX_TYPE_IN_PRIMITIVE(type, name) \
  __declspec(align(8)) byte __data_##name [sizeof(type)]; \
  inline type & __DataGet_##name () { return *reinterpret_cast<type *>(__data_##name); } \
  PROPERTY(get = __DataGet_##name) type & name

// ----------------------------------------------------------------------------

struct component_factory_data : public std::unordered_map<std::string, json::value>
{
  typedef std::unordered_map<std::string, json::value> base;

  component_factory_data() = default;
  component_factory_data(const base& b) 
    : base(b) 
  {
  }
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
  ibufferstream() = default;

  ibufferstream(const CharT *start, size_t size)
  {
    CharT *s = const_cast<CharT *>(start);
    setg(s, s, s + size);
  }
};

// ----------------------------------------------------------------------------

template <typename Map, typename DefType>
typename Map::mapped_type
map_fetch(const Map& map,
          const typename Map::key_type& key,
          const DefType& def)
{
  auto it = map.find(key);
  if (it != map.end())
    return it->second;

  return static_cast<typename Map::mapped_type>(def);
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

namespace math { class Vector; class Vector2D; }
struct Camera;

math::Vector __vectorcall ScreenToPlane(DirectX::FXMVECTOR point, 
                                        DirectX::FXMVECTOR planeOrigin, 
                                        DirectX::FXMVECTOR planeNormal, 
                                        Camera *camera, float *distance = nullptr);

// ----------------------------------------------------------------------------

D2D1::ColorF StringToColor(const std::string& name);

// ----------------------------------------------------------------------------

class DebugMessage
{
public:
#ifdef _DEBUG
  inline DebugMessage(const std::string& str)
  {
    std::cerr << str << std::endl;
  }
#else
  template <typename... A>
  inline DebugMessage(A...) {}
#endif
};

#define ONE_TIME_MESSAGE(msg) static DebugMessage _________one_time_message(msg)

// ----------------------------------------------------------------------------

#define RUN_ONCE(...) {             \
  static bool ___runonce_init = false; \
  if (!___runonce_init)                \
  {                                    \
    (__VA_ARGS__); ___runonce_init = true;     \
  } }

// ----------------------------------------------------------------------------

template <typename CharType, typename Traits = std::char_traits<CharType>, 
          typename Alloc = std::allocator<CharType>>
inline std::basic_string<CharType, Traits, Alloc> downcase(
  const std::basic_string<CharType, Traits, Alloc>& input)
{
  std::basic_string<CharType, Traits, Alloc> copy = input;
  std::transform(copy.begin(), copy.end(), copy.begin(), ::tolower);
  return copy;
}

template <typename CharType, typename Traits = std::char_traits<CharType>, 
          typename Alloc = std::allocator<CharType>>
inline std::basic_string<CharType, Traits, Alloc> downcase(
  const CharType *str)
{
  std::basic_string<CharType, Traits, Alloc> copy = str;
  std::transform(copy.begin(), copy.end(), copy.begin(), ::tolower);
  return copy;
}

// ----------------------------------------------------------------------------

template <typename T>
struct array_iterator_t
{
  array_iterator_t(T *a, size_t c) : c(c), a(a) {}
  size_t c;
  T *a;

  T *begin() { return a; }
  T *end() { return a + c; }
};

template <typename T>
array_iterator_t<T> array_iterator(T *a, size_t c)
{
  return array_iterator_t<T>(a, c);
}

template <typename T>
array_iterator_t<const T> const_array_iterator(const T *a, size_t c)
{
  return array_iterator_t<const T>(a, c);
}

// ----------------------------------------------------------------------------

template <typename Cont, typename Iter, typename Key>
bool map_get_check(const Cont& container, Iter& iter, Key& key)
{
  auto it = container.find(key);
  if (it != container.end())
  {
    iter = it;
    return true;
  }

  return false;
}

// ----------------------------------------------------------------------------

