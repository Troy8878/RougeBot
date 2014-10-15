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

  std::cout << "AVX Support: " << IsAvxSupported() << std::endl;

  HttpUri uri("http://www.cloudsdale.org/v1/clouds/hammock");
  HttpClient client;
  HttpRequest request(uri, HTTP_GET);
  request.Headers["Accept"].AddValue("application/json");
  auto res = client.MakeRequest(request);

  game.Run();
  return 0;
}
