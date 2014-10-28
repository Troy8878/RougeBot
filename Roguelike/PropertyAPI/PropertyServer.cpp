/*********************************
 * PropertyServer.cpp
 * Connor Hilarides
 * Created 2014/10/27
 *********************************/

#include "PropertyServer.h"
#include "Engine/Common.h"
#include "Engine/Level.h"

using namespace web::http;
using namespace web::http::experimental;

// ----------------------------------------------------------------------------

PropertyServer::PropertyServer()
{
  listener = listener::http_listener(U("http://localhost:5431/"));

  using namespace std::placeholders;

  listener.support(methods::GET, std::bind(&PropertyServer::handle_get, this, _1));
  listener.support(methods::PUT, std::bind(&PropertyServer::handle_put, this, _1));

  listener.open().then([]()
  {
    std::cout << "Listener opened!";
  });
}

// ----------------------------------------------------------------------------

PropertyServer::~PropertyServer()
{
  listener.close();
}

// ----------------------------------------------------------------------------

void PropertyServer::handle_get(http_request request)
{
  auto path = request.request_uri().path();
  if (path.find_first_of(L"/entity/") == 0)
  {
    entity_get(request);
  }
  else
  {
    http_response response(status_codes::OK);
    response.set_body(":O");
    request.reply(response);
  }
}

// ----------------------------------------------------------------------------

void PropertyServer::handle_put(http_request request)
{
  request.reply(status_codes::OK, U("Hello, World!"));
}

// ----------------------------------------------------------------------------

void PropertyServer::entity_get(http_request request)
{
  auto uri = request.request_uri();
  auto path = narrow(uri.path());
  auto path_parts = split(path, '/');
  path_parts.erase(path_parts.begin());

  if (path_parts.size() < 3)
  {
    request.reply(status_codes::NotFound);
    return;
  }
  
  json::value data = json::value::object();
  Entity *entity = nullptr;
  if (path_parts[1] == "id")
  {
    entity_id id = std::stoull(path_parts[2]);
    entity = GetGame()->CurrentLevel->RootEntity->FindEntity(id);
  }
  else if (path_parts[1] == "name")
  {
    entity = GetGame()->CurrentLevel->RootEntity->FindEntity(path_parts[2]);
  }
  
  if (!entity)
  {
    request.reply(status_codes::NotFound);
    return;
  }

  data["id"] = json::value::number((long double) entity->Id);
  data["name"] = json::value::string(entity->Name);

  auto component_list = json::value::array();
  auto& comp_list = component_list.as_array();
  for (auto& pair : entity->Components)
  {
    comp_list.push_back(json::value::string(pair.first));
  }
  data["components"] = component_list;

  auto children_list = json::value::array();
  auto& child_list = children_list.as_array();
  for (auto *child : entity->Children)
  {
    child_list.push_back(json::value::number((long double) child->Id));
  }
  data["children"] = children_list;

#ifdef _DEBUG
  std::ostringstream oss;
  data.pretty_print(oss);
  auto res_str = oss.str();
#else
  auto res_str = data.serialize();
#endif

  auto stream = concurrency::streams::bytestream::open_istream(res_str);
  http_response response(status_codes::OK);
  response.set_body(stream, L"application/json");
  request.reply(response);
}

// ----------------------------------------------------------------------------

