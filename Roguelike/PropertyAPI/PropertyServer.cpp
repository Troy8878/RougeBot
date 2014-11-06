/*********************************
 * PropertyServer.cpp
 * Connor Hilarides
 * Created 2014/10/27
 *********************************/

#include "PropertyServer.h"
#include "Engine/Common.h"
#include "Engine/Level.h"
#include "Engine/RubyWrappers.h"
#include "Engine/HttpClient.h"

#include <http.h>

// ----------------------------------------------------------------------------

#define STATIC_HANDLER_URL L"http://localhost:5430/game/"
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
  SetKnownHeader(response, HttpHeaderCacheControl, "no-cache");
  SetKnownHeader(response, HttpHeaderExpires, "-1");
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
  SetKnownHeader(response, HttpHeaderCacheControl, "no-cache");
  SetKnownHeader(response, HttpHeaderExpires, "-1");
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
      critical_section::guard lock(GetGame()->GameLock);

      queue.reqId = pRequest->RequestId;

      try
      {
        result = queue();
      }
      catch (basic_exception& ex)
      {
        std::ostringstream buf;
        buf << ex.what();
        ex.print_trace(buf);
        std::string text = buf.str();

        ResponseData data;
        data.code = 500;
        result = ReplyToRequest(queue, data, text.c_str(), (ULONG) text.size());
      }
      catch (std::exception& ex)
      {
        std::string text = ex.what();

        ResponseData data;
        data.code = 500;
        result = ReplyToRequest(queue, data, text.c_str(), (ULONG) text.size());
      }

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

  if (GetGame()->CurrentLevel == nullptr)
    return ReplyNotFound(queue);

  if (GetGame()->CurrentLevel->RootEntity == nullptr)
    return ReplyNotFound(queue);

  Entity *entity = nullptr;
  if (pathParts[0] == "id")
  {
    auto entityId = std::stoull(pathParts[1]);
    entity = GetGame()->CurrentLevel->RootEntity->FindEntity(entityId);
  }
  else if (pathParts[0] == "name")
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

    if (entity->Parent)
    {
      jdata["parent"] = json::value::object();
      jdata["parent"]["id"] = json::value::number((long double) entity->Parent->Id);
      jdata["parent"]["name"] = json::value::string(entity->Parent->Name);
    }

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
  else if (remainingPath.size() == 1 && remainingPath[0] == "zombify")
  {
    entity->Zombify();

    ResponseData data;
    std::string value = R"({"zombified":true})";
    return ReplyToRequest(queue, data, value.c_str(), (ULONG)value.size());
  }

  return ReplyNotFound(queue);
}

// ----------------------------------------------------------------------------

struct MetaProperty;
static std::vector<MetaProperty> GetProperties(mrb_state *mrb, mrb_value obj);
struct MetaProperty
{
  MetaProperty(mrb_state *mrb, mrb_value prop)
    : mrb(mrb), prop(prop), id(-1), type(-1), can_set((bool)-1)
  {
    static mrb_sym get_id = mrb_intern_lit(mrb, "id");
    static mrb_sym get_type = mrb_intern_lit(mrb, "type");
    static mrb_sym get_can_set = mrb_intern_lit(mrb, "can_set?");

    id = mrb_symbol(mrb_funcall_argv(mrb, prop, get_id, 0, nullptr));
    type = mrb_symbol(mrb_funcall_argv(mrb, prop, get_type, 0, nullptr));
    can_set = !!mrb_bool(mrb_funcall_argv(mrb, prop, get_can_set, 0, nullptr));

    id_name = mrb_sym2name(mrb, id);
    type_name = mrb_sym2name(mrb, type);
  }

  mrb_state *mrb;
  mrb_value prop;
  mrb_sym id;
  mrb_sym type;
  bool can_set;

  const char *id_name;
  const char *type_name;

  operator json::value() const
  {
    return json::value::object(
    {
      {"id", json::value::string(mrb_sym2name(mrb, id))},
      {"type", json::value::string(mrb_sym2name(mrb, type))},
      {"can_set", json::value::boolean(can_set)}
    });
  }

  mrb_value get_value(mrb_value obj) const
  {
    static mrb_sym s_get = mrb_intern_lit(mrb, "get");
    return mrb_funcall_argv(mrb, prop, s_get, 1, &obj);
  }

