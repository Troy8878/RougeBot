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

  return (int) status;
}

// ----------------------------------------------------------------------------

size_t HttpResult::_GetContentLength()
{
  DWORD len;
  DWORD temp = sizeof(DWORD);

  WinHttpQueryHeaders(impl->handles.request,
                      WINHTTP_QUERY_CONTENT_LENGTH | WINHTTP_QUERY_FLAG_NUMBER,
                      nullptr, &len, &temp, nullptr);

  return (size_t) len;
}

// ----------------------------------------------------------------------------
