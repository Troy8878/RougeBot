/*********************************
 * RenderSet.cpp
 * Connor Hilarides
 * Created 2014/08/20
 *********************************/

#include "Common.h"
#include "RenderSet.h"
#include "Shader.h"

// ----------------------------------------------------------------------------

RenderSet::RenderSet(Camera *camera)
  : camera(camera)
{
}

// ----------------------------------------------------------------------------

void RenderSet::AddDrawable(Drawable *drawable, Shader *shader)
{
  drawables.push_back({drawable, shader});
  std::sort(drawables.begin(), drawables.end());
}

// ----------------------------------------------------------------------------

void RenderSet::RemoveDrawable(Drawable *drawable)
{
  for (auto it = drawables.begin(); it != drawables.end(); ++it)
  {
    if (it->drawable != drawable)
      continue;

    drawables.erase(it);
  }
}

// ----------------------------------------------------------------------------

void RenderSet::Draw()
{
  Shader *shader = nullptr;

  for (auto& pair : drawables)
  {
    if (pair.shader != shader)
    {
      shader = pair.shader;
      shader->camera = camera;
    }
    
    pair.drawable->Draw();
  }
}

// ----------------------------------------------------------------------------

RenderGroup::RenderGroup()
  : BasicClassEventReciever(this)
{
}

// ----------------------------------------------------------------------------

RenderSet *RenderGroup::CreateSet(const std::string& name, Camera *camera)
{
  return sets[name] = new RenderSet(camera);
}

// ----------------------------------------------------------------------------

void RenderGroup::RemoveSet(const std::string& name)
{
  delete sets[name];
  sets.erase(name);
}

// ----------------------------------------------------------------------------

void RenderGroup::Draw(Events::EventMessage&)
{
  for (auto& set : sets)
  {
    set.second->Draw();
  }
}

// ----------------------------------------------------------------------------

