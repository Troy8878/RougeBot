/*********************************
 * HttpResult.cpp
 * Connor Hilarides
 * Created 2014/10/14
 * Copyright © 2014 DigiPen Institute of Technology, All Rights Reserved
 *********************************/

#include "Common.h"
#include "HttpClient.h"

#include "mruby/proc.h"

// ----------------------------------------------------------------------------

struct MrbHttpResult
{
  explicit MrbHttpResult(const HttpResult &res) : result(res)
  {
  }

  HttpResult result;
  mrb_value mrb_result;
  mrb_value proc;
};

// ----------------------------------------------------------------------------

struct MrbHttpClient
{
  HttpClient client;

  void Update(mrb_state *mrb, mrb_value self);
};

// ----------------------------------------------------------------------------

struct HttpResultLineReader
{
  HttpResultLineReader(const HttpResultStream &stream)
    : stream(stream)
  {
  }

  HttpResultStream stream;

  mrb_value next_line();
};

// ----------------------------------------------------------------------------

mrb_value HttpResultLineReader::next_line()
{
  return mrb_nil_value();
}

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
static RClass *mrb_http_get_class_internal(mrb_state *mrb, RClass *outer, const char *next, Path &&... path)
{
  auto newouter = mrb_class_get_under(mrb, outer, next);
  return mrb_http_get_class_internal(mrb, newouter, path...);
}

template <typename... Path>
static RClass *mrb_http_get_class(mrb_state *mrb, Path &&... path)
{
  static auto module = mrb_module_get(mrb, "Http");
  return mrb_http_get_class_internal(mrb, module, path...);
}

#pragma endregion

// ----------------------------------------------------------------------------

#pragma region Http::Client

static mrb_value mrb_http_client_new(mrb_state *mrb, mrb_value self);

static mrb_value mrb_http_client_request(mrb_state *mrb, mrb_value self);
static mrb_value mrb_http_client_update(mrb_state *mrb, mrb_value self);

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

MrbHttpUriIntProp (Port, port);

MrbHttpUriStringProp(Path, path);

MrbHttpUriStringProp(Query, query);

MrbHttpUriStringProp(Fragment, fragment);

#pragma endregion

#pragma region Http::Headers

static mrb_value mrb_http_headers_new(mrb_state *mrb, const HttpHeaderCollection &headers);

static mrb_value mrb_http_headers_each(mrb_state *mrb, mrb_value self);
static mrb_value mrb_http_headers_get(mrb_state *mrb, mrb_value self);

#pragma endregion

#pragma region Http::Headers::Set

static mrb_value mrb_http_header_set_new(mrb_state *mrb, const HttpHeaderSet &set);

static mrb_value mrb_http_header_set_key(mrb_state *mrb, mrb_value self);
static mrb_value mrb_http_header_set_each(mrb_state *mrb, mrb_value self);
static mrb_value mrb_http_header_set_add(mrb_state *mrb, mrb_value self);
static mrb_value mrb_http_header_set_remove(mrb_state *mrb, mrb_value self);
static mrb_value mrb_http_header_set_clear(mrb_state *mrb, mrb_value self);

#pragma endregion

// ----------------------------------------------------------------------------

#pragma region Http::Request

static mrb_value mrb_http_request_new(mrb_state *mrb, mrb_value self);

static mrb_value mrb_http_request_body(mrb_state *mrb, mrb_value self);
static mrb_value mrb_http_request_headers(mrb_state *mrb, mrb_value self);

#pragma endregion

#pragma region Http::Request::Body

static mrb_value mrb_http_request_body_new(mrb_state *mrb, const HttpRequestBody &body);

static mrb_value mrb_http_request_body_json_set(mrb_state *mrb, mrb_value self);
static mrb_value mrb_http_request_body_string_set(mrb_state *mrb, mrb_value self);
static mrb_value mrb_http_request_body_form_set(mrb_state *mrb, mrb_value self);
static mrb_value mrb_http_request_body_type_set(mrb_state *mrb, mrb_value self);

#pragma endregion

// ----------------------------------------------------------------------------

#pragma region Http::Result

