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

D2D1::ColorF StringToColor(const std::string& name)
{
  #pragma region Named Colors
  const static std::unordered_map<std::string, D2D1::ColorF::Enum> namedColors =
  {
    {"AliceBlue", D2D1::ColorF::AliceBlue},
    {"AntiqueWhite", D2D1::ColorF::AntiqueWhite},
    {"Aqua", D2D1::ColorF::Aqua},
    {"Aquamarine", D2D1::ColorF::Aquamarine},
    {"Azure", D2D1::ColorF::Azure},
    {"Beige", D2D1::ColorF::Beige},
    {"Bisque", D2D1::ColorF::Bisque},
    {"Black", D2D1::ColorF::Black},
    {"BlanchedAlmond", D2D1::ColorF::BlanchedAlmond},
    {"Blue", D2D1::ColorF::Blue},
    {"BlueViolet", D2D1::ColorF::BlueViolet},
    {"Brown", D2D1::ColorF::Brown},
    {"BurlyWood", D2D1::ColorF::BurlyWood},
    {"CadetBlue", D2D1::ColorF::CadetBlue},
    {"Chartreuse", D2D1::ColorF::Chartreuse},
    {"Chocolate", D2D1::ColorF::Chocolate},
    {"Coral", D2D1::ColorF::Coral},
    {"CornflowerBlue", D2D1::ColorF::CornflowerBlue},
    {"Cornsilk", D2D1::ColorF::Cornsilk},
    {"Crimson", D2D1::ColorF::Crimson},
    {"Cyan", D2D1::ColorF::Cyan},
    {"DarkBlue", D2D1::ColorF::DarkBlue},
    {"DarkCyan", D2D1::ColorF::DarkCyan},
    {"DarkGoldenrod", D2D1::ColorF::DarkGoldenrod},
    {"DarkGray", D2D1::ColorF::DarkGray},
    {"DarkGreen", D2D1::ColorF::DarkGreen},
    {"DarkKhaki", D2D1::ColorF::DarkKhaki},
    {"DarkMagenta", D2D1::ColorF::DarkMagenta},
    {"DarkOliveGreen", D2D1::ColorF::DarkOliveGreen},
    {"DarkOrange", D2D1::ColorF::DarkOrange},
    {"DarkOrchid", D2D1::ColorF::DarkOrchid},
    {"DarkRed", D2D1::ColorF::DarkRed},
    {"DarkSalmon", D2D1::ColorF::DarkSalmon},
    {"DarkSeaGreen", D2D1::ColorF::DarkSeaGreen},
    {"DarkSlateBlue", D2D1::ColorF::DarkSlateBlue},
    {"DarkSlateGray", D2D1::ColorF::DarkSlateGray},
    {"DarkTurquoise", D2D1::ColorF::DarkTurquoise},
    {"DarkViolet", D2D1::ColorF::DarkViolet},
    {"DeepPink", D2D1::ColorF::DeepPink},
    {"DeepSkyBlue", D2D1::ColorF::DeepSkyBlue},
    {"DimGray", D2D1::ColorF::DimGray},
    {"DodgerBlue", D2D1::ColorF::DodgerBlue},
    {"Firebrick", D2D1::ColorF::Firebrick},
    {"FloralWhite", D2D1::ColorF::FloralWhite},
    {"ForestGreen", D2D1::ColorF::ForestGreen},
    {"Fuchsia", D2D1::ColorF::Fuchsia},
    {"Gainsboro", D2D1::ColorF::Gainsboro},
    {"GhostWhite", D2D1::ColorF::GhostWhite},
    {"Gold", D2D1::ColorF::Gold},
    {"Goldenrod", D2D1::ColorF::Goldenrod},
    {"Gray", D2D1::ColorF::Gray},
    {"Green", D2D1::ColorF::Green},
    {"GreenYellow", D2D1::ColorF::GreenYellow},
    {"Honeydew", D2D1::ColorF::Honeydew},
    {"HotPink", D2D1::ColorF::HotPink},
    {"IndianRed", D2D1::ColorF::IndianRed},
    {"Indigo", D2D1::ColorF::Indigo},
    {"Ivory", D2D1::ColorF::Ivory},
    {"Khaki", D2D1::ColorF::Khaki},
    {"Lavender", D2D1::ColorF::Lavender},
    {"LavenderBlush", D2D1::ColorF::LavenderBlush},
    {"LawnGreen", D2D1::ColorF::LawnGreen},
    {"LemonChiffon", D2D1::ColorF::LemonChiffon},
    {"LightBlue", D2D1::ColorF::LightBlue},
    {"LightCoral", D2D1::ColorF::LightCoral},
    {"LightCyan", D2D1::ColorF::LightCyan},
    {"LightGoldenrodYellow", D2D1::ColorF::LightGoldenrodYellow},
    {"LightGreen", D2D1::ColorF::LightGreen},
    {"LightGray", D2D1::ColorF::LightGray},
    {"LightPink", D2D1::ColorF::LightPink},
    {"LightSalmon", D2D1::ColorF::LightSalmon},
    {"LightSeaGreen", D2D1::ColorF::LightSeaGreen},
    {"LightSkyBlue", D2D1::ColorF::LightSkyBlue},
    {"LightSlateGray", D2D1::ColorF::LightSlateGray},
    {"LightSteelBlue", D2D1::ColorF::LightSteelBlue},
    {"LightYellow", D2D1::ColorF::LightYellow},
    {"Lime", D2D1::ColorF::Lime},
    {"LimeGreen", D2D1::ColorF::LimeGreen},
    {"Linen", D2D1::ColorF::Linen},
    {"Magenta", D2D1::ColorF::Magenta},
    {"Maroon", D2D1::ColorF::Maroon},
    {"MediumAquamarine", D2D1::ColorF::MediumAquamarine},
    {"MediumBlue", D2D1::ColorF::MediumBlue},
    {"MediumOrchid", D2D1::ColorF::MediumOrchid},
    {"MediumPurple", D2D1::ColorF::MediumPurple},
    {"MediumSeaGreen", D2D1::ColorF::MediumSeaGreen},
    {"MediumSlateBlue", D2D1::ColorF::MediumSlateBlue},
    {"MediumSpringGreen", D2D1::ColorF::MediumSpringGreen},
    {"MediumTurquoise", D2D1::ColorF::MediumTurquoise},
    {"MediumVioletRed", D2D1::ColorF::MediumVioletRed},
    {"MidnightBlue", D2D1::ColorF::MidnightBlue},
    {"MintCream", D2D1::ColorF::MintCream},
    {"MistyRose", D2D1::ColorF::MistyRose},
    {"Moccasin", D2D1::ColorF::Moccasin},
    {"NavajoWhite", D2D1::ColorF::NavajoWhite},
    {"Navy", D2D1::ColorF::Navy},
    {"OldLace", D2D1::ColorF::OldLace},
    {"Olive", D2D1::ColorF::Olive},
    {"OliveDrab", D2D1::ColorF::OliveDrab},
    {"Orange", D2D1::ColorF::Orange},
    {"OrangeRed", D2D1::ColorF::OrangeRed},
    {"Orchid", D2D1::ColorF::Orchid},
    {"PaleGoldenrod", D2D1::ColorF::PaleGoldenrod},
    {"PaleGreen", D2D1::ColorF::PaleGreen},
    {"PaleTurquoise", D2D1::ColorF::PaleTurquoise},
    {"PaleVioletRed", D2D1::ColorF::PaleVioletRed},
    {"PapayaWhip", D2D1::ColorF::PapayaWhip},
    {"PeachPuff", D2D1::ColorF::PeachPuff},
    {"Peru", D2D1::ColorF::Peru},
    {"Pink", D2D1::ColorF::Pink},
    {"Plum", D2D1::ColorF::Plum},
    {"PowderBlue", D2D1::ColorF::PowderBlue},
    {"Purple", D2D1::ColorF::Purple},
    {"Red", D2D1::ColorF::Red},
    {"RosyBrown", D2D1::ColorF::RosyBrown},
    {"RoyalBlue", D2D1::ColorF::RoyalBlue},
    {"SaddleBrown", D2D1::ColorF::SaddleBrown},
    {"Salmon", D2D1::ColorF::Salmon},
    {"SandyBrown", D2D1::ColorF::SandyBrown},
    {"SeaGreen", D2D1::ColorF::SeaGreen},
    {"SeaShell", D2D1::ColorF::SeaShell},
    {"Sienna", D2D1::ColorF::Sienna},
    {"Silver", D2D1::ColorF::Silver},
    {"SkyBlue", D2D1::ColorF::SkyBlue},
    {"SlateBlue", D2D1::ColorF::SlateBlue},
    {"SlateGray", D2D1::ColorF::SlateGray},
    {"Snow", D2D1::ColorF::Snow},
    {"SpringGreen", D2D1::ColorF::SpringGreen},
    {"SteelBlue", D2D1::ColorF::SteelBlue},
    {"Tan", D2D1::ColorF::Tan},
    {"Teal", D2D1::ColorF::Teal},
    {"Thistle", D2D1::ColorF::Thistle},
    {"Tomato", D2D1::ColorF::Tomato},
    {"Turquoise", D2D1::ColorF::Turquoise},
    {"Violet", D2D1::ColorF::Violet},
    {"Wheat", D2D1::ColorF::Wheat},
    {"White", D2D1::ColorF::White},
    {"WhiteSmoke", D2D1::ColorF::WhiteSmoke},
    {"Yellow", D2D1::ColorF::Yellow},
    {"YellowGreen", D2D1::ColorF::YellowGreen},
  };
  #pragma endregion

  D2D1::ColorF color{0};

  auto it = namedColors.end();
  if (name.find('#') == 0 && name.size() >= 7)
  {
    color.r = std::stoul(name.substr(1, 2), 0, 16) / 255.0f;
    color.g = std::stoul(name.substr(3, 2), 0, 16) / 255.0f;
    color.b = std::stoul(name.substr(5, 2), 0, 16) / 255.0f;

    if (name.size() >= 9)
      color.a = std::stoul(name.substr(7, 2), 0, 16) / 255.0f;
  }
  if ((it = namedColors.find(name)) != namedColors.end())
  {
    color = it->second;
  }

  return color;
}

// ----------------------------------------------------------------------------


