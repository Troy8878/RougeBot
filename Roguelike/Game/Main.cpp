/*********************************
 * Main.cpp
 * Connor Hilarides
 * Created 2014/05/28
 *********************************/

#include "Game\Roguelike.h"
#include "Helpers\Console.h"
#include "Helpers\FileSystem.h"
#include "Helpers\FlatMap.h"
#include "Helpers\StackTrace.h"

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

  Roguelike game("Game 200 Project", hInstance);
  game.run();
}
