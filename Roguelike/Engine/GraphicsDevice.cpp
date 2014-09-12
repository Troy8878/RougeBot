/*********************************
 * GraphicsDevice.cpp
 * Connor Hilarides
 * Created 2014/06/24
 *********************************/

#include "Common.h"
#include "GraphicsDevice.h"
#include "Helpers\Exceptions.h"
#include "Engine\Game.h"
#include "Input.h"

// Testing stuff, remove later
#include "Level.h"
#include "SpriteComponent.h"

// ----------------------------------------------------------------------------

GraphicsDevice::GraphicsDevice()
  : _DepthStencilBuffer(nullptr),
    _DepthStencilState(nullptr),
    _DepthStencilView(nullptr)
{
}

// ----------------------------------------------------------------------------

GraphicsDevice::~GraphicsDevice()
{
  FreeDXContext();
}

// ----------------------------------------------------------------------------

void GraphicsDevice::FreeDXContext()
{
  FreeD2DResources();

  ReleaseDXInterface(BlendState);
  ReleaseDXInterface(RasterState);
  ReleaseDXInterface(DepthStencilView);
  ReleaseDXInterface(DepthStencilState);
  ReleaseDXInterface(DepthStencilBuffer);
  ReleaseDXInterface(RenderTargetView);
  ReleaseDXInterface(DeviceContext);
  ReleaseDXInterface(Device);
  ReleaseDXInterface(SwapChain);
}

void GraphicsDevice::FreeD2DResources()
{
  ReleaseDXInterface(D2D.TargetBitmap);
  ReleaseDXInterface(D2D.BackBuffer);
  ReleaseDXInterface(D2D.DeviceContext);
  ReleaseDXInterface(D2D.Device);
  ReleaseDXInterface(D2D.Factory);
}

// ----------------------------------------------------------------------------

std::unique_ptr<WindowDevice> GraphicsDevice::CreateGameWindow(const WindowCreationOptions& options)
{
  auto *window = new WindowDevice(options);
  return std::unique_ptr < WindowDevice > {window};
}

// ----------------------------------------------------------------------------

HWND WindowDevice::InitializeWindow(const WindowCreationOptions& options)
{
  std::string className = std::to_string(reinterpret_cast<size_t>(this));

  WNDCLASSEX wndc = {sizeof(wndc)};
  wndc.cbWndExtra = sizeof(WindowDevice *);
  wndc.hInstance = options.hInstance;
  wndc.lpfnWndProc = StaticWindowProc;
  wndc.lpszClassName = className.c_str();
  wndc.hbrBackground = GetSysColorBrush(COLOR_BACKGROUND);
  RegisterClassEx(&wndc);

  HWND window = CreateWindow(wndc.lpszClassName,
                             options.gameTitle.c_str(),
                             WS_OVERLAPPEDWINDOW,
                             CW_USEDEFAULT, CW_USEDEFAULT,
                             static_cast<UINT>(options.size.x),
                             static_cast<UINT>(options.size.y),
                             NULL, NULL, options.hInstance,
                             reinterpret_cast<void *>(this));

  ShowWindow(window, SW_SHOWNORMAL);
  UpdateWindow(window);

  return window;
}

// ----------------------------------------------------------------------------

WindowDevice::WindowDevice(const WindowCreationOptions& options)
  : _size(options.size)
{
  Window = InitializeWindow(options);

  InitializeD3DContext();
  InitializeD2DContext();
}

// ----------------------------------------------------------------------------

LRESULT CALLBACK WindowDevice::StaticWindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
  if (msg == WM_CREATE)
  {
    SetWindowLongPtr(hwnd, GWLP_USERDATA, lparam);
    return 0;
  }

  auto *_this = reinterpret_cast<WindowDevice *>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
  return _this->WindowProc(hwnd, msg, wparam, lparam);
}

// ----------------------------------------------------------------------------

