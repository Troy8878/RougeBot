/*********************************
 * Main.cpp
 * Connor Hilarides
 * Created 2014/05/28
 *********************************/

#include "Common.h"
#include "Engine/HttpClient.h"

static void CreateConsole()
{
  AllocConsole();
  FILE *file;
  freopen_s(&file, "CONOUT$", "wt", stdout);
  freopen_s(&file, "CONOUT$", "wt", stderr);
  freopen_s(&file, "CONIN$", "rt", stdin);
}

Roguelike game("Game 200 Project", GetModuleHandle(NULL));

extern "C" int IsAvxSupported();

INT WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, INT)
{
  performance::register_guard glperf("The Game");
  
  CreateConsole();

  std::cout << console::fg::white;
  std::cout << "AVX Support: " << IsAvxSupported() << std::endl;

#if defined(_DEBUG)
  // HTTP test
  {
    HttpUri uri("http://www.cloudsdale.org/v1/sessions");
    HttpClient client;
    HttpRequest request(uri, HTTP_GET);
    request.Headers["Accept"].AddValue("application/json");

    using namespace json;
    request.Body.SetJson(value::object(
    {
      {"email", value("connorcpu@cloudsdale.org")},
      {"password", value("not my password")}
    }));

    auto res = client.MakeRequest(request);

    while (!res.HasData)
      Sleep(10);

    auto jval = res.AsJson;
    jval.pretty_print(std::cout);
    std::cout << std::endl;
  }
#endif

  game.Run();
  return 0;
}
