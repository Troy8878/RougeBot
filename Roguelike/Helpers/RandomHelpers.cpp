/*********************************
 * RandomHelpers.cpp
 * Connor Hilarides
 * Created 2014/08/20
 *********************************/

#include "Engine/Common.h"
#include "Engine/RenderSet.h"
#include "Engine/Camera.h"

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
  static std::vector<std::string> message_list;
  size_t message_index = message_list.size();
  auto start = std::chrono::system_clock::now();
  bool has_any = false;
  size_t pos = 0;
  size_t prev_len;
  HWND hwnd = GetGame()->GameDevice->GetContextWindow();
  const char *notif_msg = nullptr;
  char unknown_modifier[] = "Unknown modifier key: X";

  str.clear();

  for (;;)
  {
    if (stop_getline_async)
      return false;

    if (!has_any && std::chrono::system_clock::now() - start > timeout)
      return false;

    bool hit = false;

    if (_kbhit())
    {
      prev_len = str.size();
      has_any = true;
      hit = true;

      char c = (char)_getch();

      if (c == '\r') // [ENTER]
      {
        _putch('\n');

        message_list.push_back(str);
        return true;
      }
      else if (c == '\b') // [BACKSPACE]
      {
        if (pos)
        {
          str.erase(--pos, 1);
        }
      }
      else if (c >= 0x20 && c <= 0x7F) // Normal letters and symbols I can just print
      {
        str.insert(pos++, 1, c);
      }
      else
      {
        if (c == -32 || c == 0) // special movement keys
        {
          char m = (char)_getch();

          switch (m)
          {
            case 'K': // [LEFT]
              if (pos)
                pos--;
              break;

            case 'M': // [RIGHT]
              if (pos < str.size())
                pos++;
              break;

            case 'H': // [UP]
              if (message_index)
                str = message_list[--message_index];
              pos = str.size();
              break;

            case 'P': // [DOWN]
              if (message_list.size() && message_index < message_list.size() - 1)
                str = message_list[++message_index];
              pos = str.size();
              break;

            case 'G': // [HOME]
              pos = 0;
              break;

            case 'O': // [END]
              pos = str.size();
              break;

            case 'S': // [DEL]
              if (pos < str.size())
                str.erase(pos, 1);
              break;

            case 'R': // [INS]
            case 'I': // [PGUP]
            case 'Q': // [PGDOWN]
              break;

            default: // for detecting keys I've missed
              unknown_modifier[ARRAYSIZE(unknown_modifier) - 2] = m;
              notif_msg = unknown_modifier;
          }
        }
        else if (c == 0x1b) // [ESC]
        {
          has_any = false;
          str = "";
          pos = 0;
        }
        else if (c == 0x18) // Ctrl-X
        {
          if (OpenClipboard(hwnd))
          {
            EmptyClipboard();

            const size_t mem_size = str.size() + 1;
            auto cbcopy = GlobalAlloc(GMEM_MOVEABLE, mem_size);

            if (cbcopy)
            {
              auto copymem = GlobalLock(cbcopy);
              memcpy_s(copymem, mem_size, str.c_str(), mem_size);
              GlobalUnlock(cbcopy);
            
              SetClipboardData(CF_TEXT, cbcopy);

              notif_msg = "[Copied to Clipboard]";
            }

            CloseClipboard();
          }
        }
        else if (c == 0x16) // Ctrl-V
        {
          if (IsClipboardFormatAvailable(CF_TEXT) && OpenClipboard(hwnd))
          {
            auto htext = GetClipboardData(CF_TEXT);
            if (htext)
            {
              auto ctext = (const char *) GlobalLock(htext);
              if (ctext)
              {
                str = ctext;
                pos = str.size();
                has_any = true;
              }
            }

            CloseClipboard();
          }
        }

        if (_kbhit())
          _getch();
      }

      // Fix the line, cbb to write the logic for printing chars above
      HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
      CONSOLE_SCREEN_BUFFER_INFO info;
      GetConsoleScreenBufferInfo(console, &info);
      COORD cpos = info.dwCursorPosition;

      cpos.Y -= static_cast<short>((prev_len + 2) / info.dwSize.X);

      if (prev_len > str.size())
      {
        cpos.X = 0; // Set the cursor back to the far left
        SetConsoleCursorPosition(console, cpos);

        for (size_t i = 0; i < prev_len + 2; ++i)
          _putch(' '); // Empty out the line

        cpos.X = 0; // Set the cursor back to the far left
        SetConsoleCursorPosition(console, cpos);
      }

      if (notif_msg)
      {
        puts(notif_msg);
        cpos.Y++;
        notif_msg = nullptr;
      }

      if (str.size())
      {
        cpos.X = 0; // Set the cursor back to the far left
        SetConsoleCursorPosition(console, cpos);
        _cputs("> ");
      }

      _cputs(str.c_str()); // Print out the contents

      // Put the cursor at the current text position
      cpos.X = (short) pos + (str.size() ? 2 : 0);
      SetConsoleCursorPosition(console, cpos);
    }
    else
    {
      Sleep(5); // Give up my time slice so I'm not just wasting cycles
    }
  }
}

// ----------------------------------------------------------------------------

math::Vector __vectorcall ScreenToPlane(DirectX::FXMVECTOR point, 
                                        DirectX::FXMVECTOR planeOrigin, 
                                        DirectX::FXMVECTOR planeNormal, 
                                        Camera *camera)
{
  auto winsz = GetGame()->GameDevice->GetSize();

  using namespace DirectX;
  auto viewInverse = XMMatrixInverse(nullptr, camera->viewMatrix);

  auto planePoint = XMVector3Unproject(point, 
                                       0, 0, winsz.x, winsz.y, 0.0f, 1.0f, 
                                       camera->projectionMatrix, 
                                       camera->viewMatrix, 
                                       XMMatrixIdentity());

  XMVECTOR c = viewInverse * g_XMIdentityR3;
  XMVECTOR v = c - planePoint;
  XMVECTOR n = planeNormal;
  XMMATRIX vn;
  vn.r[0] = v * XMVectorGetX(n);
  vn.r[1] = v * XMVectorGetY(n);
  vn.r[2] = v * XMVectorGetZ(n);
  vn.r[3] = v * XMVectorGetW(n);

  XMVECTOR d = XMVector4Dot(v, n);
  XMMATRIX oblique;
  oblique.r[0] = g_XMIdentityR0 - vn.r[0] / d;
  oblique.r[1] = g_XMIdentityR1 - vn.r[1] / d;
  oblique.r[2] = g_XMIdentityR2 - vn.r[2] / d;
  oblique.r[3] = g_XMIdentityR3 - vn.r[3] / d;

  oblique = XMMatrixTranslationFromVector(-planeOrigin) * 
            oblique *
            XMMatrixTranslationFromVector(planeOrigin);
  
  return oblique * planePoint;
}

// ----------------------------------------------------------------------------