static mrb_value mrb_http_result_headers(mrb_state *mrb, mrb_value self);
static mrb_value mrb_http_result_as_json(mrb_state *mrb, mrb_value self);
static mrb_value mrb_http_result_as_string(mrb_state *mrb, mrb_value self);

static mrb_value mrb_http_result_headers_p(mrb_state *mrb, mrb_value self);
static mrb_value mrb_http_result_data_p(mrb_state *mrb, mrb_value self);
static mrb_value mrb_http_result_failed_p(mrb_state *mrb, mrb_value self);

#pragma endregion

#pragma region Http::Result::LineReader


#pragma endregion

// ----------------------------------------------------------------------------

extern "C" void mrb_mruby_http_init(mrb_state *mrb)
{
#pragma region Define classes

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

#pragma endregion

#pragma region Http::Client

  auto client = mrb_define_class_under(mrb, http, "Client", mrb->object_class);

  mrb_define_class_method(mrb, client, "new",
                          mrb_http_client_new, MRB_ARGS_NONE());

  mrb_define_method(mrb, client, "make_request",
                    mrb_http_client_request, MRB_ARGS_REQ(1) | MRB_ARGS_BLOCK());
  mrb_define_method(mrb, client, "update",
                    mrb_http_client_update, MRB_ARGS_NONE());

#pragma endregion


#pragma region Http::Uri

  auto uri = mrb_define_class_under(mrb, http, "Uri", mrb->object_class);

  mrb_define_class_method(mrb, uri, "new", mrb_http_uri_new, MRB_ARGS_OPT(1));
  mrb_define_method(mrb, uri, "inspect", mrb_http_uri_inspect, MRB_ARGS_NONE());

  mrb_define_method(mrb, uri, "build", mrb_http_uri_build, MRB_ARGS_NONE());
  mrb_define_method(mrb, uri, "build_path", mrb_http_uri_build_path, MRB_ARGS_NONE());

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

  mrb_define_class_method(mrb, headers, "new", mrb_nop, MRB_ARGS_NONE());

  mrb_define_method(mrb, headers, "each", mrb_http_headers_each, MRB_ARGS_NONE());
  mrb_define_method(mrb, headers, "[]", mrb_http_headers_get, MRB_ARGS_NONE());

  mrb_funcall(mrb, mrb_obj_value(headers), "include", 1, mrb_obj_value(mrb_module_get(mrb, "Enumerable")));

#pragma endregion

#pragma region Http::Headers::Set

  auto header_set = mrb_define_class_under(mrb, headers, "Set", mrb->object_class);

  mrb_define_class_method(mrb, header_set, "new", mrb_nop, MRB_ARGS_NONE());

  mrb_define_method(mrb, header_set, "key", mrb_http_header_set_key, MRB_ARGS_NONE());
  mrb_define_method(mrb, header_set, "each", mrb_http_header_set_each, MRB_ARGS_BLOCK());
  mrb_define_method(mrb, header_set, "add", mrb_http_header_set_add, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, header_set, "remove", mrb_http_header_set_remove, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, header_set, "clear", mrb_http_header_set_clear, MRB_ARGS_NONE());

  mrb_funcall(mrb, mrb_obj_value(header_set), "include", 1, mrb_obj_value(mrb_module_get(mrb, "Enumerable")));

#pragma endregion


#pragma region Http::Request

  auto request = mrb_define_class_under(mrb, http, "Request", mrb->object_class);

  mrb_define_class_method(mrb, request, "new", mrb_http_request_new, MRB_ARGS_NONE());

  mrb_define_method(mrb, request, "body", mrb_http_request_body, MRB_ARGS_NONE());
  mrb_define_method(mrb, request, "headers", mrb_http_request_headers, MRB_ARGS_NONE());

#pragma endregion

#pragma region Http::Request::Body

  auto request_body = mrb_define_class_under(mrb, request, "Body", mrb->object_class);

  mrb_define_class_method(mrb, request_body, "new", mrb_nop, MRB_ARGS_NONE());

  mrb_define_method(mrb, request_body, "json=",
                    mrb_http_request_body_json_set, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, request_body, "string=",
                    mrb_http_request_body_string_set, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, request_body, "form=",
                    mrb_http_request_body_form_set, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, request_body, "content_type=",
                    mrb_http_request_body_type_set, MRB_ARGS_REQ(1));

#pragma endregion


#pragma region Http::Result

  auto result = mrb_define_class_under(mrb, http, "Result", mrb->object_class);

  mrb_define_class_method(mrb, result, "new", mrb_nop, MRB_ARGS_NONE());

  mrb_define_method(mrb, result, "headers", mrb_http_result_headers, MRB_ARGS_NONE());
  mrb_define_method(mrb, result, "as_json", mrb_http_result_as_json, MRB_ARGS_NONE());
  mrb_define_method(mrb, result, "as_string", mrb_http_result_as_string, MRB_ARGS_NONE());

  mrb_define_method(mrb, result, "headers?", mrb_http_result_headers_p, MRB_ARGS_NONE());
  mrb_define_method(mrb, result, "data?", mrb_http_result_data_p, MRB_ARGS_NONE());
  mrb_define_method(mrb, result, "failed?", mrb_http_result_failed_p, MRB_ARGS_NONE());

#pragma endregion

#pragma region Http::Result::LineReader

  auto result_linereader = mrb_define_class_under(mrb, result, "LineReader", mrb->object_class);

  mrb_define_class_method(mrb, result_linereader, "new", mrb_nop, MRB_ARGS_NONE());

#pragma endregion
}

