/*********************************
 * CODA.h
 * Connor Hilarides
 * Created 2014/11/21
 * Copyright © 2014 DigiPen Institute of Technology, All Rights Reserved
 *********************************/

#include "Common.h"
#include "CODA.h"
#include "Shader.h"
#include "Level.h"

#include "SpriteComponent.h"
#include "StandardShapes.h"

// ----------------------------------------------------------------------------

ConfirmationOfDestructiveAction::
  ConfirmationOfDestructiveAction()
{
  SpriteShader = RegisteredShaders["Textured"];
  SpriteModel = SpriteComponent::GetSpriteModel();

  using namespace DirectX;

  BackgroundPos =
    XMMatrixScaling(10, 5, 1);
  MessagePos =
    XMMatrixScaling(1.25f, 0.625f, 1);
  AffirmativePos =
    XMMatrixScaling(1, 2/3.0f, 1) *
    XMMatrixTranslation(-1, -0.5, 0);
  NegatoryPos =
    XMMatrixScaling(1, 2/3.0f, 1) *
    XMMatrixTranslation(1, -0.5, 0);
}

// ----------------------------------------------------------------------------

ConfirmationOfDestructiveAction::
  ~ConfirmationOfDestructiveAction()
{
}

// ----------------------------------------------------------------------------

bool ConfirmationOfDestructiveAction::
  Confirm(const std::string& message, const std::string& affirmative, const std::string& negatory)
{
  GetWindowRect(GetGame()->GameDevice->GetContextWindow(), &windowSize);
  ValidateFont();
  DrawMessages(message, affirmative, negatory);

  WINDOWINFO info;
  GetWindowInfo(GetGame()->GameDevice->GetContextWindow(), &info);

  info.dwStyle &= ~WS_MAXIMIZEBOX;
  SetWindowLong(GetGame()->GameDevice->GetContextWindow(), GWL_STYLE, info.dwStyle);

  GetGame()->GameDevice->PatchWndProc(*this);

  info.dwStyle |= WS_MAXIMIZEBOX;
  SetWindowLong(GetGame()->GameDevice->GetContextWindow(), GWL_STYLE, info.dwStyle);

  return answer;
}

// ----------------------------------------------------------------------------

LRESULT ConfirmationOfDestructiveAction::
  PatchedWndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp, bool &cont)
{
  cont = true;

  switch (msg)
  {
    case WM_SIZING:
    case WM_MOVE:
      return FixSizing(hwnd, msg, wp, lp);
    case WM_CLOSE:
      _exit(0);
    case WM_PAINT:
    {
      PAINTSTRUCT ps;
      BeginPaint(hwnd, &ps);
      EndPaint(hwnd, &ps);
      return 0;
    }
    case WM_LBUTTONDOWN:
    {
      auto mx = LOWORD(lp);
      auto my = HIWORD(lp);

      auto ww = windowSize.right - windowSize.left;
      auto wh = windowSize.bottom - windowSize.top;

      if (my > wh / 2)
      {
        answer = mx > ww / 2;
        cont = false;
      }
    }
  }

  return DefWindowProc(hwnd, msg, wp, lp);
}

// ----------------------------------------------------------------------------

void ConfirmationOfDestructiveAction::
  Update(const GameTime &)
{
  using namespace DirectX;

  // Draw the game
  RenderGroup::Instance.Draw(*static_cast<Events::EventMessage *>(nullptr));

  auto hudRoot = GetGame()->CurrentLevel->RootEntity->FindEntity("CameraRoot");
  auto hudTransform = XMMatrixTranslation(0, 0, 2.4f) * hudRoot->Transform;

  SpriteModel->tintTexture = Texture2D();

  // Draw background tinting
  SpriteModel->texture = Texture2D();
  SpriteModel->tint = math::Vector(0, 0, 0, 0.8f);
  SpriteModel->Draw(BackgroundPos * hudTransform);

  // Draw all of the text
  SpriteModel->tint = math::Vector(1, 1, 1, 1);

  SpriteModel->texture = Message;
  SpriteModel->Draw(MessagePos * hudTransform);

  SpriteModel->texture = Affirmative;
  SpriteModel->Draw(AffirmativePos * hudTransform);

  SpriteModel->texture = Negatory;
  SpriteModel->Draw(NegatoryPos * hudTransform);
}

// ----------------------------------------------------------------------------

void ConfirmationOfDestructiveAction::
  ValidateFont()
{
  auto &d2d = GetGame()->GameDevice->D2D;

  if (!format)
  {
    d2d.WriteFactory->CreateTextFormat(
      L"Segoe UI Symbol", nullptr,
      DWRITE_FONT_WEIGHT_BOLD,
      DWRITE_FONT_STYLE_NORMAL,
      DWRITE_FONT_STRETCH_NORMAL,
      72, L"", &format);
    format->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
    format->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
  }

  if (!Message)
    Message = TextureManager::Instance.LoadTexture("SPECIAL/SURFACE/800:400");
  if (!Affirmative)
    Affirmative = TextureManager::Instance.LoadTexture("SPECIAL/SURFACE/300:200");
  if (!Negatory)
    Negatory = TextureManager::Instance.LoadTexture("SPECIAL/SURFACE/300:200");

  ReleaseDXInterface(textBrush);
  d2d.DeviceContext->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), &textBrush);
}

void ConfirmationOfDestructiveAction::
  DrawMessages(const std::string& message, const std::string& affirmative, const std::string& negatory)
{
  auto &d2d = GetGame()->GameDevice->D2D;

  auto wmessage = widen(message);
  auto waffirm = widen(affirmative);
  auto wnegatory = widen(negatory);

  D2D1_SIZE_F size;
  D2D1_RECT_F rect;

  d2d.DrawTo(Message);
  size = d2d.DeviceContext->GetSize();
  rect = D2D1::RectF(0, 0, size.width, size.height);
  d2d.DeviceContext->Clear(D2D1::ColorF(0, 0.0f));
  d2d.DeviceContext->DrawText(
    wmessage.c_str(), static_cast<UINT>(wmessage.size()),
    format, rect, textBrush);
  d2d.EndDraw();

  d2d.DrawTo(Affirmative);
  size = d2d.DeviceContext->GetSize();
  rect = D2D1::RectF(0, 0, size.width, size.height);
  d2d.DeviceContext->Clear(D2D1::ColorF(0, 0.0f));
  d2d.DeviceContext->DrawText(
    waffirm.c_str(), static_cast<UINT>(waffirm.size()),
    format, rect, textBrush);
  d2d.EndDraw();

  d2d.DrawTo(Negatory);
  size = d2d.DeviceContext->GetSize();
  rect = D2D1::RectF(0, 0, size.width, size.height);
  d2d.DeviceContext->Clear(D2D1::ColorF(0, 0.0f));
  d2d.DeviceContext->DrawText(
    wnegatory.c_str(), static_cast<UINT>(wnegatory.size()),
    format, rect, textBrush);
  d2d.EndDraw();
}

// ----------------------------------------------------------------------------

LRESULT ConfirmationOfDestructiveAction::
  FixSizing(HWND, UINT msg, WPARAM, LPARAM lp)
{
  if (msg == WM_SIZING)
  {
    *reinterpret_cast<LPRECT>(lp) = windowSize;
    return TRUE;
  }
  if (msg == WM_MOVE)
  {
    GetWindowRect(GetGame()->GameDevice->GetContextWindow(), &windowSize);
    return 0;
  }

  return 0;
}

// ----------------------------------------------------------------------------
