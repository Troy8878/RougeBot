/*********************************
 * PropertyServer.h
 * Connor Hilarides
 * Created 2014/10/27
 *********************************/

#pragma once

#include <cpprest/http_server.h>

// ----------------------------------------------------------------------------

class PropertyServer
{
public:
  PropertyServer();
  ~PropertyServer();

private:
  web::http::experimental::listener::http_listener listener;

  void handle_get(web::http::http_request request);
  void handle_put(web::http::http_request request);

  void entity_get(web::http::http_request request);
};

// ----------------------------------------------------------------------------
