/*********************************
 * CODA.h
 * Connor Hilarides
 * Created 2014/11/21
 * Copyright © 2014 DigiPen Institute of Technology, All Rights Reserved
 *********************************/

#include "Common.h"
#include "Loading.h"
#include "Shader.h"
#include "Level.h"

#include "SpriteComponent.h"

// ----------------------------------------------------------------------------

AsyncLoadingScreen::AsyncLoadingScreen()
{
  SpriteShader = RegisteredShaders["Textured"];
  SpriteModel = SpriteComponent::GetSpriteModel();
  Spinner = TextureManager::Instance.LoadTexture("Spinner.png");
  Background = TextureManager::Instance.LoadTexture("MainMenu/OtherMenu.png");

  using namespace DirectX;

  SpinnerPos =
    XMMatrixScaling(0.15, 0.15, 0.15) *
    XMMatrixTranslation(0, 0, 1.0f);
  SpinnerRot = 0;

  BackgroundPos =
    XMMatrixScaling(6.31111111111111f * 0.3f, 3.55 * 0.3f, 1 * 0.3f) *
    XMMatrixTranslation(0, 0, 1.3f);
}

// ----------------------------------------------------------------------------

AsyncLoadingScreen::~AsyncLoadingScreen()
{
}

// ----------------------------------------------------------------------------

void AsyncLoadingScreen::Run()
{
  if (!cont)
    return;

  GetGame()->GameDevice->PatchWndProc(*this);
}

void AsyncLoadingScreen::Init()
{
  using namespace DirectX;
  Camera = *SpriteModel->shader->camera;
  auto mx = Camera.viewMatrix.get();
  mx.r[3] = XMVectorSet(0, 0, 0, 1);
  Camera.viewMatrix = mx;

  cont = true;
  time = 0;
}

void AsyncLoadingScreen::Stop()
{
  cont = false;
}

// ----------------------------------------------------------------------------

LRESULT AsyncLoadingScreen::PatchedWndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp, bool &cont)
{
  cont = this->cont;
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
  case WM_NOTIFY:
    return 0;
  }

  return DefWindowProc(hwnd, msg, wp, lp);
}

// ----------------------------------------------------------------------------

void AsyncLoadingScreen::Update(const GameTime &time, bool &cont)
{
  using namespace DirectX;

  this->time = this->time + time.Dt;
  cont = this->cont || this->time < 0.25;

  // Rotate the spinner
  SpinnerRot += static_cast<float>(time.Dt * 2 * math::pi);

  SpriteModel->shader->camera = &Camera;

  SpriteModel->tintTexture = Texture2D();
  SpriteModel->tint = math::Vector{1, 1, 1, 1};

  SpriteModel->texture = Background;
  SpriteModel->Draw(BackgroundPos);

  SpriteModel->texture = Spinner;
  SpriteModel->Draw(SpinnerPos * XMMatrixRotationZ(SpinnerRot));

  Sleep(4);
}

// ----------------------------------------------------------------------------

LRESULT AsyncLoadingScreen::FixSizing(HWND, UINT msg, WPARAM, LPARAM lp)
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

