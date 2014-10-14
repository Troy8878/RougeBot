/*********************************
 * HttpClient.cpp
 * Connor Hilarides
 * Created 2014/10/14
 *********************************/

#include "Common.h"
#include "HttpClientImpl.h"

#pragma comment(lib, "Winhttp.lib")

// ----------------------------------------------------------------------------

#define HTTP_RAISEEX(msg) \
  { throw string_exception(msg "\n" + GetLastErrorString()); }

static const std::unordered_map<std::string, int> defaultPorts =
{
  {"http", 80},
  {"https", 443}
};

// ----------------------------------------------------------------------------

HttpClient::HttpClient()
{
  impl = std::make_shared<HttpClientImpl>();
}

// ----------------------------------------------------------------------------

HttpClientImpl::HttpClientImpl()
{
  handles.session = 
    WinHttpOpen(L"Pancake Engine/1.0",
                WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
                WINHTTP_NO_PROXY_NAME,
                WINHTTP_NO_PROXY_BYPASS, 0);

  if (!handles.session)
    HTTP_RAISEEX("Failed to initialize WinHTTP session");
}

// ----------------------------------------------------------------------------

HttpClientImpl::~HttpClientImpl()
{
  WinHttpCloseHandle(handles.session);
}

// ----------------------------------------------------------------------------

HttpResult HttpClient::MakeRequest(const HttpRequest& request)
{
  if (request.Body.impl->bodyType == HttpRequestBodyImpl::BODY_EMPTY)
  {
    return impl->PerformEmpty(request);
  }

  throw basic_exception("Unknown HTTP Body Type");
}

// ----------------------------------------------------------------------------

#pragma region HttpClient Properties

DWORD HttpClient::_GetTimeout()
{
  DWORD timeout;
  DWORD size;

  if (!WinHttpQueryOption(impl->handles.session,
                          WINHTTP_OPTION_CONNECT_TIMEOUT,
                          &timeout, &size))
    HTTP_RAISEEX("Failed to get HTTP Timeout");
  return timeout;
}

// ----------------------------------------------------------------------------

void HttpClient::_SetTimeout(DWORD val)
{
  if (!WinHttpSetOption(impl->handles.session,
                        WINHTTP_OPTION_CONNECT_TIMEOUT,
                        &val, sizeof(DWORD)))
    HTTP_RAISEEX("Failed to set HTTP Timeout");
}

#pragma endregion

// ----------------------------------------------------------------------------

#pragma region Requests

void HttpClientImpl::AsyncPerformEmpty(HttpClientImpl& impl, 
                                       const HttpRequest& request, 
                                       HttpResultImpl *res)
{
  auto server = widen(request.Uri.Host);
  INTERNET_PORT port = 0;

  if (request.Uri.HasPort)
    port = (INTERNET_PORT) request.Uri.Port;
  else
  {
    auto def = defaultPorts.find(request.Uri.Scheme);
    if (def == defaultPorts.end())
      throw string_exception("Unknown port for URI `" + 
                             request.Uri.Build() +
                             "`, please specify one.");
    port = (INTERNET_PORT) def->second;
  }
  
  auto& h = res->handles;

  h.server = WinHttpConnect(impl.handles.session, server.c_str(), port, 0);
  if (!h.server)
  { 
    res->failMsg = "Failed to connect to server";
    res->hasFailed = true;
    return;
  }
}

HttpResult HttpClientImpl::PerformEmpty(const HttpRequest& request)
{
  HttpResult result;
  result.impl = HttpShared<HttpResultImpl>(
    new HttpResultImpl(
      std::bind(HttpClientImpl::AsyncPerformEmpty,
                *this, request, result.impl.get())));

  return result;
}

#pragma endregion

// ----------------------------------------------------------------------------
