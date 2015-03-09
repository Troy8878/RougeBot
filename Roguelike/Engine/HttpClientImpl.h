/*********************************
 * HttpClientImpl.h
 * Connor Hilarides
 * Created 2014/10/14
 * Copyright © 2014 DigiPen Institute of Technology, All Rights Reserved
 *********************************/

#pragma once

#include "HttpClient.h"

// ----------------------------------------------------------------------------

class HttpRequestImpl
{
public:
  HttpRequestImpl(const HttpUri &uri, HttpMethod method);
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
  void Start(ThreadFunc &&func)
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
  HttpShared<std::string> data;
  json::value jvalue;

  bool hasJson = false;

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

  HttpResult PerformEmpty(const HttpRequest &request);
  HttpResult PerformBody(const HttpRequest &request);

  static void AsyncWriteData(HttpClient client, HttpRequest request, HttpResult result);

  static void AsyncBeginRequest(HttpClient client, HttpRequest request, HttpResult result);
  static void AsyncCompleteRequest(HttpClient client, HttpResult result);

  static void AsyncPerformEmpty(HttpClient client, HttpRequest request, HttpResult result);
  static void AsyncPerformBody(HttpClient client, HttpRequest request, HttpResult result);
};

// ----------------------------------------------------------------------------

class HttpRequestBuffer : public std::streambuf
{
  static const int buffer_size = 16384;

public:
  HttpRequestBuffer(HINTERNET request)
    : request(request), temp_buf(internal_buffer + 6)
  {
  }

  ~HttpRequestBuffer()
  {
    if (error)
      return;

    WriteBuffer();
    WriteBuffer(true);
  }

  DWORD error = 0;

  NO_ASSIGNMENT_OPERATOR(HttpRequestBuffer);

protected:
  virtual int_type overflow(int_type c) override
  {
    if (error)
      return c;

    if (c == EOF)
      return c;

    if (buf_count == buffer_size)
      WriteBuffer();

    temp_buf[buf_count++] = static_cast<char>(c);

    return c;
  }

  void WriteBuffer(bool writeEmpty = false)
  {
    if (error)
      return;

    DWORD written = 0;
    BOOL res;

    if (!buf_count && !writeEmpty)
      return;

    char size[8];
    sprintf_s(size, "%x\r\n", (unsigned)buf_count);
    DWORD buf_off = 6 - (DWORD)strlen(size);

    memcpy_s(internal_buffer + buf_off, sizeof(internal_buffer) - buf_off, size, 6 - buf_off);

    temp_buf[buf_count + 0] = '\r';
    temp_buf[buf_count + 1] = '\n';

    res = WinHttpWriteData(request, internal_buffer + buf_off, (DWORD)buf_count + 8 - buf_off, &written);
    if (!res)
    {
      error = GetLastError();
      return;
    }

    buf_count = 0;
  }

private:
  char internal_buffer[buffer_size + 8];
  HINTERNET request;
  char * const temp_buf;
  size_t buf_count = 0;
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
    BODY_DATA
  } bodyType;

  std::string contentType;
  HttpRequestBody::WriteFunc func = [](const std::ostream &)
  {
  };
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
  HttpResultStreamImpl()
    : data(&databuf)
  {
  }

  ibufferstream<char> databuf;
  std::istream data;
};

// ----------------------------------------------------------------------------