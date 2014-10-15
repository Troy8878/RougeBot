/*********************************
 * HttpRequest.cpp
 * Connor Hilarides
 * Created 2014/10/14
 *********************************/

#include "Common.h"
#include "HttpClientImpl.h"

// ----------------------------------------------------------------------------

HttpRequest::HttpRequest(const HttpUri& uri, HttpMethod method)
{
  impl = std::make_shared<HttpRequestImpl>(uri, method);
}

// ----------------------------------------------------------------------------

HttpRequestImpl::HttpRequestImpl(const HttpUri& uri, HttpMethod method)
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

HttpUri& HttpRequest::_GetUri()
{
  return impl->uri;
}

// ----------------------------------------------------------------------------

const HttpUri& HttpRequest::_GetUri() const
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
