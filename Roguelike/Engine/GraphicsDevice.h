/*********************************
 * GraphicsDevice.h
 * Connor Hilarides
 * Created 2014/06/24
 * Copyright � 2014 DigiPen Institute of Technology, All Rights Reserved
 *********************************/

#pragma once

#include "Common.h"

// ----------------------------------------------------------------------------

struct WindowCreationOptions;
class GraphicsDevice;
class WindowDevice;
class Texture2D;

// ----------------------------------------------------------------------------

__interface WndProcPatch
{
  LRESULT PatchedWndProc(HWND, UINT msg, WPARAM wp, LPARAM lp, bool &cont);
  void Update(const GameTime &time, bool &cont);
};

// ----------------------------------------------------------------------------

class GraphicsDevice abstract
{
public:
  typedef std::chrono::high_resolution_clock::time_point resource_token_t;

  GraphicsDevice();
  virtual ~GraphicsDevice();

  virtual HWND GetContextWindow() = 0;

  virtual void SetSize(math::Vector2D newSize, bool overrideFullscreen = false) = 0;
  virtual math::Vector2D GetSize() const = 0;

  virtual bool BeginFrame() = 0;
  virtual void EndFrame() = 0;
  virtual void ProcessMessages() = 0;
  bool IsPatched();

  void PatchWndProc(WndProcPatch &patch);

  static std::unique_ptr<WindowDevice> CreateGameWindow(const WindowCreationOptions &options);
  void CreateInputLayout(byte *bytecode,
                         UINT bytecodeSize,
                         D3D11_INPUT_ELEMENT_DESC *layoutDesc,
                         UINT layoutDescNumElements,
                         ID3D11InputLayout **layout);

  // 3D stuff
  IR_PROPERTY(IDXGISwapChain1 *, SwapChain);
  IR_PROPERTY(IDXGIAdapter *, FactoryAdapter);
  IR_PROPERTY(IDXGIDevice *, FactoryDevice);
  IR_PROPERTY(IDXGIFactory2 *, DeviceFactory);
  IR_PROPERTY(ID3D11Device *, Device);
  IR_PROPERTY(ID3D11DeviceContext *, DeviceContext);
  IR_PROPERTY(ID3D11RenderTargetView *, RenderTargetView);
  IR_PROPERTY(ID3D11Texture2D *, DepthStencilBuffer);
  IR_PROPERTY(ID3D11DepthStencilState *, DepthStencilState);
  IR_PROPERTY(ID3D11DepthStencilView *, DepthStencilView);
  IR_PROPERTY(ID3D11RasterizerState *, RasterState);
  IR_PROPERTY(ID3D11RasterizerState *, WireframeState);
  IR_PROPERTY(ID3D11BlendState *, BlendState);

  bool WireframeDraw = false;
  bool DebugDraw = false;

  // 2D stuff
  struct D2DData
  {
    typedef std::chrono::high_resolution_clock clock;

    IR_PROPERTY(clock::time_point, ResourceTimestamp);

    IR_PROPERTY(ID2D1Factory1 *, Factory);
    IR_PROPERTY(ID2D1Device *, Device);
    IR_PROPERTY(ID2D1DeviceContext *, DeviceContext);
    IR_PROPERTY(IDXGISurface *, BackBuffer);
    IR_PROPERTY(ID2D1Bitmap1 *, TargetBitmap);
    IR_PROPERTY(IDWriteFactory *, WriteFactory);

    void DrawTo(Texture2D texture);
    HRESULT EndDraw();

    void Invalidate();

  private:
    critical_section *drawLock;
    friend class GraphicsDevice;
  };

  IR_PROPERTY(D2DData, D2D);

protected:
  void InitializeD3DContext();
  void InitializeDepthBuffer();
  void InitializeD2DContext();
  void FreeDXContext();
  void FreeD2DResources();

public:
  math::Vector backgroundColor;
  critical_section drawLock;
};

// ----------------------------------------------------------------------------

struct WindowCreationOptions
{
  HINSTANCE hInstance;
  math::Vector2D size;
  std::string gameTitle;
};

// ----------------------------------------------------------------------------

class WindowDevice final : public GraphicsDevice
{
  math::Vector2D _size;

public:
  HWND GetContextWindow() override
  {
    return Window;
  }

  void SetSize(math::Vector2D newSize, bool overrideFullscreen = false) final override;
  math::Vector2D GetSize() const final override;

  bool BeginFrame() override;
  void EndFrame() override;

  void ProcessMessages() override;

  IR_PROPERTY(HWND, Window);

  bool GetFullscreen();
  void SetFullscreen(bool value);

private:
  explicit WindowDevice(const WindowCreationOptions &options);
  static LRESULT CALLBACK StaticWindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

  HWND InitializeWindow(const WindowCreationOptions &options);
  LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

  RECT pre_fullscreen_rect;
  bool is_fullscreen = false;

  friend class GraphicsDevice;
};

// ----------------------------------------------------------------------------

struct DisplayMode : public DXGI_MODE_DESC
{
  DisplayMode() = default;
  explicit DisplayMode(const DXGI_MODE_DESC &mode);

  DisplayMode &operator=(const DXGI_MODE_DESC &other)
  {
    DXGI_MODE_DESC::operator=(other);
    return *this;
  }
};

// ----------------------------------------------------------------------------

struct DisplaySetting;

struct DisplayOutput : public DXGI_OUTPUT_DESC
{
  explicit DisplayOutput(IDXGIOutput *output);

  std::vector<DisplayMode> DisplayModes;

  void CreateResolutionList(std::vector<DisplaySetting> &settings);
};

// ----------------------------------------------------------------------------

struct DisplayAdapter
{
  static void GetAdapters(std::vector<DisplayAdapter> &adapters);

  explicit DisplayAdapter(IDXGIAdapter *dxgAdapter);

  std::vector<DisplayOutput> DisplayOutputs;
};

// ----------------------------------------------------------------------------

struct DisplaySetting
{
  explicit DisplaySetting(const DXGI_MODE_DESC &mode)
    : Width(mode.Width), Height(mode.Height), Variants({DisplayMode(mode)})
  {
  }

  UINT Width, Height;
  std::vector<DisplayMode> Variants;
};

// ----------------------------------------------------------------------------