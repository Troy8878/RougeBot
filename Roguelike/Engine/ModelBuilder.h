/*********************************
 * ModelBuilder.h
 * Connor Hilarides
 * Created 2014/09/09
 *********************************/

#pragma once

#include "Common.h"
#include "Model.h"

// ----------------------------------------------------------------------------

class ModelBuilder
{
public:
  ModelBuilder();
  ~ModelBuilder();

  void AddTri(const TexturedVertex vertices[3]);
  void AddQuad(const TexturedVertex vertices[4]);

  Model *CreateModel();

  static void InitializeRubyModule(mrb_state *mrb);

private:
  UINT SaveVertex(const TexturedVertex& vertex);

  std::vector<TexturedVertex> vertices;
  std::vector<UINT> indices;
  GraphicsDevice *graphics;
};

// ----------------------------------------------------------------------------