// ----------------------------------------------------------------------------

#pragma region Http::Client

void MrbHttpClient::Update(mrb_state *mrb, mrb_value self)
{
  static auto queue_sym = mrb_intern_lit(mrb, "@waiting_results");
  auto queue = mrb_iv_get(mrb, self, queue_sym);

  for (mrb_int i = 0; i < mrb_ary_len(mrb, queue); ++i)
  {
    auto mrb_result = mrb_ary_entry(queue, i);
    auto &result = *static_cast<MrbHttpResult *>(mrb_data_get_ptr(mrb, mrb_result, &mrb_http_result_dt));

    if (result.result.HasFailed || result.result.HasData)
    {
      mrb_funcall(mrb, result.proc, "call", 1, result.mrb_result);

      static auto delete_sym = mrb_intern_lit(mrb, "delete_at");
      mrb_value index = mrb_fixnum_value(i);
      mrb_funcall_argv(mrb, queue, delete_sym, 1, &index);
    }
  }
}

// ----------------------------------------------------------------------------

static mrb_value mrb_http_client_new(mrb_state *mrb, mrb_value)
{
  static auto client_c = mrb_http_get_class(mrb, "Client");

  auto obj = mrb_data_object_alloc(mrb, client_c, new MrbHttpClient, &mrb_http_client_dt);
  auto client = mrb_obj_value(obj);

  mrb_iv_set(mrb, client, mrb_intern_lit(mrb, "@waiting_results"), mrb_ary_new(mrb));

  return client;
}

// ----------------------------------------------------------------------------

static mrb_value mrb_http_client_request(mrb_state *mrb, mrb_value self)
{
  static auto result_c = mrb_http_get_class(mrb, "Result");

  mrb_value request_v;
  mrb_value block = mrb_nil_value();
  mrb_get_args(mrb, "o|&", &request_v, &block);

  auto &client = *static_cast<MrbHttpClient *>(mrb_data_get_ptr(mrb, self, &mrb_http_client_dt));
  auto &request = *static_cast<HttpRequest *>(mrb_data_get_ptr(mrb, request_v, &mrb_http_request_dt));

  auto result = client.client.MakeRequest(request);
  auto mresult = new MrbHttpResult(result);
  mresult->proc = block;

  auto mresult_obj = mrb_data_object_alloc(mrb, result_c, mresult, &mrb_http_result_dt);
  mresult->mrb_result = mrb_obj_value(mresult_obj);

  if (!mrb_nil_p(mresult->proc))
  {
    static auto callback_sym = mrb_intern_lit(mrb, "@callback");
    mrb_iv_set(mrb, mresult->mrb_result, callback_sym, mresult->proc);

    static auto queue_sym = mrb_intern_lit(mrb, "@waiting_results");
    auto queue = mrb_iv_get(mrb, self, queue_sym);

    mrb_ary_push(mrb, queue, mresult->mrb_result);
  }

  return mresult->mrb_result;
}

