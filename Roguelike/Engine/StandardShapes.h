#pragma once

#include "Helpers\UseDirectX.h"
#include "Model.h"

namespace Shapes
{
  Model *makeRectangle(ID3D11Device *device, math::Vector2D size, math::Vector2D center = {0, 0});
  Model *makeCircle(ID3D11Device *device, unsigned sides, float radius, math::Vector color = {1,1,1,1});
  Model *makeCube(ID3D11Device *device, float size, math::Vector color = {1,1,1,1});
}


