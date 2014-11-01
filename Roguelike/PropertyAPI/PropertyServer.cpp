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

#define MAX_REQUEST_SIZE (1024*1024) // 1MB is plenty. We aren't uploading images or anything...

// ----------------------------------------------------------------------------

class RequestBuffer;

struct RequestQueue
{
  HANDLE queue = nullptr;
  DWORD(*handler)(RequestQueue& queue);
  RequestBuffer *buffer;
  HTTP_REQUEST_ID reqId;

  inline HANDLE operator=(HANDLE h) { return queue = h; }
  inline operator HANDLE&() { return queue; }
  inline DWORD operator()() { return handler(*this); }
  inline HANDLE *operator&() { return &queue; }
};

// ----------------------------------------------------------------------------

struct PropertyServerInternal
{
  RequestQueue staticQueue;
  RequestQueue entityQueue;
  RequestQueue globalQueue;
  
  COMPLEX_TYPE_IN_PRIMITIVE(std::thread, staticThread);
  COMPLEX_TYPE_IN_PRIMITIVE(std::thread, entityThread);
  COMPLEX_TYPE_IN_PRIMITIVE(std::thread, globalThread);
};

// ----------------------------------------------------------------------------

typedef std::vector<std::string> UrlParts;

// ----------------------------------------------------------------------------

class RequestBuffer
{
public:
  RequestBuffer(ULONG initialSize)
    : buffer(0)
  {
    Resize(initialSize);
  }

  ~RequestBuffer()
  {
    delete[] buffer;
  }

  void Resize(ULONG size)
  {
    delete[] buffer;
    this->size = size;
    this->buffer = new CHAR[size];
  }

  void AddSize(ULONG size)
  {
    Resize(this->size + size);
  }

  void Zero()
  {
    ZeroMemory(buffer, size);
  }
  
