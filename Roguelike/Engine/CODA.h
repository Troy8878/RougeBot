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

class ConfirmationOfDestructiveAction final : public WndProcPatch
{
public:
  ConfirmationOfDestructiveAction();
  ~ConfirmationOfDestructiveAction();

  bool Confirm(const std::string &message,
               const std::string &affirmative = "\xEE\x84\x8A",
               const std::string &negatory = "\xEE\x84\x8B");

  LRESULT PatchedWndProc(HWND, UINT msg, WPARAM wp, LPARAM lp, bool &cont) override;
  void Update(const GameTime& time, bool &) override;

private:
  void ValidateFont();
  void DrawMessages(const std::string &message,
                    const std::string &affirmative,
                    const std::string &negatory);

  LRESULT FixSizing(HWND, UINT, WPARAM, LPARAM);

  Shader *SpriteShader;
  Model *SpriteModel;

  // Text drawing
  IDWriteTextFormat *format = nullptr;

  // Text brush
  ID2D1SolidColorBrush *textBrush = nullptr;

  // Textures
  Texture2D Background;
  Texture2D MessageBG;
  Texture2D ButtonBG;
  Texture2D Message;
  Texture2D Affirmative;
  Texture2D Negatory;

  math::Matrix BackgroundPos;
  math::Matrix MessagePos;
  math::Matrix AffirmativePos;
  math::Matrix NegatoryPos;

  // Don't change anything, we're locking down in this mode
  RECT windowSize;

  int highlight = 0;
  bool answer;
};

// ----------------------------------------------------------------------------