  void set_value(mrb_value obj, mrb_value value)
  {
    static mrb_sym s_set = mrb_intern_lit(mrb, "set");
    const mrb_value params[] = { obj, value };
    mrb_funcall_argv(mrb, prop, s_set, 2, params);
  }

  json::value display(mrb_value obj) const
  {
    static const mrb_sym t_bool = mrb_intern_lit(mrb, "bool");
    static const mrb_sym t_float = mrb_intern_lit(mrb, "float");
    static const mrb_sym t_int = mrb_intern_lit(mrb, "int");
    static const mrb_sym t_string = mrb_intern_lit(mrb, "string");
    static const mrb_sym t_vector = mrb_intern_lit(mrb, "vector");
    static const mrb_sym t_color = mrb_intern_lit(mrb, "color");

    mrb_value value = get_value(obj);

    if (type == t_bool)
    {
      return json::value::object(
      {
        {"type", json::value::string("bool")},
        {"value", json::value::boolean(!!mrb_bool(value))}
      });
    }
    else if (type == t_float)
    {
      return json::value::object(
      {
        {"type", json::value::string("float")},
        {"value", json::value::number(mrb_float(value))}
      });
    }
    else if (type == t_int)
    {
      return json::value::object(
      {
        {"type", json::value::string("int")},
        {"value", json::value::number((long double)mrb_fixnum(value))}
      });
    }
    else if (type == t_string)
    {
      return json::value::object(
      {
        {"type", json::value::string("string")},
        {"value", json::value::string(mrb_str_to_stdstring(value))}
      });
    }
    else if (type == t_vector || type == t_color)
    {
      auto& vect = ruby::get_ruby_vector(value);

      return json::value::object(
      {
        {"type", json::value::string(mrb_sym2name(mrb, type))},
        {"x", json::value::number(vect.x)},
        {"y", json::value::number(vect.y)},
        {"z", json::value::number(vect.z)},
        {"w", json::value::number(vect.w)},
      });
    }
    else
    {
      auto props = GetProperties(mrb, value);

      return json::value::object(
      {
        {"type", json::value::string("object")},
        {"properties", json::value::array(
          map_to_vector<json::value>(props,
          [](const MetaProperty& prop) -> json::value
          {
            return prop;
          })
        )}
      });
    }
  }

  bool read_in(mrb_value obj, json::value data)
  {
    static const mrb_sym t_bool = mrb_intern_lit(mrb, "bool");
    static const mrb_sym t_float = mrb_intern_lit(mrb, "float");
    static const mrb_sym t_int = mrb_intern_lit(mrb, "int");
    static const mrb_sym t_string = mrb_intern_lit(mrb, "string");
    static const mrb_sym t_vector = mrb_intern_lit(mrb, "vector");
    static const mrb_sym t_color = mrb_intern_lit(mrb, "color");

    if (type == t_bool)
    {
      if (data["type"].as_string() != "bool")
        return false;

      set_value(obj, mrb_bool_value(data["value"].as_bool()));
      return true;
    }
    else if (type == t_float)
    {
      if (data["type"].as_string() != "float")
        return false;

      set_value(obj, mrb_float_value(mrb, (mrb_float)data["value"].as_number()));
      return true;
    }
    else if (type == t_int)
    {
      if (data["type"].as_string() != "int")
        return false;

      set_value(obj, mrb_fixnum_value((mrb_int)data["value"].as_number()));
      return true;
    }
    else if (type == t_string)
    {
      if (data["type"].as_string() != "string")
        return false;

      auto& str = data["value"].as_string();
      set_value(obj, mrb_str_new(mrb, str.c_str(), str.size()));
      return true;
    }

    return false;
  }

  void on_access(mrb_value obj) const
  {
    static mrb_sym on_update = mrb_intern_lit(mrb, "on_access");
    if (mrb_respond_to(mrb, obj, on_update))
    {
      mrb_funcall_argv(mrb, obj, on_update, 0, nullptr);
    }
  }
};

// ----------------------------------------------------------------------------

