/*********************************
 * PropertyServer.cpp
 * Connor Hilarides
 * Created 2014/10/27
 *********************************/

#include "PropertyServer.h"
#include "Engine/Common.h"
#include "Engine/Level.h"

#include <http.h>

// ----------------------------------------------------------------------------

#define STATIC_HANDLER_URL L"http://localhost:5431/game/"
#define ENTITY_HANDLER_URL (STATIC_HANDLER_URL L"api/entity")
#define GLOBAL_HANDLER_URL (STATIC_HANDLER_URL L"api/game")

// ----------------------------------------------------------------------------

struct PropertyServerInternal
{
  HANDLE staticQueue = nullptr;
  HANDLE entityQueue = nullptr;
  HANDLE globalQueue = nullptr;

  COMPLEX_TYPE_IN_PRIMITIVE(std::thread, requestThread);
};

// ----------------------------------------------------------------------------

static inline void CheckHTTPResult(DWORD res)
{
  auto hr = HRESULT_FROM_WIN32(res);
  CheckHRESULT(hr);
}

// ----------------------------------------------------------------------------

static void InitializeServer(PropertyServerInternal& data);
static void CloseServer(PropertyServerInternal& data);

// ----------------------------------------------------------------------------

static void RegisterStaticHandler(PropertyServerInternal& data);
static void RegisterEntityHandler(PropertyServerInternal& data);
static void RegisterGlobalHandler(PropertyServerInternal& data);

static void CloseStaticHandler(PropertyServerInternal& data);
static void CloseEntityHandler(PropertyServerInternal& data);
static void CloseGlobalHandler(PropertyServerInternal& data);

// ----------------------------------------------------------------------------

static void RecieveRequests(PropertyServerInternal& data);

// ----------------------------------------------------------------------------

PropertyServer::PropertyServer()
  : data(new PropertyServerInternal)
{
  try
  {
    InitializeServer(*data);
  }
  catch (...)
  {
    CloseServer(*data);
    delete data;
    data = nullptr;
    throw;
  }
}

// ----------------------------------------------------------------------------

PropertyServer::~PropertyServer()
{
  data->requestThread.~thread();

  if (data)
    CloseServer(*data);
  delete data;
}

// ----------------------------------------------------------------------------

static void InitializeServer(PropertyServerInternal& data)
{
  DWORD res;

  static bool httpIsInit = false;
  if (!httpIsInit)
  {
    res = HttpInitialize(HTTPAPI_VERSION_2, HTTP_INITIALIZE_SERVER, nullptr);
    CheckHTTPResult(res);
    httpIsInit = true;
  }

  RegisterStaticHandler(data);
  RegisterEntityHandler(data);
  RegisterGlobalHandler(data);

  new (&data.requestThread) std::thread(std::bind(RecieveRequests, data));
  data.requestThread.detach();
}

// ----------------------------------------------------------------------------

static void CloseServer(PropertyServerInternal& data)
{
  CloseGlobalHandler(data);
  CloseEntityHandler(data);
  CloseStaticHandler(data);
}

// ----------------------------------------------------------------------------

static void RegisterStaticHandler(PropertyServerInternal& data)
{
  DWORD res;

  res = HttpCreateHttpHandle(&data.staticQueue, 0);
  CheckHTTPResult(res);

  res = HttpAddUrl(data.staticQueue, STATIC_HANDLER_URL, nullptr);
  CheckHTTPResult(res);
}

// ----------------------------------------------------------------------------

static void CloseStaticHandler(PropertyServerInternal& data)
{
  if (!data.staticQueue)
    return;

  HttpRemoveUrl(data.staticQueue, STATIC_HANDLER_URL);
  CloseHandle(data.staticQueue);

  data.staticQueue = nullptr;
}

// ----------------------------------------------------------------------------

static void RegisterEntityHandler(PropertyServerInternal& data)
{
  DWORD res;

  res = HttpCreateHttpHandle(&data.entityQueue, 0);
  CheckHTTPResult(res);

  res = HttpAddUrl(data.entityQueue, ENTITY_HANDLER_URL, nullptr);
  CheckHTTPResult(res);
}

// ----------------------------------------------------------------------------

static void CloseEntityHandler(PropertyServerInternal& data)
{
  if (!data.entityQueue)
    return;

  HttpRemoveUrl(data.entityQueue, ENTITY_HANDLER_URL);
  CloseHandle(data.entityQueue);

  data.entityQueue = nullptr;
}

// ----------------------------------------------------------------------------

static void RegisterGlobalHandler(PropertyServerInternal& data)
{
  DWORD res;

  res = HttpCreateHttpHandle(&data.globalQueue, 0);
  CheckHTTPResult(res);

  res = HttpAddUrl(data.globalQueue, GLOBAL_HANDLER_URL, nullptr);
  CheckHTTPResult(res);
}

// ----------------------------------------------------------------------------

static void CloseGlobalHandler(PropertyServerInternal& data)
{
  if (!data.globalQueue)
    return;

  HttpRemoveUrl(data.globalQueue, GLOBAL_HANDLER_URL);
  CloseHandle(data.globalQueue);

  data.globalQueue = nullptr;
}

// ----------------------------------------------------------------------------

static void RecieveRequests(PropertyServerInternal& data)
{
  (data);
}

// ----------------------------------------------------------------------------

