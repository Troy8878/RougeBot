/*********************************
 * HttpClient.h
 * Connor Hilarides
 * Created 2014/10/14
 *********************************/

#pragma once

#include "Common.h"

#include <winhttp.h>

// ----------------------------------------------------------------------------

template <typename T>
using HttpShared = std::shared_ptr<T>;
template <typename T>
using HttpUnique = std::unique_ptr<T>;
template <typename T>
class HttpOptional;

enum HttpMethod;

class HttpUri;

class HttpRequest;
class HttpResult;
class HttpClient;

class HttpResultStream;

// ----------------------------------------------------------------------------

class HttpRequestImpl;
class HttpResultImpl;
class HttpClientImpl;

class HttpResultStreamImpl;

// ----------------------------------------------------------------------------

class HttpRequestBody;
class HttpRequestBodyImpl;

class HttpHeaderCollection;
class HttpHeaderSet;
class HttpHeaderEntry;
class HttpHeaderCollectionImpl;

// ----------------------------------------------------------------------------

#define FRIEND_IMPLS                    \
  friend class HttpRequest;             \
  friend class HttpResult;              \
  friend class HttpClient;              \
  friend class HttpRequestImpl;         \
  friend class HttpResultImpl;          \
  friend class HttpClientImpl;          \
  friend class HttpResultStream;        \
  friend class HttpResultStreamImpl;    \
  friend class HttpRequestBodyImpl;     \
  friend class HttpHeaderCollection;    \
  friend class HttpHeaderCollectionImpl \

// ----------------------------------------------------------------------------

template <typename T>
class HttpOptional
{
public:
  HttpOptional()
    : IsSpecified(false)
  {
  }
  
  template <typename Arg1, typename... Args>
  HttpOptional(Arg1 arg1, Args... args)
    : IsSpecified(true), Value(arg1, args...)
  {
  }

  bool IsSpecified;
  T Value;
};

// ----------------------------------------------------------------------------

enum HttpMethod
{
  HTTP_GET,
  HTTP_HEAD,
  HTTP_POST,
  HTTP_PUT,
  HTTP_DELETE,
  HTTP_OPTIONS,
  HTTP_PATCH,
};

static inline const wchar_t *HttpMethodString(HttpMethod method)
{
  switch (method)
  {
    case HTTP_GET:
      return L"GET";
    case HTTP_HEAD:
      return L"HEAD";
    case HTTP_POST:
      return L"POST";
    case HTTP_PUT:
      return L"PUT";
    case HTTP_DELETE:
      return L"DELETE";
    case HTTP_OPTIONS:
      return L"OPTIONS";
    case HTTP_PATCH:
      return L"PATCH";
  }

  throw basic_exception("Unknown HTTP Method");
}

// ----------------------------------------------------------------------------

class HttpUri
{
public:
  HttpUri();
  HttpUri(const std::string& uri);

  static HttpUri Parse(std::string uri);
  std::string Build() const;
  std::string BuildPath() const;

