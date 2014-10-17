/*********************************
 * HttpResult.cpp
 * Connor Hilarides
 * Created 2014/10/14
 *********************************/

#include "Common.h"
#include "HttpClient.h"

// ----------------------------------------------------------------------------

struct MrbHttpResult
{
  MrbHttpResult() : result(std::_Noinit) { }

  HttpResult result;
  mrb_value mrb_result;
  mrb_value proc;
};

// ----------------------------------------------------------------------------

struct MrbHttpClient
{
  HttpClient client;
  std::list<MrbHttpResult> results;

  void Update(mrb_state *mrb);
};

// ----------------------------------------------------------------------------

struct HttpResultLineReader
{
  HttpResultLineReader(const HttpResultStream& stream)
    : stream(stream)
  {
  }

  HttpResultStream stream;

  mrb_value next_line();
};

// ----------------------------------------------------------------------------

#pragma region Data Types

mrb_data_type mrb_http_client_dt;

mrb_data_type mrb_http_uri_dt;
mrb_data_type mrb_http_headers_dt;
mrb_data_type mrb_http_header_set_dt;

mrb_data_type mrb_http_request_dt;
mrb_data_type mrb_http_request_body_dt;

mrb_data_type mrb_http_result_dt;
mrb_data_type mrb_http_result_linereader_dt;

// ----------------------------------------------------------------------------

#define MRB_FREE_FUN(type, name) \
  static void mrb_##name##_free(mrb_state *, void *v) \
  { \
    delete (type *)(v); \
  }

#define MRB_INIT_DT(type, _name) \
  { \
    mrb_##_name##_dt.dfree = mrb_##_name##_free; \
    mrb_##_name##_dt.struct_name = typeid(type).name(); \
  }

MRB_FREE_FUN(MrbHttpClient, http_client);
MRB_FREE_FUN(HttpUri, http_uri);
MRB_FREE_FUN(HttpHeaderCollection, http_headers);
MRB_FREE_FUN(HttpHeaderSet, http_header_set);
MRB_FREE_FUN(HttpRequest, http_request);
MRB_FREE_FUN(HttpRequestBody, http_request_body);
MRB_FREE_FUN(MrbHttpResult, http_result);
MRB_FREE_FUN(HttpResultLineReader, http_result_linereader);

#pragma endregion

// ----------------------------------------------------------------------------

#pragma region HTTP Class Helpers

static RClass *mrb_http_get_class_internal(mrb_state *mrb, RClass *outer, const char *next)
{
  return mrb_class_get_under(mrb, outer, next);
}

template <typename... Path>
static RClass *mrb_http_get_class_internal(mrb_state *mrb, RClass *outer, const char *next, Path&&... path)
{
  auto newouter = mrb_class_get_under(mrb, outer, next);
  return mrb_http_get_class_internal(mrb, newouter, path...);
}

template <typename... Path>
static RClass *mrb_http_get_class(mrb_state *mrb, Path&&... path)
{
  static auto module = mrb_module_get(mrb, "Http");
  return mrb_http_get_class_internal(mrb, module, path...);
}

#pragma endregion

// ----------------------------------------------------------------------------

// static mrb_value mrb_http_(mrb_state *mrb, mrb_value self);

#pragma region Http::Client

static mrb_value mrb_http_client_new(mrb_state *mrb, mrb_value self);

#pragma endregion

// ----------------------------------------------------------------------------

#pragma region Http::Uri

static mrb_value mrb_http_uri_new(mrb_state *mrb, mrb_value self);
static mrb_value mrb_http_uri_inspect(mrb_state *mrb, mrb_value self);

static mrb_value mrb_http_uri_build(mrb_state *mrb, mrb_value self);
static mrb_value mrb_http_uri_build_path(mrb_state *mrb, mrb_value self);

#pragma region Uri Get/Set macros
#define MrbHttpUriGetSelf \
  HttpUri& uri = *(HttpUri *) mrb_data_get_ptr(mrb, self, &mrb_http_uri_dt)
