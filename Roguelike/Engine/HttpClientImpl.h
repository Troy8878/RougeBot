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
  template <typename ThreadFunc>
  HttpResultImpl(ThreadFunc&& func)
    : thread(func)
  {
  }

  ~HttpResultImpl();

  std::thread thread;
  critical_section lock;

  bool hasFailed = false;
  bool hasHeaders = false;
  bool hasData = false;

  std::string failMsg;

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
  static void AsyncPerformEmpty(HttpClient client, const HttpRequest& request, HttpResultImpl *res);
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
  HttpHeaderCollectionImpl();
  ~HttpHeaderCollectionImpl();
};

// ----------------------------------------------------------------------------