LRESULT WindowDevice::WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
  auto& game = *GetGame();
  auto iter = game._wndprocCallbacks.find(msg);
  if (iter != game._wndprocCallbacks.end())
  {
    auto& handler = iter->second;

    LRESULT result = 0;
    handler(hwnd, msg, wparam, lparam, result);
    return result;
  }

  switch (msg)
  {
    case WM_PAINT:
    {
      PAINTSTRUCT ps;
      BeginPaint(hwnd, &ps);
      EndPaint(hwnd, &ps);
      return 0;
    }

    case WM_CLOSE:
    {
      GetGame()->Stop();
      return 0;
    }

    case WM_KEYDOWN:
    {
      if ((wparam & 0x40000000) == 0)
      {
        auto signal = Input::TranslateSignal(msg, wparam, lparam);
        Input::Instance.OnKeyDown(signal);
        return 0;
      }
    }

    case WM_KEYUP:
    {
      auto signal = Input::TranslateSignal(msg, wparam, lparam);
      Input::Instance.OnKeyUp(signal);
      return 0;
    }
  }

  return DefWindowProc(hwnd, msg, wparam, lparam);
}

// ----------------------------------------------------------------------------

void WindowDevice::ProcessMessages()
{
  MSG message;
  while (PeekMessage(&message, Window, 0, 0, TRUE))
  {
    TranslateMessage(&message);
    DispatchMessage(&message);
  }
}

// ----------------------------------------------------------------------------

void WindowDevice::SetSize(math::Vector2D size, bool overrideFullscreen)
{
  if (size.x < 400 || size.y < 400)
    return;

  if (!SwapChain)
    return;

  if (!overrideFullscreen)
  {
    BOOL fullscreen;
    SwapChain->GetFullscreenState(&fullscreen, nullptr);
    if (fullscreen)
      return;
  }

  _size = size;

  // Release D2D stuff
  FreeD2DResources();

  // Release render target
  DeviceContext->OMSetRenderTargets(0, nullptr, nullptr);
  ReleaseDXInterface(RenderTargetView);

  HRESULT hr;
  hr = SwapChain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);
  CHECK_HRESULT(hr);

  ID3D11Texture2D *pBuffer;
  hr = SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D),
                            reinterpret_cast<void **>(&pBuffer));
  CHECK_HRESULT(hr);

  hr = Device->CreateRenderTargetView(pBuffer, nullptr, &RenderTargetView);
  ReleaseDXInterface(pBuffer);

  InitializeDepthBuffer();

  D3D11_VIEWPORT vp;
  vp.Width = size.x;
  vp.Height = size.y;
  vp.MinDepth = 0.0f;
  vp.MaxDepth = 1.0f;
  vp.TopLeftX = 0;
  vp.TopLeftY = 0;
  DeviceContext->RSSetViewports(1, &vp);

  InitializeD2DContext();
}

// ----------------------------------------------------------------------------

math::Vector2D WindowDevice::GetSize() const
{
  return _size;
}

// ----------------------------------------------------------------------------

void TestDrawText(GraphicsDevice::D2DData& D2D);

bool WindowDevice::BeginFrame()
{
  if (DeviceContext == nullptr)
    return false;

  DeviceContext->ClearRenderTargetView(RenderTargetView, backgroundColor.buffer());
  DeviceContext->ClearDepthStencilView(DepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

  TestDrawText(D2D);

  return true;
}

// ----------------------------------------------------------------------------

void TestDrawText(GraphicsDevice::D2DData& D2D)
{
  HRESULT hr;
  
  static ID2D1SolidColorBrush *boxBrush = nullptr;
  static ID2D1SolidColorBrush *textBrush = nullptr;
  static IDWriteTextFormat *textFormat = nullptr;

  static const WCHAR helloWorld[] = L"Hello, World!";

  static GraphicsDevice::D2DData::clock::time_point created;
  if (created < D2D.ResourceTimestamp)
  {
    ReleaseDXInterface(textFormat);
    ReleaseDXInterface(textBrush);
    ReleaseDXInterface(boxBrush);

    created = GraphicsDevice::D2DData::clock::now();

    hr = D2D.DeviceContext->CreateSolidColorBrush(D2D1::ColorF(1, 0, 0, 0.7f), &boxBrush);
    CHECK_HRESULT(hr);

    hr = D2D.DeviceContext->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), &textBrush);
    CHECK_HRESULT(hr);

    hr = D2D.WriteFactory->
      CreateTextFormat(L"Segoe Script", nullptr,
                       DWRITE_FONT_WEIGHT_EXTRA_BOLD,
                       DWRITE_FONT_STYLE_NORMAL,
                       DWRITE_FONT_STRETCH_NORMAL,
                       96, L"", &textFormat);
    CHECK_HRESULT(hr);

    hr = textFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
    CHECK_HRESULT(hr);

    hr = textFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
    CHECK_HRESULT(hr);

    auto testent = GetGame()->CurrentLevel->RootEntity->FindEntity("2DSurfaceTest");

    auto sprite = (SpriteComponent *) testent->GetComponent("SpriteComponent");
    auto texture = sprite->GetTexture(0);

    D2D.DrawTo(texture);
    auto targetSize = D2D.DeviceContext->GetSize();

    D2D.DeviceContext->FillRectangle(
      D2D1::RectF(0, 0, targetSize.width, targetSize.height),
      boxBrush);

    D2D.DeviceContext->DrawTextA(
      helloWorld, 
      ARRAYSIZE(helloWorld),
      textFormat,
      D2D1::RectF(0, 0, targetSize.width, targetSize.height),
      textBrush);

    hr = D2D.EndDraw();
    CHECK_HRESULT(hr);
  }
}

