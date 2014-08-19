/*********************************
 * FixedWindows.h
 * Connor Hilarides
 * Created 2014/05/28
 *********************************/

// This file is for including windows.h and fixing some of the things it
// does, but it's also the place for misfit helpers that have nowhere
// else to go.

#pragma once

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#include <wincodec.h>
#include <comdef.h>
#include <locale>
#include <codecvt>
#include <string>

typedef _com_error COMError;

inline std::wstring widen(const std::string& narrow_string)
{
  std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
  return converter.from_bytes(narrow_string.c_str());
}

inline std::string narrow(const std::wstring& wide_string)
{
  std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
  return converter.to_bytes(wide_string);
}

#define CSTR_WIDEN(x) widen(x).c_str()
#define CSTR_NARROW(x) narrow(x).c_str()

#ifdef _DEBUG
#define IFDEBUG(x) x
#define IFNDEBUG(x) 
#else
#define IFDEBUG(x) 
#define IFNDEBUG(x) x
#endif

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


class DXFatalError : public std::exception
{
public:

  DXFatalError(HRESULT hr)
    : _error(hr)
  {
  }

  const char *what() const
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

class win32_exception : public string_exception
{
public:
  win32_exception()
    : string_exception(GetLastErrorString())
  {
  }
};

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

#define RELEASE_AFTER_SCOPE(itr) DXReleaser<std::remove_reference<decltype(*(itr))>::type> __##itr##__releaser{(itr)}


#define CHECK_HRESULT(hr) CheckHRESULT(hr)

inline void CheckHRESULT(HRESULT hr)
{
  if (FAILED(hr)) 
    throw DXFatalError(hr);
}

template <typename Interface>
void setDXDebugName(Interface *object, const std::wstring& name)
{
  setDXDebugName(object, narrow(name));
}

template <typename Interface>
void setDXDebugName(Interface *object, const std::string& name)
{
  HRESULT result = object->SetPrivateData(WKPDID_D3DDebugObjectName,
                                          (UINT) name.length(),
                                          name.c_str());
  CHECK_HRESULT(result);
}

template <typename Container>
void variadic_push_container(Container&)
{
}

template <typename Container, typename Arg>
void variadic_push_container(Container& container, const Arg& param)
{
  container.push_back(param);
}

template <typename Container, typename Arg, typename... Args>
void variadic_push_container(Container& containter, const Arg& param, const Args&&... params)
{
  container.push_back(param);
  variadic_push_container(container, params...);
}


#define strcmpi _strcmpi


#define NO_COPY_CONSTRUCTOR(type) type(const type&) = delete
#define NO_ASSIGNMENT_OPERATOR(type) type& operator=(const type&) = delete

#define _PROPERTY_GET(pType, pName)                 \
  virtual pType& _PropGet ## pName() {              \
    return this->_##pName;                          \
  }                                                 \
  virtual pType const & _PropGet ## pName() const { \
    return this->_##pName;                          \
  }

#define _PROPERTY_SET(pType, pName)              \
  virtual void _PropSet ## pName(pType value) {  \
    this->_##pName = value;                      \
  }

#define _IPROPERTY_GET(pType, pName)                 \
  inline pType& _PropGet ## pName() {              \
    return this->_##pName;                          \
  }                                                 \
  inline pType const & _PropGet ## pName() const { \
    return this->_##pName;                          \
  }

#define _IPROPERTY_SET(pType, pName)              \
  inline void _PropSet ## pName(pType value) {  \
    this->_##pName = value;                      \
  }

#define _PROPERTY_STORAGE(pType, pName) \
  private: pType _##pName; public:

#define W_PROPERTY(pType, pName)  \
  _PROPERTY_STORAGE(pType, pName) \
  private:                        \
  _PROPERTY_GET(pType, pName)     \
  public:                         \
  _PROPERTY_SET(pType, pName)     \
  __declspec(property(             \
    get = _PropGet ## pName,       \
    put = _PropSet ## pName)       \
  ) pType pName

#define R_PROPERTY(pType, pName)  \
  _PROPERTY_STORAGE(pType, pName) \
  _PROPERTY_GET(pType, pName)     \
  private:                        \
  _PROPERTY_SET(pType, pName)     \
  public:                         \
  __declspec(property(             \
    get = _PropGet ## pName,       \
    put = _PropSet ## pName)       \
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
