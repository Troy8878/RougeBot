/*********************************
 * Roguelike.h
 * Connor Hilarides
 * Created 2014/05/28
 * Copyright © 2014 DigiPen Institute of Technology, All Rights Reserved
 *********************************/

#pragma once

#include "Engine\Common.h"

#include "Engine\Game.h"
#include "Engine\Shader.h"
#include "Engine\Model.h"
#include "Engine\Camera.h"
#include "Engine\StandardShapes.h"
#include "Engine\Colors.h"
#include "Engine\RenderSet.h"

#include "GameConsole.h"

#include "mruby/data.h"
#include "mruby/value.h"
#include "mruby/class.h"

#include "FMOD/SoundManager.h"
#include "Engine/Input.h"

// ----------------------------------------------------------------------------

class Roguelike : public Game
{
public:
  Roguelike(const std::string& title, HINSTANCE hInstance)
    : Game(title, hInstance)
  {
    initSettings.cullTriangles = false;
    initSettings.assetPack = L"./Assets.respack";
    initSettings.assetFolder = L"./Assets";
    initSettings.vsync = true;
  }

  ManagedSound sound;

  void OnInit() override
  {
    using namespace DirectX;

    // Starting level
    levelChangeContext.name = "Splash";

    _graphicsDevice->backgroundColor = XMVectorSet(0, 0, 0, 1);
    _console = new GameConsole(true);

    RenderGroup::Instance.Initialize();

    InitShaders();
    InitWMHandlers();

    using namespace Events;
    static EventId updateEvent("update");
    SetHandler(updateEvent, &Roguelike::OnUpdate);

    static EventId resizeEvent("window_resize");

    Event::GlobalDispatcher->AddListener(_console);
  }

  void InitShaders()
  {
    Shader::LoadShader(_graphicsDevice.get(), "Basic.shaderdef");
    Shader::LoadShader(_graphicsDevice.get(), "Textured.shaderdef");
  }

  void InitWMHandlers()
  {
    // Keep the window no less wide than a square
    SetProcHandler(WM_GETMINMAXINFO, [this](HWND, UINT, WPARAM, LPARAM lp, LRESULT&)
    {
      MINMAXINFO& info = *reinterpret_cast<LPMINMAXINFO>(lp);
      info.ptMinTrackSize.y = 480;
      info.ptMinTrackSize.x = 480;
    });
    SetProcHandler(WM_SIZING, [this](HWND, UINT, WPARAM wp, LPARAM lp, LRESULT& res)
    {
      RECT& rect = *reinterpret_cast<LPRECT>(lp);
      
      auto minwidth = LONG((rect.bottom - rect.top) * (4.0 / 3.0));
      if (rect.right - rect.left < minwidth)
      {
        // Squarify while resizing from right
        if (wp == WMSZ_RIGHT || wp == WMSZ_TOPRIGHT || wp == WMSZ_BOTTOMRIGHT ||
            wp == WMSZ_TOP || wp == WMSZ_BOTTOM)
        {
          rect.right = rect.left + minwidth;
          res = TRUE;
        }
        // Squarify while resizing from left
        else if (wp == WMSZ_LEFT || wp == WMSZ_TOPLEFT || wp == WMSZ_BOTTOMLEFT)
        {
          rect.left = rect.right - minwidth;
          res = TRUE;
        }
      }
    });
  }

  void OnUpdate(Events::EventMessage& e)
  {
    using namespace Events;
    auto& time = e.GetData<UpdateEvent>()->gameTime;
    float dt = (float) time.Dt;



    UpdateTitleFPS(dt);
  }

  void UpdateTitleFPS(float dt)
  {
    if (dt > 0.001)
    {
      const int update_res = 60;

      static float prev_fps = 60;
      static float fps = 60;
      fps = (fps * (update_res - 1) + 1 / dt) / update_res;

      if (fps != prev_fps)
      {
        prev_fps = fps;

        auto title = _title + " [" + std::to_string((int) (fps + 0.5f)) + " fps]";
        SetWindowText(_graphicsDevice->Window, title.c_str());
      }
    }
  }

  void OnFree() override
  {
    delete _console;
  }

private:
  GameConsole *_console = nullptr;
};

// ----------------------------------------------------------------------------
