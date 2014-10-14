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
  if (handles.server)
    WinHttpCloseHandle(handles.server);
}

// ----------------------------------------------------------------------------
