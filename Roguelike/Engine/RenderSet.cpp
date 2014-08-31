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
  : _RenderCamera(camera)
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
  for (auto& pair : drawables)
  {
    pair.shader->camera = RenderCamera;
    pair.drawable->Draw();
  }
}

// ----------------------------------------------------------------------------

RenderGroup RenderGroup::Instance;

// ----------------------------------------------------------------------------

RenderGroup::RenderGroup()
  : BasicClassEventReciever(this)
{
}

// ----------------------------------------------------------------------------

RenderSet *RenderGroup::GetSet(const std::string& name)
{
  return sets.find(name)->second.first;
}

// ----------------------------------------------------------------------------

RenderSet *RenderGroup::CreateSet(const std::string& name, Camera *camera, bool perma)
{
  std::pair<RenderSet *, bool> pair{new RenderSet(camera), perma};
  sets[name] = pair;
  return pair.first;
}

// ----------------------------------------------------------------------------

void RenderGroup::RemoveSet(const std::string& name)
{
  delete sets[name].first;
  sets.erase(name);
}

// ----------------------------------------------------------------------------

void RenderGroup::ClearSets()
{
  for (auto& pair : sets)
  {
    if (pair.second.second)
      continue;

    RemoveSet(pair.first);
    ClearSets();
    return;
  }
}

// ----------------------------------------------------------------------------

void RenderGroup::Draw(Events::EventMessage&)
{
  for (auto& set : sets)
  {
    set.second.first->Draw();
  }
}

// ----------------------------------------------------------------------------

