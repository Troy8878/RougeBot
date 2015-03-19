/*********************************
 * CODA.h
 * Connor Hilarides
 * Created 2014/11/21
 * Copyright © 2014 DigiPen Institute of Technology, All Rights Reserved
 *********************************/

#pragma once

#include "Common.h"
#include "Camera.h"

// ----------------------------------------------------------------------------

struct Shader;
class Model;

// ----------------------------------------------------------------------------

class AsyncLoadingScreen final : public WndProcPatch
{
public:
  AsyncLoadingScreen();
  ~AsyncLoadingScreen();

  void Run();

  LRESULT PatchedWndProc(HWND, UINT msg, WPARAM wp, LPARAM lp, bool &cont) override;
  void Update(const GameTime& time) override;

private:

  LRESULT FixSizing(HWND, UINT, WPARAM, LPARAM);

  Shader *SpriteShader;
  Model *SpriteModel;

  // Text drawing
  IDWriteTextFormat *format = nullptr;

  // Text brush
  ID2D1SolidColorBrush *textBrush = nullptr;

  // Textures
  Texture2D Spinner;

  math::Matrix SpinnerPos;
  math::Matrix SpinnerRot;

  // Don't change anything, we're locking down in this mode
  RECT windowSize;

  int highlight = 0;
  std::atomic<bool> cont = true;
  double time = 0;
};

// ----------------------------------------------------------------------------
