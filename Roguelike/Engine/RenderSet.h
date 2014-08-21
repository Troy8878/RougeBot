/*********************************
 * RenderSet.h
 * Connor Hilarides
 * Created 2014/08/20
 *********************************/

#pragma once

#include "Helpers\FixedWindows.h"
#include "Event.h"
#include "EventHandlers.h"
#include <unordered_map>
#include <vector>

// ----------------------------------------------------------------------------

struct Shader;
struct Camera;

// ----------------------------------------------------------------------------

__interface Drawable
{
  void Draw();
};

// ----------------------------------------------------------------------------

class RenderSet
{
public:
  RenderSet(Camera *camera);

  void AddDrawable(Drawable *drawable, Shader *shader);
  void RemoveDrawable(Drawable *drawable);

  void Draw();

private:
  struct DrawablePair
  {
    DrawablePair() = default;
    DrawablePair(Drawable *drawable, Shader *shader)
      : drawable(drawable), shader(shader)
    {
    }
    
    Drawable *drawable = nullptr;
    Shader *shader = nullptr;

    inline bool operator<(const DrawablePair& other)
    {
      return shader < other.shader;
    }
  };

  std::vector<DrawablePair> drawables;
  Camera *camera;
};

// ----------------------------------------------------------------------------

class RenderGroup : Events::BasicClassEventReciever<RenderGroup>
{
public:
  RenderGroup();

  RenderSet *CreateSet(const std::string& name, Camera *camera);
  void RemoveSet(const std::string& name);

  void Draw(Events::EventMessage&);

private:
  std::unordered_map<std::string, RenderSet *> sets;
};

// ----------------------------------------------------------------------------

