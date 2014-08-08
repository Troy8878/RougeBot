/*********************************
 * FixedWindows.h
 * Connor Hilarides
 * Created 2014/05/28
 *********************************/

#pragma once

#define WIN32_LEAN_AND_MEAN
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

#undef min
#undef max

#ifdef _DEBUG
#define IFDEBUG(x) x
#else
#define IFDEBUG(x) 
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

template <class Interface>
/** Safely releases DirectX interfaces */
inline void releaseDXInterface(Interface * &interfaceToRelease)
{
  if (interfaceToRelease)
  {
    interfaceToRelease->Release();
    interfaceToRelease = nullptr;
  }
}

#define PROTECTED_ACCESSIBLE(type, name) \
  type _##name = nullptr; public: \
  inline type const& name() { return _##name; } protected:

#define CHECK_HRESULT(hr) if (FAILED(hr)) { throw DXFatalError(hr); }

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