// ----------------------------------------------------------------------------

void WindowDevice::EndFrame()
{
  static bool vsync = GetGame()->initSettings.vsync;
  if (vsync)
  {
    SwapChain->Present(1, 0);
  }
  else
  {
    const double min_frame_time = 0.001;

    static auto& time = GetGame()->Time;
    while (time.CurrFrameTime < min_frame_time)
      Sleep(0);

    SwapChain->Present(0, 0);
  }
}

// ----------------------------------------------------------------------------

void GraphicsDevice::InitializeD3DContext()
{
  auto contextSize = GetSize();
  auto _window = GetContextWindow();
  CHECK_HRESULT(GetLastError());

  #pragma region Initialize Swap Chain

  DXGI_SWAP_CHAIN_DESC sd;
  ZeroMemory(&sd, sizeof(sd));
  sd.BufferCount = 2;
  sd.BufferDesc.Width = static_cast<UINT>(contextSize.x);
  sd.BufferDesc.Height = static_cast<UINT>(contextSize.y);
  sd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
  sd.BufferDesc.RefreshRate.Numerator = 120;
  sd.BufferDesc.RefreshRate.Denominator = 1;
  sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
  sd.OutputWindow = _window;
  sd.SampleDesc.Count = 1;
  sd.SampleDesc.Quality = 0;
  sd.Windowed = TRUE;

  #pragma endregion

  #pragma region Create Device and Swap Chain

  D3D_FEATURE_LEVEL  FeatureLevelsRequested[] =
  {
    D3D_FEATURE_LEVEL_11_0,
  };
  UINT               numLevelsRequested = ARRAYSIZE(FeatureLevelsRequested);
  D3D_FEATURE_LEVEL  FeatureLevelSupported;

  // this is a big function call >.> just go to
  // http://msdn.microsoft.com/en-us/library/windows/desktop/ff476879(v=vs.85).aspx
  // if you have no idea, because it's too complex to describe here
  HRESULT hr = D3D11CreateDeviceAndSwapChain(
    nullptr,
    D3D_DRIVER_TYPE_HARDWARE,
    nullptr,
    D3D11_CREATE_DEVICE_BGRA_SUPPORT,
    FeatureLevelsRequested,
    numLevelsRequested,
    D3D11_SDK_VERSION,
    &sd,
    &SwapChain,
    &Device,
    &FeatureLevelSupported,
    &DeviceContext);
  CHECK_HRESULT(hr);

  hr = ConvertInterface<IDXGIDevice>(Device, &FactoryDevice);
  CHECK_HRESULT(hr);

  ID3D11Texture2D *backBuffer;
  hr = SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D),
                            reinterpret_cast<LPVOID *>(&backBuffer));
  CHECK_HRESULT(hr);

  hr = Device->CreateRenderTargetView(backBuffer, nullptr, &RenderTargetView);
  CHECK_HRESULT(hr);

  ReleaseDXInterface(backBuffer);

  #pragma endregion

  InitializeDepthBuffer();

  #pragma region Rasterizer

  D3D11_RASTERIZER_DESC rasterDesc;
  rasterDesc.CullMode = GetGame()->initSettings.cullTriangles ? D3D11_CULL_BACK : D3D11_CULL_NONE;
  rasterDesc.DepthBias = 0;
  rasterDesc.DepthBiasClamp = 0.0f;
  rasterDesc.DepthClipEnable = false;
  rasterDesc.FillMode = D3D11_FILL_SOLID;
  rasterDesc.FrontCounterClockwise = false;
  rasterDesc.MultisampleEnable = true;
  rasterDesc.ScissorEnable = false;
  rasterDesc.SlopeScaledDepthBias = 0.0f;
  rasterDesc.AntialiasedLineEnable = true;

  hr = Device->CreateRasterizerState(&rasterDesc, &RasterState);
  CHECK_HRESULT(hr);

  DeviceContext->RSSetState(RasterState);

  #pragma endregion

  #pragma region Viewport

  D3D11_VIEWPORT viewport;
  viewport.Width = contextSize.x;
  viewport.Height = contextSize.y;
  viewport.MinDepth = 0.0f;
  viewport.MaxDepth = 1.0f;
  viewport.TopLeftX = 0.0f;
  viewport.TopLeftY = 0.0f;

  DeviceContext->RSSetViewports(1, &viewport);

  #pragma endregion

  #pragma region Blend State

  D3D11_BLEND_DESC blendStateDesc;
  ZeroMemory(&blendStateDesc, sizeof(D3D11_BLEND_DESC));
  blendStateDesc.AlphaToCoverageEnable = TRUE;
  blendStateDesc.IndependentBlendEnable = FALSE;
  blendStateDesc.RenderTarget[0].BlendEnable = TRUE;
  blendStateDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
  blendStateDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
  blendStateDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
  blendStateDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA;
  blendStateDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_DEST_ALPHA;
  blendStateDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
  blendStateDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

  HRESULT result = Device->CreateBlendState(&blendStateDesc, &BlendState);
  CHECK_HRESULT(result);

  DeviceContext->OMSetBlendState(BlendState, nullptr, 0xFFFFFF);

  #pragma endregion

}

