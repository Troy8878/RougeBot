/*********************************
 * HttpRequest.cpp
 * Connor Hilarides
 * Created 2014/10/14
 * Copyright © 2014 DigiPen Institute of Technology, All Rights Reserved
 *********************************/

#include "Common.h"
#include "HttpClientImpl.h"

// ----------------------------------------------------------------------------

HttpRequest::HttpRequest(const HttpUri &uri, HttpMethod method)
{
  impl = std::make_shared<HttpRequestImpl>(uri, method);
}

// ----------------------------------------------------------------------------

HttpRequestImpl::HttpRequestImpl(const HttpUri &uri, HttpMethod method)
  : uri(uri), method(method)
{
}

// ----------------------------------------------------------------------------

HttpRequestImpl::~HttpRequestImpl()
{
}

// ----------------------------------------------------------------------------

HttpMethod HttpRequest::_GetMethod() const
{
  return impl->method;
}

// ----------------------------------------------------------------------------

void HttpRequest::_SetMethod(HttpMethod method)
{
  impl->method = method;
}

// ----------------------------------------------------------------------------

HttpUri &HttpRequest::_GetUri()
{
  return impl->uri;
}

// ----------------------------------------------------------------------------

const HttpUri &HttpRequest::_GetUri() const
{
  return impl->uri;
}

// ----------------------------------------------------------------------------

HttpRequestBody HttpRequest::_GetBody() const
{
  return impl->body;
}

// ----------------------------------------------------------------------------

HttpHeaderCollection HttpRequest::_GetHeaders() const
{
  return impl->headers;
}

// ----------------------------------------------------------------------------

void HttpRequestBody::SetJson(json::value jvalue)
{
  impl->bodyType = HttpRequestBodyImpl::BODY_DATA;
  SetContentType("application/json");
  WriteStream([jvalue](std::ostream &out)
    {
      jvalue.serialize(out);
    });
}

// ----------------------------------------------------------------------------

void HttpRequestBody::SetString(const std::string &str)
{
  auto shared = std::make_shared<std::string>(str);
  SetString(shared);
}

// ----------------------------------------------------------------------------

void HttpRequestBody::SetString(HttpShared<std::string> str)
{
  impl->bodyType = HttpRequestBodyImpl::BODY_DATA;
  WriteStream([str](std::ostream &out)
    {
      out << str;
    });
}

// ----------------------------------------------------------------------------

void HttpRequestBody::SetForm(
  const std::unordered_map<std::string, std::string> &data)
{
  auto shared = std::make_shared<const std::unordered_map<
    std::string, std::string>>(data);

  impl->bodyType = HttpRequestBodyImpl::BODY_DATA;
  SetContentType("application/x-www-form-urlencoded");
  WriteStream([shared](std::ostream &out)
    {
      bool first = true;
      for (auto &pair : *shared)
      {
        if (first)
          first = false;
        else
          out << "&";

        out << pair.first;
        out << "=";
        out << HttpURLEscape(pair.second);
      }
    });
}

// ----------------------------------------------------------------------------

void HttpRequestBody::SetContentType(const std::string &type)
{
  impl->contentType = type;
}

// ----------------------------------------------------------------------------

void HttpRequestBody::WriteStream(WriteFunc func)
{
  impl->func = func;
}

// ----------------------------------------------------------------------------