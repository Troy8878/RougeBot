/*********************************
 * ModelBuilder.h
 * Connor Hilarides
 * Created 2014/09/09
 * Copyright © 2014 DigiPen Institute of Technology, All Rights Reserved
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
  UINT SaveVertex(const TexturedVertex &vertex);

  std::vector<TexturedVertex> vertices;
  std::vector<UINT> indices;

  template <typename Key>
  struct int64_hashmap : public std::unordered_map<int64_t, Key>
  {
  };

  struct textureindex : public std::vector<std::pair<TexturedVertex, UINT>>
  {
  };

  int64_hashmap<int64_hashmap<int64_hashmap<textureindex>>> vertexIndex;

  GraphicsDevice *graphics;
};

// ----------------------------------------------------------------------------

class TilemapBuilder
{
public:
  TilemapBuilder(size_t zipSize);
  ~TilemapBuilder();

  void AddTile(mrb_int x, mrb_int y, UINT texture);

  Model *CreateModel();

  static void InitializeRubyModule(mrb_state *mrb);

private:
  size_t zipSize;
  std::vector<TexturedVertex> vertices;
  std::vector<UINT> indices;

  GraphicsDevice *graphics;
};

// ----------------------------------------------------------------------------