  #define URI_PROP(type, name) \
    bool _PropHas##name() const { return (_##name).IsSpecified; } \
    PROPERTY(get = _PropHas##name) bool Has##name; \
    void Remove##name() { (_##name).IsSpecified = false; } \
    type const & _PropGet##name() const { return (_##name).Value; } \
    void _PropSet##name(type const& val) \
    { \
      (_##name).Value = val; \
      (_##name).IsSpecified = true; \
    } \
    private: HttpOptional<type> _##name; public: \
    PROPERTY(get = _PropGet##name, put = _PropSet##name) type name;

  URI_PROP(std::string, Scheme);
  URI_PROP(std::string, Username);
  URI_PROP(std::string, Password);
  URI_PROP(std::string, Host);
  URI_PROP(int,         Port);
  URI_PROP(std::string, Path);
  URI_PROP(std::string, Query);
  URI_PROP(std::string, Fragment);

  #undef URI_PROP
};

// ----------------------------------------------------------------------------

class HttpClient
{
public:
  HttpClient();
  HttpClient(HttpShared<HttpClientImpl> shared) : impl(shared) {}

  PROPERTY(get = _GetTimeout, put = _SetTimeout) DWORD Timeout;

  HttpResult MakeRequest(const HttpRequest& request);

private:
  HttpShared<HttpClientImpl> impl;
  FRIEND_IMPLS;

public:
  DWORD _GetTimeout();
  void _SetTimeout(DWORD val);
};

// ----------------------------------------------------------------------------

class HttpRequest
{
public:
  HttpRequest(const HttpUri& uri, HttpMethod method);

  PROPERTY(get = _GetMethod, put = _SetMethod) HttpMethod Method;
  PROPERTY(get = _GetUri) HttpUri& Uri;
  PROPERTY(get = _GetBody) HttpRequestBody Body;
  PROPERTY(get = _GetHeaders) HttpHeaderCollection Headers;

private:
  HttpShared<HttpRequestImpl> impl;
  FRIEND_IMPLS;

public:
  HttpMethod _GetMethod() const;
  void _SetMethod(HttpMethod method);

  HttpUri& _GetUri();
  const HttpUri& _GetUri() const;
  HttpRequestBody _GetBody() const;
  HttpHeaderCollection _GetHeaders() const;
};

// ----------------------------------------------------------------------------

class HttpResult
{
public:
  HttpResult(std::_Uninitialized) {}

  PROPERTY(get = _GetHasHeaders)
    /**
      Checks whether enough of the HTTP data has
      been loaded to decipher the headers.
    */
    bool HasHeaders;

  PROPERTY(get = _HasData)
    /**
      Checks whether Preload has finished on this
      HTTP result, if it had been initiated.
    */
    bool HasData;

  PROPERTY(get = _HasFailed)
    bool HasFailed;

  PROPERTY(get = _ErrorMsg)
    const std::string& ErrorMessage;


  PROPERTY(get = _GetAsStream)
    /**
      Returns the HTTP data in a way such that it can be
      read with functions that work on std::istream&.

      This operation is constant time if the data has been
      loaded. It will take longer if the data is not loaded.
    */
    HttpResultStream AsStream;

  PROPERTY(get = _GetAsString)
    /**
      Returns the HTTP data as a string of UTF-8 encoded data.
      The result will be an empty string if there was an error.

      This operation is constant time if the data has been
      loaded. It will take longer if the data is not loaded.
    */
    HttpShared<std::string> AsString;

  PROPERTY(get = _GetAsJson)
    /**
      Returns the HTTP data already parsed into a JSON format.
      The result will be a jnull if the data is malformed.

      This operation is constant time if the data has been
      loaded. It will take longer if the data is not loaded.
    */
    json::value AsJson;

  PROPERTY(get = _GetHeaders)
    /**
      Gets the header colection.
      Behavior undocumented if HasHeaders is false.
    */
    HttpHeaderCollection Headers;

  PROPERTY(get = _GetStatusCode)
    /**
      Returns the web service status code.
      Behavior undocumented if HasHeaders is false.
    */
    int StatusCode;

  PROPERTY(get = _GetContentLength)
    /**
      Returns the response content length.
      Behavior undocumented if HasHeaders is false.
    */
    size_t ContentLength;

private:
  HttpResult(HttpClient client);

  HttpShared<HttpResultImpl> impl;
  FRIEND_IMPLS;

  HttpClient client;

public:
  bool _HasHeaders();
  bool _HasData();
  bool _HasFailed();
  const std::string& _ErrorMsg();

  HttpHeaderCollection _GetHeaders();

  int _GetStatusCode();
  size_t _GetContentLength();

  HttpResultStream _GetAsStream();
  HttpShared<std::string> _GetAsString();
  json::value _GetAsJson();
};

// ----------------------------------------------------------------------------

class HttpResultStream
{
public:
  PROPERTY(get = _GetStream) std::istream& Stream;
  inline operator std::istream&() { return Stream; }

  void Reset();

private:
  HttpResultStream(HttpResult result);

  HttpShared<HttpResultStreamImpl> impl;
  FRIEND_IMPLS;

public:
  std::istream& _GetStream();
};

// ----------------------------------------------------------------------------

class HttpRequestBody
{
public:
  typedef std::function<void(std::ostream& out)> WriteFunc;

  void SetJson(json::value jvalue);
  void SetString(const std::string& str);
  void SetString(HttpShared<std::string> str);
  void SetForm(const std::unordered_map<std::string, std::string>& data);

  void SetContentType(const std::string& type);

  /**
    Delays writing data to a buffer.
    The callback will be called when HttpClient::MakeRequest
    is called, so use something that will be valid at that point.

    The function may be called multiple times if redirects happen.
  */
  void WriteStream(WriteFunc func);
  
private:
  HttpRequestBody();

  HttpShared<HttpRequestBodyImpl> impl;
  FRIEND_IMPLS;
};

// ----------------------------------------------------------------------------

class HttpHeaderCollection
{
  typedef std::unordered_map<std::string, std::vector<HttpHeaderEntry>> map;

public:
  static HttpHeaderCollection ParseHeaders(const WCHAR *str);

  HttpHeaderSet operator[](const std::string& key);
  const HttpHeaderSet operator[](const std::string& key) const;

  std::wstring BuildList() const;

  map::iterator begin();
  map::iterator end();

private:
  HttpHeaderCollection();

  HttpShared<HttpHeaderCollectionImpl> impl;
  FRIEND_IMPLS;
};

// ----------------------------------------------------------------------------

class HttpHeaderSet
{
  std::string key;
  std::vector<HttpHeaderEntry> *items;

public:
  HttpHeaderSet(const decltype(key)& key, decltype(items) items)
    : key(key), items(items)
  {
  }

  PROPERTY(get = _GetKey) const std::string& Key;

  void AddValue(const std::string& val);
  void RemoveValue(const std::string& val);
  void Clear();

  size_t Count() { return items->size(); }

  auto begin() -> decltype(items->begin()) { return items->begin(); }
  auto end() -> decltype(items->end()) { return items->end(); }

private:
  FRIEND_IMPLS;

public:
  inline const std::string& _GetKey() { return key; }
};

// ----------------------------------------------------------------------------

class HttpHeaderEntry
{
public:
  HttpHeaderEntry(const std::string& key, const std::string& value)
    : Key(key), Value(value)
  {
  }

  bool operator==(const HttpHeaderEntry& rhs) const
  {
    return Key == rhs.Key && Value == rhs.Value;
  }

  bool operator!=(const HttpHeaderEntry& rhs) const
  {
    return !(*this == rhs);
  }

  std::string Key, Value;
};

// ----------------------------------------------------------------------------

#undef FRIEND_IMPLS

// ----------------------------------------------------------------------------

static inline std::string HttpURLEscape(const std::string& unescaped, bool escapeSpace = true)
{
  static std::locale loc("en-US");

  std::wstringstream out;

  for (auto& c : widen(unescaped))
  {
    if (std::isalnum(c, loc) || (!escapeSpace && c == L' '))
      out << c;
    else
    {
      for (auto& uc : narrow({c}))
      {
        out << std::hex << L'%' << std::setw(2) << std::setfill(L'0') << ((unsigned)(byte)uc);
      }
    }
  }

  return narrow(out.str());
}

// ----------------------------------------------------------------------------

