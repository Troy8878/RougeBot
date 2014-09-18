/*********************************
 * FixedWindows.h
 * Connor Hilarides
 * Created 2014/05/28
 *********************************/

#pragma once

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#include <wincodec.h>
#include <comdef.h>
#include <locale>
#include <codecvt>
#include <string>

#pragma region DirectX stuff

#include <DirectXMath.h>
#include <d3d11.h>
#include <d2d1_1.h>
#include <d2d1_1helper.h>
#include <dxgi1_2.h>
#include <dwrite_1.h>

typedef DirectX::XMVECTOR XVector;

#include "Helpers\VectorHelp.h"

#define ARRAY_STRIDE(arr) ((UINT)(reinterpret_cast<BYTE *>((arr) + 1) - reinterpret_cast<BYTE *>(arr)))

template <typename To, typename From, typename Holder>
HRESULT ConvertInterface(From *current, Holder **result)
{
  return current->QueryInterface(__uuidof(To), reinterpret_cast<void **>(result));
}

#pragma endregion

#include "RandomHelpers.h"
