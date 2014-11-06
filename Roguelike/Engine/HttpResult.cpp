/*********************************
 * HttpResult.cpp
 * Connor Hilarides
 * Created 2014/10/14
 *********************************/

#include "Common.h"
#include "HttpClientImpl.h"

// ----------------------------------------------------------------------------

HttpResult::HttpResult(HttpClient client)
  : client(client)
{
}

// ----------------------------------------------------------------------------

HttpResultImpl::~HttpResultImpl()
{
  if (handles.request)
    WinHttpCloseHandle(handles.request);
  if (handles.server)
    WinHttpCloseHandle(handles.server);
}

// ----------------------------------------------------------------------------

int HttpResult::_GetStatusCode()
{
  DWORD status;
  DWORD temp = sizeof(DWORD);

  WinHttpQueryHeaders(impl->handles.request,
                      WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,
                      nullptr, &status, &temp, nullptr);

  return static_cast<int>(status);
}

// ----------------------------------------------------------------------------

size_t HttpResult::_GetContentLength()
{
  DWORD len;
  DWORD temp = sizeof(DWORD);

  WinHttpQueryHeaders(impl->handles.request,
                      WINHTTP_QUERY_CONTENT_LENGTH | WINHTTP_QUERY_FLAG_NUMBER,
                      nullptr, &len, &temp, nullptr);

  return static_cast<size_t>(len);
}

// ----------------------------------------------------------------------------

bool HttpResult::_HasHeaders()
{
  return impl->HasHeaders;
}

// ----------------------------------------------------------------------------

bool HttpResult::_HasData()
{
  return impl->HasData;
}

// ----------------------------------------------------------------------------

bool HttpResult::_HasFailed()
{
  return impl->HasFailed;
}

// ----------------------------------------------------------------------------

const std::string &HttpResult::_ErrorMsg()
{
  return impl->FailMsg;
}

// ----------------------------------------------------------------------------

HttpHeaderCollection HttpResult::_GetHeaders()
{
  return impl->headers;
}

// ----------------------------------------------------------------------------

HttpResultStream HttpResult::_GetAsStream()
{
  return HttpResultStream(*this);
}

// ----------------------------------------------------------------------------

HttpShared<std::string> HttpResult::_GetAsString()
{
  return impl->data;
}

// ----------------------------------------------------------------------------

json::value HttpResult::_GetAsJson()
{
  if (!impl->hasJson)
  {
    impl->jvalue = json::value::parse(AsStream);
    impl->hasJson = true;
  }

  return impl->jvalue;
}

// ----------------------------------------------------------------------------

HttpResultStream::HttpResultStream(HttpResult result)
{
  auto data = result.impl->data;

  impl = std::make_shared<HttpResultStreamImpl>();
  impl->databuf = ibufferstream<char>(data->c_str(), data->size() + 1);
  impl->data.rdbuf(&impl->databuf);
}

// ----------------------------------------------------------------------------

void HttpResultStream::Reset()
{
  impl->data.seekg(0);
}

// ----------------------------------------------------------------------------

std::istream &HttpResultStream::_GetStream()
{
  return impl->data;
}

// ----------------------------------------------------------------------------