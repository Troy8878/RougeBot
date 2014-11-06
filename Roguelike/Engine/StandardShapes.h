#pragma once

#include "Helpers\UseDirectX.h"
#include "Model.h"

// ----------------------------------------------------------------------------

namespace Shapes
{
  Model *MakeRectangle(ID3D11Device *device, math::Vector2D size, math::Vector color = {1.f,1.f,1.f,1.f});
  Model *MakeCircle(ID3D11Device *device, unsigned sides, float radius, math::Vector color = {1.f,1.f,1.f,1.f});
  Model *MakeCube(ID3D11Device *device, float size, math::Vector color = {1.f,1.f,1.f,1.f});
}

// ----------------------------------------------------------------------------