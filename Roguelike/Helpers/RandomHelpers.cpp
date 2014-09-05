/*********************************
 * RandomHelpers.cpp
 * Connor Hilarides
 * Created 2014/08/20
 *********************************/

#include "Engine/Common.h"

#include <conio.h>

// ----------------------------------------------------------------------------

json::value ParseJsonAsset(const std::string& containerName, const std::string& asset)
{
  auto container = GetGame()->Respack[containerName];
  RELEASE_AFTER_SCOPE(container);

  auto resource = container->GetResource(asset);
  RELEASE_AFTER_SCOPE(resource);

  return json::value::parse(resource->Stream);
}

// ----------------------------------------------------------------------------

bool stop_getline_async = false;

bool getline_async(std::string& str,
                   std::chrono::system_clock::duration timeout)
{
  auto start = std::chrono::system_clock::now();
  bool has_any = false;

  for (;;)
  {
    if (stop_getline_async)
      return false;

    if (!has_any && std::chrono::system_clock::now() - start > timeout)
      return false;

    if (_kbhit())
    {
      char c = (char)_getche();

      has_any = true;

      if (c == '\r')
      {
        str += '\n';
        _putch('\n');
        return true;
      }
      else if (c == '\b')
      {
        str.pop_back();
        _putch(' ');
        _putch('\b');
      }
      else
      {
        str += c;
      }
    }
    else
    {
      Sleep(0);
    }
  }
}

// ----------------------------------------------------------------------------


