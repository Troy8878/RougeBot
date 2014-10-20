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
  impl->self_ref = impl;
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
  else if (request.Body.impl->bodyType == HttpRequestBodyImpl::BODY_DATA)
  {
    return impl->PerformBody(request);
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

void HttpClientImpl::AsyncWriteData(HttpClient client, 
                                    HttpRequest request, 
                                    HttpResult result)
{
  auto res = result.impl;
  
  HttpRequestBuffer buffer(res->handles.request);
  std::ostream stream(&buffer);

  request.Body.impl->func(stream);

  stream.flush();

  if (buffer.error)
  {
    if (buffer.error == ERROR_WINHTTP_RESEND_REQUEST)
    {
      AsyncWriteData(client, request, result);
    }
    else
    {
      res->FailMsg = GetLastErrorString(buffer.error);
      res->HasFailed = true;
    }
  }
}

void HttpClientImpl::AsyncBeginRequest(HttpClient client, 
                                       HttpRequest request, 
                                       HttpResult result)
{
  auto res = result.impl;

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

  // Connect to the server
  h.server = WinHttpConnect(client.impl->handles.session, server.c_str(), port, 0);
  if (!h.server)
  {
    res->FailMsg = "Failed to connect to server";
    res->HasFailed = true;
    return;
  }
  
  std::vector<std::wstring> wide_accepts;
  auto accepts_set = request.Headers["Accept"];

  LPCWSTR *accepts = WINHTTP_DEFAULT_ACCEPT_TYPES;
  if (accepts_set.Count())
  {
    for (auto& accept : accepts_set)
      wide_accepts.push_back(widen(accept.Value));

    accepts = new LPCWSTR[wide_accepts.size() + 1];
    for (size_t i = 0; i < wide_accepts.size(); ++i)
      accepts[i] = wide_accepts[i].c_str();
    accepts[wide_accepts.size()] = 0;
  }

  // Open the request
  auto wide_path = widen(request.Uri.BuildPath());

  h.request = WinHttpOpenRequest(h.server, HttpMethodString(request.Method),
                                 wide_path.c_str(), nullptr, 
                                 WINHTTP_NO_REFERER, accepts,
                                 request.Uri.Scheme == "https" 
                                   ? WINHTTP_FLAG_SECURE 
                                   : 0);
  if (!h.request)
  {
    res->FailMsg = "Failed to open request";
    res->HasFailed = true;
  }

  delete[] accepts;
}

// ----------------------------------------------------------------------------

void HttpClientImpl::AsyncCompleteRequest(HttpClient client, 
                                          HttpResult result)
{
  auto res = result.impl;
  auto& h = res->handles;

  BOOL results = WinHttpReceiveResponse(h.request, nullptr);
  WCHAR *buffer = nullptr;

  if (results)
  {
    DWORD dwSize;

    WinHttpQueryHeaders(h.request, WINHTTP_QUERY_RAW_HEADERS_CRLF,
                        WINHTTP_HEADER_NAME_BY_INDEX, nullptr,
                        &dwSize, WINHTTP_NO_HEADER_INDEX);

    if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
    {
      buffer = new WCHAR[dwSize / sizeof(WCHAR)];
      results = WinHttpQueryHeaders(h.request, WINHTTP_QUERY_RAW_HEADERS_CRLF,
                                    WINHTTP_HEADER_NAME_BY_INDEX,
                                    buffer, &dwSize, WINHTTP_NO_HEADER_INDEX);
    }
    else
    {
      results = false;
    }
  }

  std::ostringstream buf;

  if (results)
  {
    res->headers = HttpHeaderCollection::ParseHeaders(buffer);
    res->HasHeaders = true;

    // Now read in the actual data
    DWORD dwSize, dwRead;
    do
    {
      dwSize = 0;
      if (!WinHttpQueryDataAvailable(h.request, &dwSize))
      {
        results = false;
        break;
      }

      auto temp = new char[dwSize + 1];
      temp[dwSize] = 0;

      if (!WinHttpReadData(h.request, temp, dwSize, &dwRead))
      {
        delete[] temp;
        results = false;
        break;
      }

      temp[dwRead] = 0;
      buf << temp;
      delete[] temp;

    } while (dwSize);
  }

  delete[] buffer;

  if (results)
  {
    res->data = std::make_shared<std::string>(buf.str());
    res->HasData = true;
  }
  else
  {
    res->FailMsg = GetLastErrorString();
    res->HasFailed = true;
  }
}

// ----------------------------------------------------------------------------

void HttpClientImpl::AsyncPerformEmpty(HttpClient client, 
                                       HttpRequest request, 
                                       HttpResult result)
{
  auto res = result.impl;
  auto& h = res->handles;

  // Connect to the server and open the request
  AsyncBeginRequest(client, request, result);
  if (res->HasFailed)
    return;

  std::wstring headers = request.Headers.BuildList();

  // Send the request
  BOOL results = WinHttpSendRequest(h.request,
                                    headers.c_str(), 0,
                                    WINHTTP_NO_REQUEST_DATA, 0,
                                    0, 0);
  if (results)
  {
    AsyncCompleteRequest(client, result);
  }
  else
  {
    res->FailMsg = GetLastErrorString();
    res->HasFailed = true;
  }
}

// ----------------------------------------------------------------------------

void HttpClientImpl::AsyncPerformBody(HttpClient client, 
                                      HttpRequest request, 
                                      HttpResult result)
{
  auto res = result.impl;
  auto& h = res->handles;

  // I hate this, but it would be a mess to rework
  auto transfer = request.Headers["Transfer-Encoding"];
  transfer.Clear();
  transfer.AddValue("chunked");
  auto content = request.Headers["Content-Type"];
  content.Clear();
  content.AddValue(request.Body.impl->contentType);

  // Connect to the server and open the request
  AsyncBeginRequest(client, request, result);
  if (res->HasFailed)
    return;

  std::wstring headers = request.Headers.BuildList();

  // Send the request
  BOOL results = WinHttpSendRequest(h.request,
                                    headers.c_str(), 0,
                                    WINHTTP_NO_REQUEST_DATA, 0,
                                    0, 0);

  if (results)
  {
    AsyncWriteData(client, request, result);
    AsyncCompleteRequest(client, result);
  }
  else
  {
    res->FailMsg = GetLastErrorString();
    res->HasFailed = true;
  }
}

// ----------------------------------------------------------------------------

HttpResult HttpClientImpl::PerformEmpty(const HttpRequest& request)
{
  HttpResult result(self_ref.lock());

  result.impl = std::make_shared<HttpResultImpl>();
  result.impl->Start(
    std::bind(HttpClientImpl::AsyncPerformEmpty,
              self_ref.lock(), request, result));

  return result;
}

// ----------------------------------------------------------------------------

HttpResult HttpClientImpl::PerformBody(const HttpRequest& request)
{
  HttpResult result(self_ref.lock());

  result.impl = std::make_shared<HttpResultImpl>();
  result.impl->Start(
    std::bind(HttpClientImpl::AsyncPerformBody,
              self_ref.lock(), request, result));

  return result;
}

#pragma endregion

// ----------------------------------------------------------------------------
