/*********************************
 * CODA.h
 * Connor Hilarides
 * Created 2014/11/21
 * Copyright © 2014 DigiPen Institute of Technology, All Rights Reserved
 *********************************/

#include "Common.h"
#include "CODA.h"
#include "Shader.h"

#include "SpriteComponent.h"

// ----------------------------------------------------------------------------

ConfirmationOfDestructiveAction::
  ConfirmationOfDestructiveAction()
{
  SpriteShader = RegisteredShaders["Textured"];
  SpriteModel = SpriteComponent::GetSpriteModel();

  camera.position.z = -3;
  camera.Init();
  camera.Update();

  using namespace DirectX;

  BackgroundPos =
    XMMatrixScaling(10, 5, 1);
  MessagePos =
    XMMatrixScaling(6, 3, 1);
  AffirmativePos =
    XMMatrixTranslation(-4, -2, 0) *
    XMMatrixScaling(3, 2, 1);
  NegatoryPos =
    XMMatrixTranslation(4, -2, 0) *
    XMMatrixScaling(3, 2, 1);
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

  GetGame()->GameDevice->PatchWndProc(*this);

  return false;
}

// ----------------------------------------------------------------------------

LRESULT ConfirmationOfDestructiveAction::
  PatchedWndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp, bool &cont)
{
  cont = true;

  switch (msg)
  {
    case WM_SIZING:
      return FixSizing(hwnd, msg, wp, lp);
  }

  return DefWindowProc(hwnd, msg, wp, lp);
}

// ----------------------------------------------------------------------------

void ConfirmationOfDestructiveAction::
  Update(const GameTime &)
{
  SpriteShader->camera = &camera;
  SpriteModel->shader = SpriteShader;

  SpriteModel->texture = Message;
  SpriteModel->Draw(MessagePos);

  SpriteModel->texture = Affirmative;
  SpriteModel->Draw(AffirmativePos);

  SpriteModel->texture = Negatory;
  SpriteModel->Draw(NegatoryPos);
}

// ----------------------------------------------------------------------------

void ConfirmationOfDestructiveAction::
  ValidateFont()
{
  auto &d2d = GetGame()->GameDevice->D2D;

  if (!format)
  {
    d2d.WriteFactory->CreateTextFormat(
      L"Segoe UI", nullptr,
      DWRITE_FONT_WEIGHT_BOLD,
      DWRITE_FONT_STYLE_NORMAL,
      DWRITE_FONT_STRETCH_NORMAL,
      72, L"", &format);
    format->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
    format->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
  }

  if (!Message)
    Message = TextureManager::Instance.LoadTexture("SPECIAL/SURFACE/600:300");
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
  FixSizing(HWND, UINT, WPARAM, LPARAM lp)
{
  *reinterpret_cast<LPRECT>(lp) = windowSize;
  return TRUE;
}

// ----------------------------------------------------------------------------