// ----------------------------------------------------------------------------

void GraphicsDevice::InitializeDepthBuffer()
{
  HRESULT hr;

  ReleaseDXInterface(DepthStencilBuffer);
  ReleaseDXInterface(DepthStencilState);
  ReleaseDXInterface(DepthStencilView);

  #pragma region Depth Stencil Buffer

  D3D11_TEXTURE2D_DESC depthBufferDesc;
  ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));
  depthBufferDesc.Width = static_cast<UINT>(GetSize().x);
  depthBufferDesc.Height = static_cast<UINT>(GetSize().y);
  depthBufferDesc.MipLevels = 1;
  depthBufferDesc.ArraySize = 1;
  depthBufferDesc.Format = DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
  depthBufferDesc.SampleDesc.Count = 1;
  depthBufferDesc.SampleDesc.Quality = 0;
  depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
  depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
  depthBufferDesc.CPUAccessFlags = 0;
  depthBufferDesc.MiscFlags = 0;

  hr = Device->CreateTexture2D(&depthBufferDesc, nullptr, &DepthStencilBuffer);
  CHECK_HRESULT(hr);

  #pragma endregion

  #pragma region Depth Stencil State

  D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
  ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

  depthStencilDesc.DepthEnable = TRUE;
  depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
  depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

  depthStencilDesc.StencilEnable = TRUE;
  depthStencilDesc.StencilReadMask = 0xFF;
  depthStencilDesc.StencilWriteMask = 0xFF;

  depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
  depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
  depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
  depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

  depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
  depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
  depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
  depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

  hr = Device->CreateDepthStencilState(&depthStencilDesc, &DepthStencilState);
  CHECK_HRESULT(hr);

  DeviceContext->OMSetDepthStencilState(DepthStencilState, 1);

  #pragma endregion

  #pragma region Depth Stencil View Desc

  D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
  ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));
  depthStencilViewDesc.Format = DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
  depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
  depthStencilViewDesc.Texture2D.MipSlice = 0;

  hr = Device->CreateDepthStencilView(DepthStencilBuffer, &depthStencilViewDesc, &DepthStencilView);
  CHECK_HRESULT(hr);

  DeviceContext->OMSetRenderTargets(1, &RenderTargetView, DepthStencilView);

  #pragma endregion

}

// ----------------------------------------------------------------------------

void GraphicsDevice::CreateInputLayout(byte *bytecode,
                                       UINT bytecodeSize,
                                       D3D11_INPUT_ELEMENT_DESC *layoutDesc,
                                       UINT layoutDescNumElements,
                                       ID3D11InputLayout **layout)
{
  Device->CreateInputLayout(
    layoutDesc,
    layoutDescNumElements,
    bytecode,
    bytecodeSize,
    layout);
}

