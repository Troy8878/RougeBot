/*********************************
 * RandomHelpers.cpp
 * Connor Hilarides
 * Created 2014/08/20
 * Copyright © 2014 DigiPen Institute of Technology, All Rights Reserved
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

    if (_kbhit())
    {
      prev_len = str.size();
      has_any = true;

      char c = static_cast<char>(_getch());

      if (c == '\r') // [ENTER]
      {
        _putch('\n');

        message_list.push_back(str);
        return true;
      }
      if (c == '\b') // [BACKSPACE]
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
          char m = static_cast<char>(_getch());

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
              unknown_modifier[ARRAYSIZE(reinterpret_cast<const char(&)[24]>(unknown_modifier)) - 2] = m;
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
              auto ctext = static_cast<const char *>(GlobalLock(htext));
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
      cpos.X = static_cast<short>(pos) + (str.size() ? 2 : 0);
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
                                        Camera *camera, float *distance)
{
  auto winsz = GetGame()->GameDevice->GetSize();

  using namespace DirectX;
  auto viewInverse = XMMatrixInverse(nullptr, camera->viewMatrix);

  auto planePoint = XMVector3Unproject(point,
                                       0, 0, winsz.x, winsz.y, 0.1f, 1.0f,
                                       camera->projectionMatrix,
                                       camera->viewMatrix, 
                                       XMMatrixIdentity());

  XMVECTOR c = viewInverse * g_XMIdentityR3;
  XMVECTOR v = c - planePoint;
  XMVECTOR n = planeNormal;
  XMVECTOR d = XMVector4Dot(v, n);

  XMMATRIX vn = XMMatrixFromVectorTimesVectorTranspose(v, n);
  XMMATRIX oblique = 
    XMMatrixTranslationFromVector(-planeOrigin) *
    XMMatrixSubDiv(XMMatrixIdentity(), vn, d) *
    XMMatrixTranslationFromVector(planeOrigin);
  
  XMVECTOR projected = oblique * planePoint;

  if (distance != nullptr)
  {
    XMVECTOR diff = projected - planePoint;
    *distance = XMVectorGetX(XMVector3Length(diff));

    if (XMVectorGetX(XMVectorAbs(XMVector3AngleBetweenVectors(diff, v))) > math::pi / 2)
    {
      *distance = -*distance;
    }
  }

  return projected;
}

// ----------------------------------------------------------------------------

D2D1::ColorF JsonToColor(json::value json)
{
  if (json.is(json::json_type::jnull))
    return math::Vector(0, 0, 0, 0);

  D2D1::ColorF color(D2D1::ColorF::White);
  if (json.is(json::json_type::jstring))
  {
    color = StringToColor(json.as_string());
  }
  else if (json.is_array_of<json::value::number_t>())
  {
    auto ary = json.as_array_of<json::value::number_t>();
    if (ary.size() >= 1)
      color.r = static_cast<float>(ary[0]);
    if (ary.size() >= 2)
      color.g = static_cast<float>(ary[1]);
    if (ary.size() >= 3)
      color.b = static_cast<float>(ary[2]);
    if (ary.size() >= 4)
      color.a = static_cast<float>(ary[3]);
  }
  return color;
}

// ----------------------------------------------------------------------------

D2D1::ColorF StringToColor(const std::string& name)
{
  #pragma region Named Colors
  const static std::unordered_map<std::string, D2D1::ColorF::Enum> namedColors =
  {
    {downcase("AliceBlue"),             D2D1::ColorF::AliceBlue},
    {downcase("AntiqueWhite"),          D2D1::ColorF::AntiqueWhite},
    {downcase("Aqua"),                  D2D1::ColorF::Aqua},
    {downcase("Aquamarine"),            D2D1::ColorF::Aquamarine},
    {downcase("Azure"),                 D2D1::ColorF::Azure},
    {downcase("Beige"),                 D2D1::ColorF::Beige},
    {downcase("Bisque"),                D2D1::ColorF::Bisque},
    {downcase("Black"),                 D2D1::ColorF::Black},
    {downcase("BlanchedAlmond"),        D2D1::ColorF::BlanchedAlmond},
    {downcase("Blue"),                  D2D1::ColorF::Blue},
    {downcase("BlueViolet"),            D2D1::ColorF::BlueViolet},
    {downcase("Brown"),                 D2D1::ColorF::Brown},
    {downcase("BurlyWood"),             D2D1::ColorF::BurlyWood},
    {downcase("CadetBlue"),             D2D1::ColorF::CadetBlue},
    {downcase("Chartreuse"),            D2D1::ColorF::Chartreuse},
    {downcase("Chocolate"),             D2D1::ColorF::Chocolate},
    {downcase("Coral"),                 D2D1::ColorF::Coral},
    {downcase("CornflowerBlue"),        D2D1::ColorF::CornflowerBlue},
    {downcase("Cornsilk"),              D2D1::ColorF::Cornsilk},
    {downcase("Crimson"),               D2D1::ColorF::Crimson},
    {downcase("Cyan"),                  D2D1::ColorF::Cyan},
    {downcase("DarkBlue"),              D2D1::ColorF::DarkBlue},
    {downcase("DarkCyan"),              D2D1::ColorF::DarkCyan},
    {downcase("DarkGoldenrod"),         D2D1::ColorF::DarkGoldenrod},
    {downcase("DarkGray"),              D2D1::ColorF::DarkGray},
    {downcase("DarkGreen"),             D2D1::ColorF::DarkGreen},
    {downcase("DarkKhaki"),             D2D1::ColorF::DarkKhaki},
    {downcase("DarkMagenta"),           D2D1::ColorF::DarkMagenta},
    {downcase("DarkOliveGreen"),        D2D1::ColorF::DarkOliveGreen},
    {downcase("DarkOrange"),            D2D1::ColorF::DarkOrange},
    {downcase("DarkOrchid"),            D2D1::ColorF::DarkOrchid},
    {downcase("DarkRed"),               D2D1::ColorF::DarkRed},
    {downcase("DarkSalmon"),            D2D1::ColorF::DarkSalmon},
    {downcase("DarkSeaGreen"),          D2D1::ColorF::DarkSeaGreen},
    {downcase("DarkSlateBlue"),         D2D1::ColorF::DarkSlateBlue},
    {downcase("DarkSlateGray"),         D2D1::ColorF::DarkSlateGray},
    {downcase("DarkTurquoise"),         D2D1::ColorF::DarkTurquoise},
    {downcase("DarkViolet"),            D2D1::ColorF::DarkViolet},
    {downcase("DeepPink"),              D2D1::ColorF::DeepPink},
    {downcase("DeepSkyBlue"),           D2D1::ColorF::DeepSkyBlue},
    {downcase("DimGray"),               D2D1::ColorF::DimGray},
    {downcase("DodgerBlue"),            D2D1::ColorF::DodgerBlue},
    {downcase("Firebrick"),             D2D1::ColorF::Firebrick},
    {downcase("FloralWhite"),           D2D1::ColorF::FloralWhite},
    {downcase("ForestGreen"),           D2D1::ColorF::ForestGreen},
    {downcase("Fuchsia"),               D2D1::ColorF::Fuchsia},
    {downcase("Gainsboro"),             D2D1::ColorF::Gainsboro},
    {downcase("GhostWhite"),            D2D1::ColorF::GhostWhite},
    {downcase("Gold"),                  D2D1::ColorF::Gold},
    {downcase("Goldenrod"),             D2D1::ColorF::Goldenrod},
    {downcase("Gray"),                  D2D1::ColorF::Gray},
    {downcase("Green"),                 D2D1::ColorF::Green},
    {downcase("GreenYellow"),           D2D1::ColorF::GreenYellow},
    {downcase("Honeydew"),              D2D1::ColorF::Honeydew},
    {downcase("HotPink"),               D2D1::ColorF::HotPink},
    {downcase("IndianRed"),             D2D1::ColorF::IndianRed},
    {downcase("Indigo"),                D2D1::ColorF::Indigo},
    {downcase("Ivory"),                 D2D1::ColorF::Ivory},
    {downcase("Khaki"),                 D2D1::ColorF::Khaki},
    {downcase("Lavender"),              D2D1::ColorF::Lavender},
    {downcase("LavenderBlush"),         D2D1::ColorF::LavenderBlush},
    {downcase("LawnGreen"),             D2D1::ColorF::LawnGreen},
    {downcase("LemonChiffon"),          D2D1::ColorF::LemonChiffon},
    {downcase("LightBlue"),             D2D1::ColorF::LightBlue},
    {downcase("LightCoral"),            D2D1::ColorF::LightCoral},
    {downcase("LightCyan"),             D2D1::ColorF::LightCyan},
    {downcase("LightGoldenrodYellow"),  D2D1::ColorF::LightGoldenrodYellow},
    {downcase("LightGreen"),            D2D1::ColorF::LightGreen},
    {downcase("LightGray"),             D2D1::ColorF::LightGray},
    {downcase("LightPink"),             D2D1::ColorF::LightPink},
    {downcase("LightSalmon"),           D2D1::ColorF::LightSalmon},
    {downcase("LightSeaGreen"),         D2D1::ColorF::LightSeaGreen},
    {downcase("LightSkyBlue"),          D2D1::ColorF::LightSkyBlue},
    {downcase("LightSlateGray"),        D2D1::ColorF::LightSlateGray},
    {downcase("LightSteelBlue"),        D2D1::ColorF::LightSteelBlue},
    {downcase("LightYellow"),           D2D1::ColorF::LightYellow},
    {downcase("Lime"),                  D2D1::ColorF::Lime},
    {downcase("LimeGreen"),             D2D1::ColorF::LimeGreen},
    {downcase("Linen"),                 D2D1::ColorF::Linen},
    {downcase("Magenta"),               D2D1::ColorF::Magenta},
    {downcase("Maroon"),                D2D1::ColorF::Maroon},
    {downcase("MediumAquamarine"),      D2D1::ColorF::MediumAquamarine},
    {downcase("MediumBlue"),            D2D1::ColorF::MediumBlue},
    {downcase("MediumOrchid"),          D2D1::ColorF::MediumOrchid},
    {downcase("MediumPurple"),          D2D1::ColorF::MediumPurple},
    {downcase("MediumSeaGreen"),        D2D1::ColorF::MediumSeaGreen},
    {downcase("MediumSlateBlue"),       D2D1::ColorF::MediumSlateBlue},
    {downcase("MediumSpringGreen"),     D2D1::ColorF::MediumSpringGreen},
    {downcase("MediumTurquoise"),       D2D1::ColorF::MediumTurquoise},
    {downcase("MediumVioletRed"),       D2D1::ColorF::MediumVioletRed},
    {downcase("MidnightBlue"),          D2D1::ColorF::MidnightBlue},
    {downcase("MintCream"),             D2D1::ColorF::MintCream},
    {downcase("MistyRose"),             D2D1::ColorF::MistyRose},
    {downcase("Moccasin"),              D2D1::ColorF::Moccasin},
    {downcase("NavajoWhite"),           D2D1::ColorF::NavajoWhite},
    {downcase("Navy"),                  D2D1::ColorF::Navy},
    {downcase("OldLace"),               D2D1::ColorF::OldLace},
    {downcase("Olive"),                 D2D1::ColorF::Olive},
    {downcase("OliveDrab"),             D2D1::ColorF::OliveDrab},
    {downcase("Orange"),                D2D1::ColorF::Orange},
    {downcase("OrangeRed"),             D2D1::ColorF::OrangeRed},
    {downcase("Orchid"),                D2D1::ColorF::Orchid},
    {downcase("PaleGoldenrod"),         D2D1::ColorF::PaleGoldenrod},
    {downcase("PaleGreen"),             D2D1::ColorF::PaleGreen},
    {downcase("PaleTurquoise"),         D2D1::ColorF::PaleTurquoise},
    {downcase("PaleVioletRed"),         D2D1::ColorF::PaleVioletRed},
    {downcase("PapayaWhip"),            D2D1::ColorF::PapayaWhip},
    {downcase("PeachPuff"),             D2D1::ColorF::PeachPuff},
    {downcase("Peru"),                  D2D1::ColorF::Peru},
    {downcase("Pink"),                  D2D1::ColorF::Pink},
    {downcase("Plum"),                  D2D1::ColorF::Plum},
    {downcase("PowderBlue"),            D2D1::ColorF::PowderBlue},
    {downcase("Purple"),                D2D1::ColorF::Purple},
    {downcase("Red"),                   D2D1::ColorF::Red},
    {downcase("RosyBrown"),             D2D1::ColorF::RosyBrown},
    {downcase("RoyalBlue"),             D2D1::ColorF::RoyalBlue},
    {downcase("SaddleBrown"),           D2D1::ColorF::SaddleBrown},
    {downcase("Salmon"),                D2D1::ColorF::Salmon},
    {downcase("SandyBrown"),            D2D1::ColorF::SandyBrown},
    {downcase("SeaGreen"),              D2D1::ColorF::SeaGreen},
    {downcase("SeaShell"),              D2D1::ColorF::SeaShell},
    {downcase("Sienna"),                D2D1::ColorF::Sienna},
    {downcase("Silver"),                D2D1::ColorF::Silver},
    {downcase("SkyBlue"),               D2D1::ColorF::SkyBlue},
    {downcase("SlateBlue"),             D2D1::ColorF::SlateBlue},
    {downcase("SlateGray"),             D2D1::ColorF::SlateGray},
    {downcase("Snow"),                  D2D1::ColorF::Snow},
    {downcase("SpringGreen"),           D2D1::ColorF::SpringGreen},
    {downcase("SteelBlue"),             D2D1::ColorF::SteelBlue},
    {downcase("Tan"),                   D2D1::ColorF::Tan},
    {downcase("Teal"),                  D2D1::ColorF::Teal},
    {downcase("Thistle"),               D2D1::ColorF::Thistle},
    {downcase("Tomato"),                D2D1::ColorF::Tomato},
    {downcase("Turquoise"),             D2D1::ColorF::Turquoise},
    {downcase("Violet"),                D2D1::ColorF::Violet},
    {downcase("Wheat"),                 D2D1::ColorF::Wheat},
    {downcase("White"),                 D2D1::ColorF::White},
    {downcase("WhiteSmoke"),            D2D1::ColorF::WhiteSmoke},
    {downcase("Yellow"),                D2D1::ColorF::Yellow},
    {downcase("YellowGreen"),           D2D1::ColorF::YellowGreen},
  };
  #pragma endregion

  D2D1::ColorF color(D2D1::ColorF::White);

  auto it = namedColors.end();
  if (name.find('#') == 0 && name.size() >= 7)
  {
    color.r = stoul(name.substr(1, 2), nullptr, 16) / 255.0f;
    color.g = stoul(name.substr(3, 2), nullptr, 16) / 255.0f;
    color.b = stoul(name.substr(5, 2), nullptr, 16) / 255.0f;

    if (name.size() >= 9)
      color.a = stoul(name.substr(7, 2), nullptr, 16) / 255.0f;
  }
  else if ((it = namedColors.find(downcase(name))) != namedColors.end())
  {
    color = it->second;
  }
  else
  {
    auto delim = name.find_first_of(',');
    auto colorName = name.substr(0, delim);
    auto alphaStart = name.find_first_not_of(' ', delim + 1);
    auto alphaTxt = name.substr(alphaStart);

    if ((it = namedColors.find(downcase(colorName))) != namedColors.end())
    {
      color = it->second;
      color.a = stof(alphaTxt);
    }
  }

  return color;
}

// ----------------------------------------------------------------------------


