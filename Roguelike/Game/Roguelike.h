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
  Roguelike(const std::string &title, HINSTANCE hInstance)
    : Game(title, hInstance)
  {
    initSettings.cullTriangles = false;
    initSettings.assetPack = L"./Assets.respack";
    initSettings.assetFolder = L"./Assets";
    initSettings.vsync = false;
  }

  ManagedSound sound;

  void OnInit() override
  {
    using namespace DirectX;

    // Starting level
    levelChangeContext.name = "Init";

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
    SetProcHandlers(
      [this](HWND, UINT, WPARAM, LPARAM lp, LRESULT &)
      {
        MINMAXINFO &info = *reinterpret_cast<LPMINMAXINFO>(lp);
        info.ptMinTrackSize.y = 480;
        info.ptMinTrackSize.x = 480;
      }, WM_GETMINMAXINFO
    );

    SetProcHandlers(
      [this](HWND, UINT, WPARAM wp, LPARAM lp, LRESULT &res)
      {
        RECT &rect = *reinterpret_cast<LPRECT>(lp);

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
      }, WM_SIZING
    );
  }

  void OnUpdate(Events::EventMessage &e)
  {
    using namespace Events;
    auto &time = e.GetData<UpdateEvent>()->gameTime;
    float dt = static_cast<float>(time.Dt);

    UpdateTitleFPS(dt);
  }

  void UpdateTitleFPS(float dt);

  void OnFree() override
  {
    delete _console;
  }

private:
  GameConsole *_console = nullptr;

public:
  GameConsole *ConsoleInst()
  {
    return _console;
  }
};

// ----------------------------------------------------------------------------

struct fps_item
{
  float fps = 0;
  float wndproc_part = 0;
  float unlock_part = 0;
  float load_part = 0;
  float update_part = 0;
  float draw_part = 0;
  float gc_wait_part = 0;
  float kill_part = 0;

  void operator+=(const fps_item &rhs)
  {
    fps += rhs.fps;
    wndproc_part += rhs.wndproc_part;
    unlock_part += rhs.unlock_part;
    load_part += rhs.load_part;
    update_part += rhs.update_part;
    draw_part += rhs.draw_part;
    gc_wait_part += rhs.gc_wait_part;
    kill_part += rhs.kill_part;
  }

  void operator/=(int rhs)
  {
    fps /= rhs;
    wndproc_part /= rhs;
    unlock_part /= rhs;
    load_part /= rhs;
    update_part /= rhs;
    draw_part /= rhs;
    gc_wait_part /= rhs;
    kill_part /= rhs;
  }

  void pull_performance(Roguelike *game, float dt)
  {
    fps = 1 / dt;
    wndproc_part = game->performance.wndproc.time_dt() / dt;
    unlock_part = game->performance.unlock_game.time_dt() / dt;
    load_part = game->performance.load_level.time_dt() / dt;
    update_part = game->performance.update.time_dt() / dt;
    draw_part = game->performance.draw.time_dt() / dt;
    gc_wait_part = game->performance.gc_join.time_dt() / dt;
    kill_part = game->performance.entity_kill.time_dt() / dt;
  }

  void print(std::ostream &out)
  {
    out << static_cast<int>(fps + 0.5f) << " fps";

    // Detailed performance
    out << ", ";
    out << static_cast<int>(wndproc_part * 100) << "% wndproc, ";
    out << static_cast<int>(unlock_part * 100) << "% unlock, ";
    out << static_cast<int>(load_part * 100) << "% loading, ";
    out << static_cast<int>(update_part * 100) << "% update, ";
    out << static_cast<int>(draw_part * 100) << "% draw, ";
    out << static_cast<int>(gc_wait_part * 100) << "% gc wait, ";
    out << static_cast<int>(kill_part * 100) << "% kill";
    out << ")";
  }
};

inline void Roguelike::UpdateTitleFPS(float dt)
{
  if (dt > 0.0001)
  {
    const int update_res = 20;
    static fps_item fps_buffer[update_res];
    static int buf_count = 0;

    fps_buffer[buf_count++].pull_performance(this, dt);

    if (buf_count == update_res)
    {
      buf_count = 0;

      fps_item avg;
      for (auto &val : fps_buffer)
      {
        avg += val;
      }
      avg /= update_res;

      std::ostringstream title;
      title << _title << " [";
      avg.print(title);
      title << "]";
      SetWindowText(this->_graphicsDevice->Window, title.str().c_str());
    }
  }
}

// ----------------------------------------------------------------------------