// ----------------------------------------------------------------------------

void GraphicsDevice::InitializeD2DContext()
{
  HRESULT hr;

  hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &D2D.Factory);
  CHECK_HRESULT(hr);

  hr = D2D.Factory->CreateDevice(FactoryDevice, &D2D.Device);
  CHECK_HRESULT(hr);

  hr = D2D.Device->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &D2D.DeviceContext);
  CHECK_HRESULT(hr);

  D2D.DeviceContext->SetDpi(96, 96);

  hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, 
                           __uuidof(*D2D.WriteFactory),
                           reinterpret_cast<IUnknown **>(&D2D.WriteFactory));
  CHECK_HRESULT(hr);

  *&D2D.ResourceTimestamp = D2DData::clock::now();
}

// ----------------------------------------------------------------------------

void DisplayAdapter::GetAdapters(std::vector<DisplayAdapter>& adapters)
{
  IDXGIFactory1 *dxgFactory = nullptr;
  auto result = CreateDXGIFactory1(__uuidof(IDXGIFactory1), reinterpret_cast<void**>(&dxgFactory));
  CHECK_HRESULT(result);
  RELEASE_AFTER_SCOPE(dxgFactory);

  IDXGIAdapter1 *dxgAdapter;
  for (UINT i = 0; dxgFactory->EnumAdapters1(i, &dxgAdapter) != DXGI_ERROR_NOT_FOUND; ++i)
  {
    adapters.emplace_back(dxgAdapter);

    auto last = adapters.end() - 1;
    if (last->DisplayOutputs.empty())
      adapters.pop_back();

    ReleaseDXInterface(dxgAdapter);
  }
}

// ----------------------------------------------------------------------------

DisplayAdapter::DisplayAdapter(IDXGIAdapter *dxgAdapter)
{
  IDXGIOutput *dxgOutput = nullptr;
  for (UINT i = 0; dxgAdapter->EnumOutputs(i, &dxgOutput) != DXGI_ERROR_NOT_FOUND; ++i)
  {
    DisplayOutputs.emplace_back(dxgOutput);

    ReleaseDXInterface(dxgOutput);
  }
}

// ----------------------------------------------------------------------------

DisplayOutput::DisplayOutput(IDXGIOutput *dxgOutput)
{
  auto hr = dxgOutput->GetDesc(this);
  CHECK_HRESULT(hr);

  UINT numModes = 0;
  const DXGI_FORMAT format = DXGI_FORMAT_B8G8R8A8_UNORM;
  const UINT flags = 0;

  hr = dxgOutput->GetDisplayModeList(format, flags, &numModes, nullptr);
  CHECK_HRESULT(hr);

  std::vector<DXGI_MODE_DESC> modeStructs(numModes);
  hr = dxgOutput->GetDisplayModeList(format, flags, &numModes, modeStructs.data());
  CHECK_HRESULT(hr);

  for (auto& mode : modeStructs)
    DisplayModes.emplace_back(mode);
}

// ----------------------------------------------------------------------------

void DisplayOutput::CreateResolutionList(std::vector<DisplaySetting>& settings)
{
  for (auto& mode : DisplayModes)
  {
    auto pred = [&mode] (const DisplaySetting& setting)
    {
      return mode.Width == setting.Width &&
             mode.Height == setting.Height;
    };

    auto it = std::find_if(settings.begin(), settings.end(), pred);
    if (it != settings.end())
    {
      it->Variants.push_back(mode);
    }
    else
    {
      settings.emplace_back(mode);
    }
  }
}

// ----------------------------------------------------------------------------

DisplayMode::DisplayMode(const DXGI_MODE_DESC& desc)
  : DXGI_MODE_DESC(desc)
{
}

// ----------------------------------------------------------------------------

void GraphicsDevice::D2DData::DrawTo(Texture2D texture)
{
  assert(texture.RenderTarget);

  DeviceContext->SetTarget(texture.RenderTarget);
  DeviceContext->BeginDraw();
  DeviceContext->Clear(D2D1::ColorF(1, 1, 1, 0));
}

// ----------------------------------------------------------------------------

HRESULT GraphicsDevice::D2DData::EndDraw()
{
  return DeviceContext->EndDraw();
}

// ----------------------------------------------------------------------------

