/*********************************
 * HttpClientImpl.h
 * Connor Hilarides
 * Created 2014/10/14
 *********************************/

#pragma once

#include "HttpClient.h"

// ----------------------------------------------------------------------------

class HttpRequestImpl
{
public:
  HttpRequestImpl(const HttpUri& uri, HttpMethod method);
  ~HttpRequestImpl();

  HttpUri uri;
  HttpMethod method;

  HttpRequestBody body;
  HttpHeaderCollection headers;
};

// ----------------------------------------------------------------------------

class HttpResultImpl
{
public:
  HttpResultImpl()
  {
    _HasFailed = false;
    _HasHeaders = false;
    _HasData = false;
  }

  ~HttpResultImpl();
  
  template <typename ThreadFunc>
  void Start(ThreadFunc&& func)
  {
    thread = std::thread(func);
    thread.detach();
  }
  
  mutable critical_section lock;
  std::thread thread;

  SRW_PROPERTY(bool, HasFailed);
  SRW_PROPERTY(bool, HasHeaders);
  SRW_PROPERTY(bool, HasData);

  SRW_PROPERTY(std::string, FailMsg);

  struct
  {
    HINTERNET server = nullptr;
    HINTERNET request = nullptr;

  } handles;

  HttpHeaderCollection headers;
  shared_array<byte> data;
  json::value jvalue;

  void LoadResult();

  NO_COPY_CONSTRUCTOR(HttpResultImpl);
  NO_ASSIGNMENT_OPERATOR(HttpResultImpl);
};

// ----------------------------------------------------------------------------

class HttpClientImpl
{
public:
  HttpClientImpl();
  ~HttpClientImpl();

  std::weak_ptr<HttpClientImpl> self_ref;

  struct
  {
    HINTERNET session;
  } handles;

  HttpResult PerformEmpty(const HttpRequest& request);
  
  static void AsyncBeginRequest(HttpClient client, const HttpRequest& request, HttpResult result);
  static void AsyncCompleteRequest(HttpClient client, HttpResult result);

  static void AsyncPerformEmpty(HttpClient client, const HttpRequest& request, HttpResult result);
};

// ----------------------------------------------------------------------------

class HttpRequestBodyImpl
{
public:
  HttpRequestBodyImpl();
  ~HttpRequestBodyImpl();

  enum BodyType
  {
    BODY_EMPTY,
    BODY_STRING,
    BODY_JSON,
    BODY_FORM,
    BODY_MULTIPART

  } bodyType;

  union
  {
    std::string *str;
    json::value *json;

    // TODO: Make some kind of form multipart wrapper

  } body;
};

// ----------------------------------------------------------------------------

class HttpHeaderCollectionImpl
{
public:
  std::unordered_map<std::string, std::vector<HttpHeaderEntry>> headers;
};

// ----------------------------------------------------------------------------

class HttpResultStreamImpl
{
public:
  critical_section lock;

  std::ostringstream data;
  size_t read_pos;
};

// ----------------------------------------------------------------------------
