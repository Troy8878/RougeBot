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

struct PropertyServerInternal
{
  HANDLE staticQueue;
  HANDLE entityQueue;
  HANDLE globalQueue;
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

static void RegisterStaticHandler(PropertyServerInternal& data);
static void RegisterEntityHandler(PropertyServerInternal& data);
static void RegisterGlobalHandler(PropertyServerInternal& data);

// ----------------------------------------------------------------------------

PropertyServer::PropertyServer()
  : data(new PropertyServerInternal)
{
  InitializeServer(*data);
}

// ----------------------------------------------------------------------------

PropertyServer::~PropertyServer()
{
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
}

// ----------------------------------------------------------------------------

static void CloseServer(PropertyServerInternal& data)
{
  (data);
}

// ----------------------------------------------------------------------------

static void RegisterStaticHandler(PropertyServerInternal& data)
{
  DWORD res;

  res = HttpCreateHttpHandle(&data.staticQueue, 0);
  CheckHTTPResult(res);

  res = HttpAddUrl(data.staticQueue, L"http://localhost:7523/", nullptr);
  CheckHTTPResult(res);
}

// ----------------------------------------------------------------------------

static void RegisterEntityHandler(PropertyServerInternal& data)
{
  DWORD res;

  res = HttpCreateHttpHandle(&data.entityQueue, 0);
  CheckHTTPResult(res);

  res = HttpAddUrl(data.entityQueue, L"http://localhost:7523/api/entity/", nullptr);
  CheckHTTPResult(res);
}

// ----------------------------------------------------------------------------

static void RegisterGlobalHandler(PropertyServerInternal& data)
{
  DWORD res;

  res = HttpCreateHttpHandle(&data.globalQueue, 0);
  CheckHTTPResult(res);

  res = HttpAddUrl(data.globalQueue, L"http://localhost:7523/api/game/", nullptr);
  CheckHTTPResult(res);
}

// ----------------------------------------------------------------------------