  inline operator ULONG() { return size; }
  inline operator PCHAR() { return buffer; }
  inline operator PHTTP_REQUEST() { return (PHTTP_REQUEST) buffer; }

private:
  ULONG size;
  PCHAR buffer;
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

static void RecieveRequests(RequestQueue& queue);

static DWORD RecieveStatic(RequestQueue& queue);
static DWORD RecieveEntity(RequestQueue& queue);
static DWORD RecieveGlobal(RequestQueue& queue);

static DWORD ReplyNotFound(RequestQueue& queue);
static DWORD ReplyInternalError(RequestQueue& queue);

// ----------------------------------------------------------------------------

struct ResponseData
{
  USHORT code = 200;
  PCSTR reason = "OK";
  PCSTR contentType = "text/plain";
};

// ----------------------------------------------------------------------------

inline void SetKnownHeader(HTTP_RESPONSE& resp, HTTP_HEADER_ID id, PCSTR value)
{
  resp.Headers.KnownHeaders[id].pRawValue = value;
  resp.Headers.KnownHeaders[id].RawValueLength = (USHORT) strlen(value);
}

// ----------------------------------------------------------------------------

template <typename HeaderFunc>
static DWORD ReplyToRequest(RequestQueue& queue, const ResponseData& data, 
                            const void *buffer, ULONG bufferSize, HeaderFunc&& setHeaders)
{
  DWORD bytesSent;

  HTTP_RESPONSE response;
  ZeroMemory(&response, sizeof(response));
  response.StatusCode = data.code;
  response.pReason = data.reason;
  response.ReasonLength = (USHORT) strlen(data.reason);

  SetKnownHeader(response, HttpHeaderContentType, data.contentType);
  setHeaders(response);

  HTTP_DATA_CHUNK chunk;
  chunk.DataChunkType = HttpDataChunkFromMemory;
  chunk.FromMemory.pBuffer = (PVOID) buffer;
  chunk.FromMemory.BufferLength = bufferSize;

  response.EntityChunkCount = 1;
  response.pEntityChunks = &chunk;

  return HttpSendHttpResponse(
    queue,
    queue.reqId,
    0,
    &response,
    nullptr,
    &bytesSent,
    nullptr,
    0,
    nullptr,
    nullptr);
}

// ----------------------------------------------------------------------------

static DWORD ReplyToRequest(RequestQueue& queue, const ResponseData& data, 
                            const void *buffer, ULONG bufferSize)
{
  return ReplyToRequest(queue, data, buffer, bufferSize, [](HTTP_RESPONSE&){});
}

// ----------------------------------------------------------------------------

template <typename HeaderFunc>
static DWORD ReplyToRequestFile(RequestQueue& queue, const ResponseData& data, 
                                HANDLE file, HeaderFunc&& setHeaders)
{
  DWORD bytesSent;

  HTTP_RESPONSE response;
  ZeroMemory(&response, sizeof(response));
  response.StatusCode = data.code;
  response.pReason = data.reason;
  response.ReasonLength = (USHORT) strlen(data.reason);

  SetKnownHeader(response, HttpHeaderContentType, data.contentType);
  setHeaders(response);

  LARGE_INTEGER fileSize;
  GetFileSizeEx(file, &fileSize);

  HTTP_DATA_CHUNK chunk;
  chunk.DataChunkType = HttpDataChunkFromFileHandle;
  chunk.FromFileHandle.FileHandle = file;
  chunk.FromFileHandle.ByteRange.StartingOffset.QuadPart = 0;
  chunk.FromFileHandle.ByteRange.Length.QuadPart = (ULONGLONG) fileSize.QuadPart;

  response.EntityChunkCount = 1;
  response.pEntityChunks = &chunk;

  return HttpSendHttpResponse(
    queue,
    queue.reqId,
    0,
    &response,
    nullptr,
    &bytesSent,
    nullptr,
    0,
    nullptr,
    nullptr);
}

// ----------------------------------------------------------------------------

template <typename It>
static DWORD ReplyToRequest(RequestQueue& queue, const ResponseData& data, It begin, const It& end)
{
  std::vector<decltype(*begin)> databuffer;
  while (begin != end)
  {
    databuffer.push_back(*begin);
    ++begin;
  }

  ReplyToRequest(queue, data, databuffer.data(), databuffer.size() * sizeof(*begin));
}

// ----------------------------------------------------------------------------

static DWORD ReplyToRequest(RequestQueue& queue, const ResponseData& data, json::value value)
{
  auto reply = value.pretty_print();
  return ReplyToRequest(queue, data, reply.c_str(), (ULONG) reply.size());
}

// ----------------------------------------------------------------------------

std::string GetMimeType(const char *extension)
{
  static const char * const defaultType = "application/octet-stream";

  LSTATUS res;
  HKEY extClass;
  res = RegOpenKeyExA(HKEY_CLASSES_ROOT, extension, 0, KEY_READ, &extClass);
  if (res != ERROR_SUCCESS)
    return defaultType;

  BYTE value[128];
  DWORD dataSize = sizeof(value);
  
  DWORD dataType;
  res = RegQueryValueExA(extClass, "Content Type", 0, &dataType, value, &dataSize);

  RegCloseKey(extClass);
  if (res != ERROR_SUCCESS || dataType != REG_SZ)
    return defaultType;

  return (char *)value;
}

// ----------------------------------------------------------------------------

static DWORD ReplyAsset(RequestQueue& queue, std::string asset)
{
  // Open up the container just once
  static auto *container = GetGame()->Respack["PropertyViewer"];

  // Trim a slash at the beginning (It's almost certainly there, but may not be)
  if (asset.size() && asset[0] == '/')
    asset = asset.substr(1);

  // Get the resource
  auto *resource = container->GetResource(asset);
  RELEASE_AFTER_SCOPE(resource);

  // Oopsies
  if (!resource->Exists())
    return ReplyNotFound(queue);

  // Figure out the file extension
  size_t lastDot = asset.find_last_of('.');
  std::string extension;
  if (lastDot != asset.npos)
    extension = asset.substr(lastDot);
  else
    extension = ".txt";

  auto mimeType = GetMimeType(extension.c_str());

  ResponseData data;
  data.contentType = mimeType.c_str();

  fs::path filePath;
  if (resource->IsFileBased(&filePath))
  {
    HANDLE file = CreateFile(filePath.file_string().c_str(),
                             GENERIC_READ,
                             FILE_SHARE_WRITE,
                             nullptr,
                             OPEN_EXISTING,
                             FILE_ATTRIBUTE_NORMAL,
                             nullptr);
    if (file == INVALID_HANDLE_VALUE)
      return ReplyInternalError(queue);

    auto res = ReplyToRequestFile(queue, data, (HANDLE) file, [](HTTP_RESPONSE&){});
    CloseHandle(file);
    return res;
  }
  else
  {
    auto rdata = resource->GetData();
    auto rdata_len = resource->GetSize();
    return ReplyToRequest(queue, data, rdata, (ULONG) rdata_len);
  }
}

// ----------------------------------------------------------------------------

static DWORD ReplyRedirect(RequestQueue& queue, 
                           const std::string& destination, bool perma)
{
  ResponseData data;
  data.code = perma? 301 : 307;
  data.reason = perma? "Moved Permanently" : "Temporary Redirect";

  // {"redirect":"#{destination}"}
  std::string body = R"({"redirect":")" + destination + R"("})";

  return ReplyToRequest(
    queue, data, body.c_str(), (ULONG) body.size(),
    [&destination](HTTP_RESPONSE& response)
    {
      SetKnownHeader(response, HttpHeaderLocation, destination.c_str());
    }
  );
}

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

