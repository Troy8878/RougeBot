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

  using namespace DirectX;

  SpinnerPos = XMMatrixScaling(0.15, 0.15, 0.15);
  SpinnerRot = XMMatrixIdentity();
}

// ----------------------------------------------------------------------------

AsyncLoadingScreen::~AsyncLoadingScreen()
{
}

// ----------------------------------------------------------------------------

void AsyncLoadingScreen::Run()
{
  GetWindowRect(GetGame()->GameDevice->GetContextWindow(), &windowSize);

  WINDOWINFO info;
  GetWindowInfo(GetGame()->GameDevice->GetContextWindow(), &info);

  //info.dwStyle &= ~WS_MAXIMIZEBOX;
  //SetWindowLong(GetGame()->GameDevice->GetContextWindow(), GWL_STYLE, info.dwStyle);

  GetGame()->GameDevice->PatchWndProc(*this);

  //info.dwStyle |= WS_MAXIMIZEBOX;
  //SetWindowLong(GetGame()->GameDevice->GetContextWindow(), GWL_STYLE, info.dwStyle);
}

// ----------------------------------------------------------------------------

LRESULT AsyncLoadingScreen::PatchedWndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp, bool &cont)
{
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
    cont = false;
    return 0;
  }

  return DefWindowProc(hwnd, msg, wp, lp);
}

// ----------------------------------------------------------------------------

void AsyncLoadingScreen::Update(const GameTime &time)
{
  using namespace DirectX;

  this->time += time.Dt;

  // Rotate the spinner
  SpinnerRot = SpinnerRot * XMMatrixRotationZ(static_cast<float>(time.Dt * 2 * math::pi));

  // Clear the depth buffer
  GetGame()->GameDevice->DeviceContext->ClearDepthStencilView(
             GetGame()->GameDevice->DepthStencilView,
             D3D10_CLEAR_DEPTH | D3D10_CLEAR_STENCIL,
             1.0f, 0
           );

  SpriteModel->shader->camera->viewMatrix = XMMatrixLookAtLH(
    math::Vector{0, 0, 1, 1},
    math::Vector{0, 0, -1, 0},
    math::Vector{0, 1, 0, 0});

  SpriteModel->tintTexture = Texture2D();
  SpriteModel->tint = math::Vector{1, 1, 1, 1};
  SpriteModel->texture = Spinner;
  SpriteModel->Draw(SpinnerPos * SpinnerRot);

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