// ----------------------------------------------------------------------------

static mrb_value mrb_http_client_update(mrb_state *mrb, mrb_value self)
{
  auto &client = *static_cast<MrbHttpClient *>(mrb_data_get_ptr(mrb, self, &mrb_http_client_dt));

  client.Update(mrb, self);

  return mrb_nil_value();
}

#pragma endregion

// ----------------------------------------------------------------------------

#pragma region Http::Uri

static mrb_value mrb_http_uri_new(mrb_state *mrb, mrb_value)
{
  static auto uri_c = mrb_http_get_class(mrb, "Uri");

  mrb_value str = mrb_nil_value();
  mrb_get_args(mrb, "|S", &str);

  auto uri = mrb_nil_p(str) ? new HttpUri() : new HttpUri(mrb_str_to_stdstring(str));
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

static mrb_value mrb_http_headers_new(mrb_state *mrb, const HttpHeaderCollection &headers)
{
  static auto headers_c = mrb_http_get_class(mrb, "Headers");

  auto pHeaders = new HttpHeaderCollection(headers);
  auto obj = mrb_data_object_alloc(mrb, headers_c, pHeaders, &mrb_http_headers_dt);

  return mrb_obj_value(obj);
}

static mrb_value mrb_http_headers_each(mrb_state *mrb, mrb_value self)
{
  mrb_value block = mrb_nil_value();
  mrb_get_args(mrb, "|&", &block);

  if (mrb_nil_p(block))
  {
    static const auto to_enum = mrb_intern_lit(mrb, "to_enum");
    static const auto each = mrb_symbol_value(mrb_intern_lit(mrb, "each"));

    return mrb_funcall_argv(mrb, self, to_enum, 1, &each);
  }

  auto &headers = *static_cast<HttpHeaderCollection *>(mrb_data_get_ptr(mrb, self, &mrb_http_headers_dt));

  for (auto &pair : headers)
  {
    mrb_yield(mrb, block, mrb_http_header_set_new(mrb, headers[pair.first]));
  }

  return self;
}

static mrb_value mrb_http_headers_get(mrb_state *mrb, mrb_value self)
{
  mrb_value str;
  mrb_get_args(mrb, "S", &str);

  auto &headers = *static_cast<HttpHeaderCollection *>(mrb_data_get_ptr(mrb, self, &mrb_http_headers_dt));

  return mrb_http_header_set_new(mrb, headers[mrb_str_to_stdstring(str)]);
}

#pragma endregion

// ----------------------------------------------------------------------------

#pragma region Http::Headers::Set

static mrb_value mrb_http_header_set_new(mrb_state *mrb, const HttpHeaderSet &set)
{
  static auto set_c = mrb_http_get_class(mrb, "Headers", "Set");

  auto pSet = new HttpHeaderSet(set);
  auto obj = mrb_data_object_alloc(mrb, set_c, pSet, &mrb_http_header_set_dt);

  return mrb_obj_value(obj);
}

// ----------------------------------------------------------------------------

static mrb_value mrb_http_header_set_key(mrb_state *mrb, mrb_value self)
{
  auto &set = *static_cast<HttpHeaderSet *>(mrb_data_get_ptr(mrb, self, &mrb_http_header_set_dt));
  return mrb_str_new(mrb, set.Key.c_str(), set.Key.size());
}

// ----------------------------------------------------------------------------

static mrb_value mrb_http_header_set_each(mrb_state *mrb, mrb_value self)
{
  mrb_value block;
  mrb_get_args(mrb, "|&", &block);

  if (mrb_nil_p(block))
  {
    static const auto to_enum = mrb_intern_lit(mrb, "to_enum");
    static const auto each = mrb_symbol_value(mrb_intern_lit(mrb, "each"));

    return mrb_funcall_argv(mrb, self, to_enum, 1, &each);
  }

  auto &set = *static_cast<HttpHeaderSet *>(mrb_data_get_ptr(mrb, self, &mrb_http_header_set_dt));

  for (auto &item : set)
  {
    mrb_yield(mrb, block, mrb_str_new(mrb, item.Value.c_str(), item.Value.size()));
  }

  return self;
}

// ----------------------------------------------------------------------------

static mrb_value mrb_http_header_set_add(mrb_state *mrb, mrb_value self)
{
  mrb_value str;
  mrb_get_args(mrb, "S", &str);

  auto &set = *static_cast<HttpHeaderSet *>(mrb_data_get_ptr(mrb, self, &mrb_http_header_set_dt));
  set.AddValue(mrb_str_to_stdstring(str));

  return mrb_nil_value();
}

// ----------------------------------------------------------------------------

static mrb_value mrb_http_header_set_remove(mrb_state *mrb, mrb_value self)
{
  mrb_value str;
  mrb_get_args(mrb, "S", &str);

  auto &set = *static_cast<HttpHeaderSet *>(mrb_data_get_ptr(mrb, self, &mrb_http_header_set_dt));
  set.RemoveValue(mrb_str_to_stdstring(str));

  return mrb_nil_value();
}

// ----------------------------------------------------------------------------

static mrb_value mrb_http_header_set_clear(mrb_state *mrb, mrb_value self)
{
  auto &set = *static_cast<HttpHeaderSet *>(mrb_data_get_ptr(mrb, self, &mrb_http_header_set_dt));
  set.Clear();

  return mrb_nil_value();
}

#pragma endregion

// ----------------------------------------------------------------------------

#pragma region Http::Request

static mrb_value mrb_http_request_new(mrb_state *mrb, mrb_value)
{
  static auto req_c = mrb_http_get_class(mrb, "Request");

  mrb_value uri;
  mrb_int verb;
  mrb_value block = mrb_nil_value();
  mrb_get_args(mrb, "oi&", &uri, &verb, &block);

  HttpUri cpp_uri;

  if (mrb_string_p(uri))
  {
    cpp_uri = HttpUri(mrb_str_to_stdstring(uri));
  }
  else
  {
    cpp_uri = *static_cast<HttpUri *>(mrb_data_get_ptr(mrb, uri, &mrb_http_uri_dt));
  }

  auto request = new HttpRequest(cpp_uri, static_cast<HttpMethod>(verb));
  auto obj = mrb_data_object_alloc(mrb, req_c, request, &mrb_http_request_dt);
  auto req = mrb_obj_value(obj);

  if (!mrb_nil_p(block))
  {
    mrb_yield(mrb, block, req);
  }

  return req;
}

// ----------------------------------------------------------------------------

static mrb_value mrb_http_request_body(mrb_state *mrb, mrb_value self)
{
  auto &request = *static_cast<HttpRequest *>(mrb_data_get_ptr(mrb, self, &mrb_http_request_dt));
  return mrb_http_request_body_new(mrb, request.Body);
}

// ----------------------------------------------------------------------------

static mrb_value mrb_http_request_headers(mrb_state *mrb, mrb_value self)
{
  auto &request = *static_cast<HttpRequest *>(mrb_data_get_ptr(mrb, self, &mrb_http_request_dt));
  return mrb_http_headers_new(mrb, request.Headers);
}

#pragma endregion

// ----------------------------------------------------------------------------

#pragma region Http::Request::Body

static mrb_value mrb_http_request_body_new(mrb_state *mrb, const HttpRequestBody &body)
{
  static auto body_c = mrb_http_get_class(mrb, "Request", "Body");

  auto pBody = new HttpRequestBody(body);
  auto obj = mrb_data_object_alloc(mrb, body_c, pBody, &mrb_http_request_body_dt);

  return mrb_obj_value(obj);
}

// ----------------------------------------------------------------------------

static mrb_value mrb_http_request_body_json_set(mrb_state *mrb, mrb_value self)
{
  mrb_value data;
  mrb_get_args(mrb, "o", &data);

  json::value jval;
  try
  {
    jval = mrb_inst->value_to_json(data);
  }
  catch (std::exception &ex)
  {
    mrb_raise(mrb, mrb->eException_class, ex.what());
  }

  auto &body = *static_cast<HttpRequestBody *>(mrb_data_get_ptr(mrb, self, &mrb_http_request_body_dt));
  body.SetJson(jval);

  return mrb_nil_value();
}

// ----------------------------------------------------------------------------

static mrb_value mrb_http_request_body_string_set(mrb_state *mrb, mrb_value self)
{
  mrb_value str;
  mrb_get_args(mrb, "S", &str);

  auto &body = *static_cast<HttpRequestBody *>(mrb_data_get_ptr(mrb, self, &mrb_http_request_body_dt));
  body.SetString(mrb_str_to_stdstring(str));

  return mrb_nil_value();
}

// ----------------------------------------------------------------------------

static mrb_value mrb_http_request_body_form_set(mrb_state *mrb, mrb_value self)
{
  mrb_value data;
  mrb_get_args(mrb, "o", &data);

  json::value jval;
  try
  {
    jval = mrb_inst->value_to_json(data);
  }
  catch (std::exception &ex)
  {
    mrb_raise(mrb, mrb->eException_class, ex.what());
  }

  if (!jval.is_object_of<json::value::string_t>())
  {
    jval = json::value(std::_Noinit);
    mrb_raise(mrb, mrb->eException_class, "Form must be a Hash containing Strings");
  }

  auto &body = *static_cast<HttpRequestBody *>(mrb_data_get_ptr(mrb, self, &mrb_http_request_body_dt));
  body.SetForm(jval.as_object_of<json::value::string_t>());

  return mrb_nil_value();
}

// ----------------------------------------------------------------------------

static mrb_value mrb_http_request_body_type_set(mrb_state *mrb, mrb_value self)
{
  mrb_value str;
  mrb_get_args(mrb, "S", &str);

  auto &body = *static_cast<HttpRequestBody *>(mrb_data_get_ptr(mrb, self, &mrb_http_request_body_dt));
  body.SetContentType(mrb_str_to_stdstring(str));

  return mrb_nil_value();
}

#pragma endregion

// ----------------------------------------------------------------------------

#pragma region Http::Result

static mrb_value mrb_http_result_headers(mrb_state *mrb, mrb_value self)
{
  auto &result = *static_cast<MrbHttpResult *>(mrb_data_get_ptr(mrb, self, &mrb_http_result_dt));
  return mrb_http_headers_new(mrb, result.result.Headers);
}

// ----------------------------------------------------------------------------

static mrb_value mrb_http_result_as_json(mrb_state *mrb, mrb_value self)
{
  auto &result = *static_cast<MrbHttpResult *>(mrb_data_get_ptr(mrb, self, &mrb_http_result_dt));
  auto json = result.result.AsJson;

  return mrb_inst->json_to_value(json);
}

// ----------------------------------------------------------------------------

static mrb_value mrb_http_result_as_string(mrb_state *mrb, mrb_value self)
{
  auto &result = *static_cast<MrbHttpResult *>(mrb_data_get_ptr(mrb, self, &mrb_http_result_dt));

  if (result.result.HasFailed)
    mrb_raise(mrb, mrb->eException_class, "HTTP Request failed, cannot get data");

  auto &str = *result.result.AsString;
  return mrb_str_new(mrb, str.c_str(), str.size());
}

// ----------------------------------------------------------------------------

static mrb_value mrb_http_result_headers_p(mrb_state *mrb, mrb_value self)
{
  auto &result = *static_cast<MrbHttpResult *>(mrb_data_get_ptr(mrb, self, &mrb_http_result_dt));
  return mrb_bool_value(result.result.HasHeaders);
}

// ----------------------------------------------------------------------------

static mrb_value mrb_http_result_data_p(mrb_state *mrb, mrb_value self)
{
  auto &result = *static_cast<MrbHttpResult *>(mrb_data_get_ptr(mrb, self, &mrb_http_result_dt));
  return mrb_bool_value(result.result.HasData);
}

// ----------------------------------------------------------------------------

static mrb_value mrb_http_result_failed_p(mrb_state *mrb, mrb_value self)
{
  auto &result = *static_cast<MrbHttpResult *>(mrb_data_get_ptr(mrb, self, &mrb_http_result_dt));
  return mrb_bool_value(result.result.HasFailed);
}

#pragma endregion

// ----------------------------------------------------------------------------

#pragma region Http::Result::LineReader


#pragma endregion

// ----------------------------------------------------------------------------