#define MrbHttpUriStringProp(pname, mname) \
  static mrb_value mrb_http_uri_##mname(mrb_state *mrb, mrb_value self) \
  { \
    MrbHttpUriGetSelf; \
    return mrb_str_new(mrb, uri.pname.c_str(), uri.pname.size()); \
  } \
  static mrb_value mrb_http_uri_##mname##_p(mrb_state *mrb, mrb_value self) \
  { \
    MrbHttpUriGetSelf; \
    return mrb_bool_value(uri.Has##pname); \
  } \
  static mrb_value mrb_http_uri_##mname##_s(mrb_state *mrb, mrb_value self) \
  { \
    MrbHttpUriGetSelf; \
    mrb_value str; \
    mrb_get_args(mrb, "S", &str); \
    uri.pname = mrb_str_to_stdstring(str); \
    return mrb_nil_value(); \
  } \
  static mrb_value mrb_http_uri_remove_##mname(mrb_state *mrb, mrb_value self) \
  { \
    MrbHttpUriGetSelf; \
    uri.Remove##pname(); \
    return mrb_nil_value(); \
  }
#define MrbHttpUriIntProp(pname, mname) \
  static mrb_value mrb_http_uri_##mname(mrb_state *mrb, mrb_value self) \
  { \
    MrbHttpUriGetSelf; \
    return mrb_fixnum_value(uri.pname); \
  } \
  static mrb_value mrb_http_uri_##mname##_p(mrb_state *mrb, mrb_value self) \
  { \
    MrbHttpUriGetSelf; \
    return mrb_bool_value(uri.Has##pname); \
  } \
  static mrb_value mrb_http_uri_##mname##_s(mrb_state *mrb, mrb_value self) \
  { \
    MrbHttpUriGetSelf; \
    long long i; \
    mrb_get_args(mrb, "i", &i); \
    uri.pname = int(i); \
    return mrb_nil_value(); \
  } \
  static mrb_value mrb_http_uri_remove_##mname(mrb_state *mrb, mrb_value self) \
  { \
    MrbHttpUriGetSelf; \
    uri.Remove##pname(); \
    return mrb_nil_value(); \
  }

#define MrbHttpUriDefMethods(mname) {\
  mrb_define_method(mrb, uri, #mname, mrb_http_uri_##mname, MRB_ARGS_NONE()); \
  mrb_define_method(mrb, uri, #mname "?", mrb_http_uri_##mname##_p, MRB_ARGS_NONE()); \
  mrb_define_method(mrb, uri, #mname "=", mrb_http_uri_##mname##_s, MRB_ARGS_REQ(1)); \
  mrb_define_method(mrb, uri, "remove_" #mname, mrb_http_uri_remove_##mname, MRB_ARGS_NONE()); }

#pragma endregion

MrbHttpUriStringProp(Scheme, scheme);
MrbHttpUriStringProp(Username, username);
MrbHttpUriStringProp(Password, password);
MrbHttpUriStringProp(Host, host);
MrbHttpUriIntProp   (Port, port);
MrbHttpUriStringProp(Path, path);
MrbHttpUriStringProp(Query, query);
MrbHttpUriStringProp(Fragment, fragment);

#pragma endregion

#pragma region Http::Headers



#pragma endregion

#pragma region Http::Headers::Set



#pragma endregion

// ----------------------------------------------------------------------------

#pragma region Http::Request

static mrb_value mrb_http_request_new(mrb_state *mrb, mrb_value self);

#pragma endregion

#pragma region Http::Request::Body

#pragma endregion

// ----------------------------------------------------------------------------

#pragma region Http::Result



#pragma endregion

#pragma region Http::Result::LineReader



#pragma endregion

// ----------------------------------------------------------------------------

extern "C" void mrb_mruby_http_init(mrb_state *mrb)
{
  MRB_INIT_DT(MrbHttpClient, http_client);
  MRB_INIT_DT(HttpUri, http_uri);
  MRB_INIT_DT(HttpHeaderCollection, http_headers);
  MRB_INIT_DT(HttpHeaderSet, http_header_set);
  MRB_INIT_DT(HttpRequest, http_request);
  MRB_INIT_DT(HttpRequestBody, http_request_body);
  MRB_INIT_DT(MrbHttpResult, http_result);
  MRB_INIT_DT(HttpResultLineReader, http_result_linereader);

  auto http = mrb_define_module(mrb, "Http");

  mrb_define_const(mrb, http, "GET", mrb_fixnum_value(HTTP_GET));
  mrb_define_const(mrb, http, "HEAD", mrb_fixnum_value(HTTP_HEAD));
  mrb_define_const(mrb, http, "POST", mrb_fixnum_value(HTTP_POST));
  mrb_define_const(mrb, http, "PUT", mrb_fixnum_value(HTTP_PUT));
  mrb_define_const(mrb, http, "DELETE", mrb_fixnum_value(HTTP_DELETE));
  mrb_define_const(mrb, http, "OPTIONS", mrb_fixnum_value(HTTP_OPTIONS));
  mrb_define_const(mrb, http, "PATCH", mrb_fixnum_value(HTTP_PATCH));

#pragma region Http::Client

  auto client = mrb_define_class_under(mrb, http, "Client", mrb->object_class);

  mrb_define_class_method(mrb, client, "new", mrb_http_client_new, ARGS_NONE());

#pragma endregion
  

#pragma region Http::Uri

  auto uri = mrb_define_class_under(mrb, http, "Uri", mrb->object_class);

  mrb_define_class_method(mrb, uri, "new", mrb_http_uri_new, ARGS_OPT(1));
  mrb_define_method(mrb, uri, "inspect", mrb_http_uri_inspect, ARGS_NONE());

  mrb_define_method(mrb, uri, "build", mrb_http_uri_build, ARGS_NONE());
  mrb_define_method(mrb, uri, "build_path", mrb_http_uri_build_path, ARGS_NONE());

  MrbHttpUriDefMethods(scheme);
  MrbHttpUriDefMethods(username);
  MrbHttpUriDefMethods(password);
  MrbHttpUriDefMethods(host);
  MrbHttpUriDefMethods(port);
  MrbHttpUriDefMethods(path);
  MrbHttpUriDefMethods(query);
  MrbHttpUriDefMethods(fragment);

#pragma endregion

#pragma region Http::Headers

  auto headers = mrb_define_class_under(mrb, http, "Headers", mrb->object_class);

#pragma endregion

#pragma region Http::Headers::Set

  auto header_set = mrb_define_class_under(mrb, headers, "Set", mrb->object_class);

#pragma endregion
  

#pragma region Http::Request

  auto request = mrb_define_class_under(mrb, http, "Request", mrb->object_class);

#pragma endregion

#pragma region Http::Request::Body

  auto request_body = mrb_define_class_under(mrb, request, "Body", mrb->object_class);

#pragma endregion
  

#pragma region Http::Result

  auto result = mrb_define_class_under(mrb, http, "Result", mrb->object_class);

#pragma endregion

#pragma region Http::Result::LineReader

  auto result_linereader = mrb_define_class_under(mrb, result, "LineReader", mrb->object_class);

#pragma endregion

  (headers, header_set, request, request_body, result, result_linereader);
}

// ----------------------------------------------------------------------------

#pragma region Http::Client

void MrbHttpClient::Update(mrb_state *mrb)
{
  std::vector<decltype(results.begin())> kill;

  for (auto it = results.begin(); it != results.end(); ++it)
  {
    auto& result = *it;

    if (result.result.HasFailed || result.result.HasData)
    {
      mrb_funcall(mrb, result.proc, "call", 1, result.mrb_result);
      kill.push_back(it);
    }
  }

  for (auto& it : kill)
  {
    results.erase(it);
  }
}

static mrb_value mrb_http_client_new(mrb_state *mrb, mrb_value)
{
  static auto client_c = mrb_http_get_class(mrb, "Client");

  auto obj = mrb_data_object_alloc(mrb, client_c, new HttpClient, &mrb_http_client_dt);
  return mrb_obj_value(obj);
}

#pragma endregion

// ----------------------------------------------------------------------------

#pragma region Http::Uri

static mrb_value mrb_http_uri_new(mrb_state *mrb, mrb_value)
{
  static auto uri_c = mrb_http_get_class(mrb, "Uri");

  mrb_value str = mrb_nil_value();
  mrb_get_args(mrb, "|S", &str);

  auto uri = mrb_nil_p(str)? new HttpUri() : new HttpUri(mrb_str_to_stdstring(str));
  auto obj = mrb_data_object_alloc(mrb, uri_c, uri, &mrb_http_uri_dt);
  return mrb_obj_value(obj);
}

static mrb_value mrb_http_uri_inspect(mrb_state *mrb, mrb_value self)
{
  MrbHttpUriGetSelf;

  std::ostringstream buf;
  buf << "#<Uri:0x" << &uri;

  if (uri.HasHost)
    buf << " built=" << uri.Build();
  else
    buf << " no_host";

  if (uri.HasScheme)
    buf << ", scheme=" << uri.Scheme;
  if (uri.HasUsername)
  {
    buf << ", username=" << uri.Username;
    if (uri.HasPassword)
      buf << ", password=" << uri.Password;
  }
  if (uri.HasHost)
    buf << ", host=" << uri.Host;
  if (uri.HasPort)
    buf << ", port=" << uri.Port;
  if (uri.HasPath)
    buf << ", path=" << uri.Path;
  if (uri.HasQuery)
    buf << ", query=" << uri.Query;
  if (uri.HasFragment)
    buf << ", fragment=" << uri.Fragment;

  buf << ">";
  auto uristr = buf.str();
  
  return mrb_str_new(mrb, uristr.c_str(), uristr.size());
}

static mrb_value mrb_http_uri_build(mrb_state *mrb, mrb_value self)
{
  MrbHttpUriGetSelf;

  auto uristr = uri.Build();
  return mrb_str_new(mrb, uristr.c_str(), uristr.size());
}

static mrb_value mrb_http_uri_build_path(mrb_state *mrb, mrb_value self)
{
  MrbHttpUriGetSelf;

  auto uristr = uri.BuildPath();
  return mrb_str_new(mrb, uristr.c_str(), uristr.size());
}

#pragma endregion

// ----------------------------------------------------------------------------

#pragma region Http::Headers



#pragma endregion

// ----------------------------------------------------------------------------

#pragma region Http::Headers::Set



#pragma endregion

// ----------------------------------------------------------------------------

#pragma region Http::Request

static mrb_value mrb_http_request_new(mrb_state *mrb, mrb_value)
{
  static auto req_c = mrb_http_get_class(mrb, "Request");

  mrb_value uri;
  mrb_int verb;
  mrb_get_args(mrb, "oi", &uri, &verb);

  auto& cpp_uri = *(HttpUri *)mrb_data_get_ptr(mrb, uri, &mrb_http_uri_dt);
  auto request = new HttpRequest(cpp_uri, (HttpMethod) verb);
  auto obj = mrb_data_object_alloc(mrb, req_c, request, &mrb_http_request_dt);

  return mrb_obj_value(obj);
}

#pragma endregion

// ----------------------------------------------------------------------------

#pragma region Http::Request::Body



#pragma endregion

// ----------------------------------------------------------------------------

#pragma region Http::Result



#pragma endregion

// ----------------------------------------------------------------------------

#pragma region Http::Result::LineReader



#pragma endregion

// ----------------------------------------------------------------------------