static std::vector<MetaProperty> GetProperties(mrb_state *mrb, mrb_value obj)
{
  std::vector<MetaProperty> props;

  static mrb_sym get_properties = mrb_intern_lit(mrb, "properties");
  auto sclass = mrb_obj_value(mrb_obj_class(mrb, obj));
  mrb_value ary = mrb_funcall_argv(mrb, sclass, get_properties, 0, nullptr);

  for (auto prop : ruby::array_each(mrb, ary))
  {
    props.push_back(MetaProperty(mrb, prop));
  }

  return props;
}

// ----------------------------------------------------------------------------

static DWORD DisplayGetProperty(RequestQueue& queue, mrb_value obj, UrlParts& remainingPath);
static DWORD DisplaySetProperty(RequestQueue& queue, mrb_value obj, UrlParts& remainingPath);

// ----------------------------------------------------------------------------

static DWORD DisplayComponent(RequestQueue& queue, Component *component,
                              UrlParts& remainingPath)
{
  static mrb_state *mrb = *mrb_inst;
  ruby::ruby_gc_guard guard(mrb);

  auto wrapper = component->GetRubyWrapper();

  if (remainingPath.empty())
  {
    auto props = GetProperties(mrb, wrapper);

    json::value jdata = json::value::object(
    {
      {"properties", json::value::array(
        map_to_vector<json::value>(props,
        [](const MetaProperty& prop) -> json::value
        {
          return prop;
        })
      )}
    });
  
    ResponseData data;
    data.contentType = "application/json";
    return ReplyToRequest(queue, data, jdata);
  }
  else if (remainingPath[0] == "get")
  {
    pop_front(remainingPath);
    return DisplayGetProperty(queue, wrapper, remainingPath);
  }
  else if (remainingPath[0] == "set")
  {
    pop_front(remainingPath);
    return DisplaySetProperty(queue, wrapper, remainingPath);
  }
  
  return ReplyNotFound(queue);
}

// ----------------------------------------------------------------------------

static DWORD DisplayGetProperty(RequestQueue& queue, mrb_value obj, UrlParts& remainingPath)
{
  static mrb_state * const mrb = *mrb_inst;

  if (remainingPath.empty())
    return ReplyNotFound(queue);

  const auto props = GetProperties(mrb, obj);
  const auto propName = remainingPath.front();
  pop_front(remainingPath);
  
  auto propIt = std::find_if(props.begin(), props.end(),
  [&propName](const MetaProperty& prop)
  {
    return propName == prop.id_name;
  });

  if (propIt == props.end())
  {
    return ReplyNotFound(queue);
  }

  const MetaProperty prop = *propIt;
  
  if (remainingPath.empty())
  {
    ResponseData data;
    data.contentType = "application/json";
    auto res = ReplyToRequest(queue, data, prop.display(obj));
    prop.on_access(obj);
    return res;
  }
  else if (remainingPath[0] == "get")
  {
    pop_front(remainingPath);
    auto res = DisplayGetProperty(queue, prop.get_value(obj), remainingPath);
    prop.on_access(obj);
    return res;
  }
  else if (remainingPath[0] == "set")
  {
    pop_front(remainingPath);
    auto res =  DisplaySetProperty(queue, prop.get_value(obj), remainingPath);
    prop.on_access(obj);
    return res;
  }

  return ReplyNotFound(queue);
}

// ----------------------------------------------------------------------------

static DWORD DisplaySetProperty(RequestQueue& queue, mrb_value obj, UrlParts& remainingPath)
{
  static mrb_state * const mrb = *mrb_inst;

  if (remainingPath.empty())
    return ReplyNotFound(queue);

  const auto props = GetProperties(mrb, obj);
  const auto propName = remainingPath.front();
  pop_front(remainingPath);
  
  auto propIt = std::find_if(props.begin(), props.end(),
  [&propName](const MetaProperty& prop)
  {
    return propName == prop.id_name;
  });

  if (propIt == props.end())
  {
    return ReplyNotFound(queue);
  }

  MetaProperty prop = *propIt;
  
  if (remainingPath.size() != 1)
    return ReplyNotFound(queue);

  auto indata = HttpUri::Decode(remainingPath[0]);
  auto input = json::value::parse(indata);

  auto response = json::value::object(
  {
    {"success", json::value::boolean(prop.read_in(obj, input))}
  });

  prop.on_access(obj);

  ResponseData data;
  data.contentType = "application/json";
  return ReplyToRequest(queue, data, response);
}

// ----------------------------------------------------------------------------
