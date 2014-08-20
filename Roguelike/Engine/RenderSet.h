/*********************************
 * RenderSet.h
 * Connor Hilarides
 * Created 2014/08/20
 *********************************/

#pragma once

#include "Helpers\FixedWindows.h"
#include <unordered_map>
#include <vector>

// ----------------------------------------------------------------------------

struct Shader;
struct Camera;

// ----------------------------------------------------------------------------

INTERFACE Drawable
{
  void Draw();
};

// ----------------------------------------------------------------------------

class RenderSet
{
public:
  RenderSet();

  void AddDrawable(Drawable *drawable, Shader *shader);

private:
  std::unordered_map<Shader *, std::vector<Drawable *>> drawables;
};

// ----------------------------------------------------------------------------

class RenderGroup
{
public:
  RenderGroup();

  void CreateSet(Camera *)

private:
  std::unordered_map<std::string, RenderSet *> sets;
};

// ----------------------------------------------------------------------------

