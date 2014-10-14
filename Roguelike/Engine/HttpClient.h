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
class HttpHeaderEntry;
class HttpHeaderCollectionImpl;

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

static inline const char *HttpMethodString(HttpMethod method)
{
  switch (method)
  {
    case HTTP_GET:
      return "GET";
    case HTTP_HEAD:
      return "HEAD";
    case HTTP_POST:
      return "POST";
    case HTTP_PUT:
      return "PUT";
    case HTTP_DELETE:
      return "DELETE";
    case HTTP_OPTIONS:
      return "OPTIONS";
    case HTTP_PATCH:
      return "PATCH";
  }
}

// ----------------------------------------------------------------------------

class HttpUri
{
public:
  HttpUri();
  HttpUri(const std::string& uri);

  static HttpUri Parse(const std::string& uri);
  std::string Build() const;

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
  friend class HttpClientImpl;

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
  PROPERTY(get = _GetHasHeaders)
    /**
      Checks whether enough of the HTTP data has
      been loaded to decipher the headers.
    */
    bool HasHeaders;

  PROPERTY(get = _GetIsLoaded)
    /**
      Checks whether Preload has finished on this
      HTTP result, if it had been initiated.
    */
    bool IsLoaded;


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

  PROPERTY(get = _GetStatusCode)
    /**
      Returns the web service status code.

      This operation is constant time if the headers have been
      loaded. It will take longer if the headers are not loaded.
    */
    int StatusCode;

private:
  HttpResult();

  HttpShared<HttpResultImpl> impl;
  friend class HttpClientImpl;

public:
  bool _HasHeaders();
  bool _GetIsLoaded();

  HttpResultStream _GetAsStream();
  HttpShared<std::string> _GetAsString();
  json::value _GetAsJson();

  int _GetStatusCode();
};

// ----------------------------------------------------------------------------

class HttpClient
{
public:
  HttpClient();

  PROPERTY(get = _GetTimeout, put = _SetTimeout) DWORD Timeout;

  HttpResult MakeRequest(const HttpRequest& request);

private:
  HttpShared<HttpClientImpl> impl;

public:
  DWORD _GetTimeout();
  void _SetTimeout(DWORD val);
};

// ----------------------------------------------------------------------------

class HttpResultStream
{
public:
  PROPERTY(get = _GetStream) std::istream& Stream;
  inline operator std::istream&() { return Stream; }

private:
  HttpResultStream();

  HttpShared<HttpResultStreamImpl> impl;
  friend class HttpResultImpl;

public:
  std::istream& _GetStream();
};

// ----------------------------------------------------------------------------

class HttpRequestBody
{
public:
  
private:
  HttpRequestBody();

  HttpShared<HttpRequestBodyImpl> impl;
  friend class HttpClient;
  friend class HttpRequestImpl;
};

// ----------------------------------------------------------------------------

class HttpHeaderCollection
{
public:

private:
  HttpHeaderCollection();

  HttpShared<HttpHeaderCollectionImpl> impl;
  friend class HttpRequestImpl;
  friend class HttpResultImpl;
};

// ----------------------------------------------------------------------------
