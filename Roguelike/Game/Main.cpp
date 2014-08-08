/*********************************
 * Main.cpp
 * Connor Hilarides
 * Created 2014/05/28
 *********************************/

#include "Game\Roguelike.h"
#include "Helpers\Console.h"
#include "Helpers\FileSystem.h"

#pragma comment(lib, "dxguid.lib")

static void createConsole()
{
  AllocConsole();
  FILE *file;
  freopen_s(&file, "CONOUT$", "wt", stdout);
  freopen_s(&file, "CONOUT$", "wt", stderr);
  freopen_s(&file, "CONIN$", "rt", stdin);
}

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, INT)
{
  IFDEBUG(createConsole());
  IFDEBUG(debug << console::fg::red << "Starting game!" << std::endl);
  IFDEBUG(debug.flush());

#ifdef _DEBUG
  fs::wpath path = L".";
  std::wcout << L"Contents of " << path.directory_string() << L":" << std::endl;
  for (auto file : fs::directory_contents{path})
  {
    std::wcout << file.file_string() << std::endl;
  }
#endif

  Roguelike game("Game 200 Project", hInstance);
  game.run();
}