  new (&data.staticThread) std::thread(std::bind(RecieveRequests, data.staticQueue));
  data.staticThread.detach();

  new (&data.staticThread) std::thread(std::bind(RecieveRequests, data.entityQueue));
  data.staticThread.detach();

  new (&data.staticThread) std::thread(std::bind(RecieveRequests, data.globalQueue));
  data.staticThread.detach();
}

// ----------------------------------------------------------------------------

static void CloseServer(PropertyServerInternal& data)
{
  CloseGlobalHandler(data);
  CloseEntityHandler(data);
  CloseStaticHandler(data);

  HttpTerminate(HTTP_INITIALIZE_SERVER, nullptr);
}

// ----------------------------------------------------------------------------

static void RegisterStaticHandler(PropertyServerInternal& data)
{
  DWORD res;

  res = HttpCreateHttpHandle(&data.staticQueue, 0);
  CheckHTTPResult(res);

  res = HttpAddUrl(data.staticQueue, STATIC_HANDLER_URL, nullptr);
  CheckHTTPResult(res);

  data.staticQueue.handler = RecieveStatic;
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

  data.entityQueue.handler = RecieveEntity;
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

  data.globalQueue.handler = RecieveGlobal;
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

static void RecieveRequests(RequestQueue& queue)
{
  ULONG result;
  HTTP_REQUEST_ID requestId;
  DWORD bytesRead;
  PHTTP_REQUEST pRequest;
  RequestBuffer buffer(sizeof(HTTP_REQUEST) + 2048);

  pRequest = buffer;
  queue.buffer = &buffer;

  HTTP_SET_NULL_ID(&requestId);
  
  for (;;)
  {
    buffer.Zero();

    result = HttpReceiveHttpRequest(
      queue, 
      requestId,
      0,
      pRequest,
      buffer,
      &bytesRead,
      nullptr);

    if (result == NO_ERROR)
    {
      queue.reqId = pRequest->RequestId;
      result = queue();

      if (result != NO_ERROR)
        break;

      HTTP_SET_NULL_ID(&requestId);
    }
    else if (result == ERROR_MORE_DATA)
    {
      requestId = pRequest->RequestId;
      buffer.Resize(bytesRead);
      pRequest = buffer;
    }
    else if (result == ERROR_CONNECTION_INVALID &&
             !HTTP_IS_NULL_ID(&requestId))
    {
      HTTP_SET_NULL_ID(&requestId);
    }
    else
    {
      break;
    }
  }
}

// ----------------------------------------------------------------------------

static DWORD ReplyNotFound(RequestQueue& queue)
{
  ResponseData data;
  data.code = 404;
  data.reason = "Not Found";
  std::string reply = R"({"error": "Not Found"})";
  return ReplyToRequest(queue, data, reply.c_str(), (ULONG) reply.size());
}

// ----------------------------------------------------------------------------

static DWORD ReplyInternalError(RequestQueue& queue)
{
  ResponseData data;
  data.code = 500;
  data.reason = "Internal Server Error";
  std::string reply = R"({"error": "Internal Server Error"})";
  return ReplyToRequest(queue, data, reply.c_str(), (ULONG) reply.size());
}

// ----------------------------------------------------------------------------

static DWORD DisplayEntity(RequestQueue& queue, Entity *entity, 
                           UrlParts& remainingPath);
static DWORD DisplayComponent(RequestQueue& queue, Component *component,
                              UrlParts& remainingPath);

// ----------------------------------------------------------------------------

static DWORD RecieveStatic(RequestQueue& queue)
{
  PHTTP_REQUEST request = *queue.buffer;
  std::string path = request->pRawUrl;

  if (path == "/game" || path == "/game/")
    return ReplyRedirect(queue, "/game/index.html", true);
  else
    return ReplyAsset(queue, path);
}

// ----------------------------------------------------------------------------

static DWORD RecieveEntity(RequestQueue& queue)
{
  critical_section::guard lock(GetGame()->GameLock);

  PHTTP_REQUEST pRequest = *queue.buffer;
  std::string path = narrow(pRequest->CookedUrl.pAbsPath);
  UrlParts pathParts = split(path, '/');

  for (auto it = pathParts.begin(); it != pathParts.end(); ++it)
  {
    if (it->empty())
    {
      it = pathParts.erase(it);
    }
  }

  pop_front(pathParts); // /game
  pop_front(pathParts); // /api
  pop_front(pathParts); // /entity

  if (pathParts.size() < 2)
  {
    return ReplyNotFound(queue);
  }

  Entity *entity = nullptr;
  if (pathParts[0] == "id")
  {
    auto entityId = std::stoull(pathParts[1]);
    entity = GetGame()->CurrentLevel->RootEntity->FindEntity(entityId);
  }
  else if (pathParts[1] == "name")
  {
    entity = GetGame()->CurrentLevel->RootEntity->FindEntity(pathParts[1]);
  }

  if (entity == nullptr)
  {
    return ReplyNotFound(queue);
  }

  // Remove the two front bits that we already processed
  pop_front(pathParts);
  pop_front(pathParts);

  return DisplayEntity(queue, entity, pathParts);
}

// ----------------------------------------------------------------------------

static DWORD RecieveGlobal(RequestQueue& queue)
{
  critical_section::guard lock(GetGame()->GameLock);

  ResponseData data;
  std::string reply = "Hello, World!";

  return ReplyToRequest(queue, data, reply.c_str(), (ULONG) reply.size());
}

// ----------------------------------------------------------------------------

static DWORD DisplayEntity(RequestQueue& queue, Entity *entity, 
                           UrlParts& remainingPath)
{
  static mrb_state *mrb = *mrb_inst;

  if (remainingPath.empty())
  {
    json::value jdata = json::value::object(
    {
      {"id", json::value::number((long double) entity->Id)},
      {"name", json::value::string(entity->Name)},
      {"components", json::value::array(
        map_to_vector<json::value>(entity->Components,
        [](const std::pair<std::string, Component *>& pair)
        {
          return json::value::string(pair.second->Name);
        })
      )},
      {"children", json::value::array(
        map_to_vector<json::value>(entity->Children,
        [](Entity *child)
        {
          return json::value::object(
          {
            {"id", json::value::number((long double) child->Id)},
            {"name", json::value::string(child->Name)}
          });
        })
      )}
    });

    ResponseData data;
    data.contentType = "application/json";
    return ReplyToRequest(queue, data, jdata);
  }
  else if (remainingPath.size() >= 2 && remainingPath[0] == "component")
  {
    auto *component = entity->GetComponent(remainingPath[1]);
    if (component != nullptr)
    {
      pop_front(remainingPath);
      pop_front(remainingPath);

      return DisplayComponent(queue, component, remainingPath);
    }
  }
  else if (remainingPath.size() == 1 && remainingPath[0] == "eval")
  {
    PHTTP_REQUEST pRequest = *queue.buffer;
    if (pRequest->CookedUrl.pQueryString)
    {
      auto query = narrow(pRequest->CookedUrl.pQueryString);
      mrb_value eval_str = mrb_str_new(mrb, query.c_str(), query.size());
      auto res = mrb_funcall_argv(mrb, mrb_obj_value(mrb->kernel_module),
                                  mrb_intern_lit(mrb, "eval"), 1, &eval_str);
      auto res_str = mrb_str_to_stdstring(res);

      ResponseData data;
      return ReplyToRequest(queue, data, res_str.c_str(), (ULONG) res_str.size());
    }
  }

  return ReplyNotFound(queue);
}

// ----------------------------------------------------------------------------

static std::unordered_set<mrb_sym> ListMethods(mrb_value val)
{
  static mrb_state *mrb = *mrb_inst;
  if (val.tt != MRB_TT_CLASS)
  {
    val = mrb_obj_value(mrb_obj_class(mrb, val));
  }

  mrb_value ary = mrb_funcall_argv(mrb, val, mrb_intern_lit(mrb, "instance_methods"), 0, nullptr);
  mrb_int len = mrb_ary_len(mrb, ary);

  std::unordered_set<mrb_sym> methods;

  for (mrb_int i = 0; i < len; ++i)
  {
    methods.insert(mrb_symbol(mrb_ary_entry(ary, i)));
  }

  return methods;
}

// ----------------------------------------------------------------------------

static std::unordered_set<mrb_sym>& ObjectMethods()
{
  static mrb_state *mrb = *mrb_inst;
  static std::unordered_set<mrb_sym> methods = ListMethods(mrb_obj_value(mrb->object_class));
  return methods;
}

// ----------------------------------------------------------------------------

static std::unordered_set<mrb_sym> GetOwnMethods(mrb_value val)
{
  static auto& object_methods = ObjectMethods();
  auto self_methods = ListMethods(val);
  
  for (auto method : object_methods)
  {
    auto it = self_methods.find(method);
    if (it != self_methods.end())
      self_methods.erase(it);
  }

  return self_methods;
}

// ----------------------------------------------------------------------------

static DWORD DisplayComponent(RequestQueue& queue, Component *component,
                              UrlParts& remainingPath)
{
  static mrb_state *mrb = *mrb_inst;

  if (remainingPath.empty())
  {
    auto wrapper = component->GetRubyWrapper();
    auto all_methods = ListMethods(wrapper);
    auto methods = GetOwnMethods(wrapper);
  
    json::value jdata = json::value::object(
    {
      {"all_methods", json::value::array(
        map_to_vector<json::value>(all_methods,
        [](mrb_sym mid)
        {
          return json::value::string(mrb_sym2name(mrb, mid));
        })
      )},
      {"methods", json::value::array(
        map_to_vector<json::value>(methods,
        [](mrb_sym mid)
        {
          return json::value::string(mrb_sym2name(mrb, mid));
        })
      )}
    });
  
    ResponseData data;
    data.contentType = "application/json";
    return ReplyToRequest(queue, data, jdata);
  }
  
  return ReplyNotFound(queue);
}

// ----------------------------------------------------------------------------
