/*********************************
 * PositionDisplayComponent.cpp
 * YOUR NAME HERE
 * Created 2014/YOUR DATE HERE
 *********************************/

#include "Common.h"
#include "PositionDisplayComponent.h"
#include "Engine/Level.h"
#include "Engine/TextureComponent.h"
#include "Engine/TransformComponent.h"

// ----------------------------------------------------------------------------

PositionDisplayComponentFactory PositionDisplayComponent::factory;

// ----------------------------------------------------------------------------

PositionDisplayComponent::PositionDisplayComponent(const std::string& entity)
{
  entityName = new std::string{entity};
}

// ----------------------------------------------------------------------------

void PositionDisplayComponent::Initialize(Entity *owner, const std::string& name)
{
  Component::Initialize(owner, name);

  static Events::EventId update{"update"};
  Owner->AddEvent(this, update, &PositionDisplayComponent::OnFirstUpdate);

  texture = Owner->GetComponent<TextureComponent>("TextureComponent")->Textures[0];
}

// ----------------------------------------------------------------------------

void PositionDisplayComponent::OnFirstUpdate(Events::EventMessage&)
{
  auto entity =  GetGame()->CurrentLevel->RootEntity->FindEntity(*entityName);
  watchedTransform = entity->GetComponent<TransformComponent>("TransformComponent");
  
  delete entityName;
  entityName = nullptr;

  DEF_EVENT_ID(update);
  Owner->RemoveEvent(this, update);
  Owner->AddEvent(this, update, &PositionDisplayComponent::OnUpdate);
  
  lastPos = watchedTransform->Position;
  DrawDisplay();
}

// ----------------------------------------------------------------------------

void PositionDisplayComponent::OnUpdate(Events::EventMessage&)
{
  auto newPos = watchedTransform->Position;
  if (newPos != lastPos)
  {
    lastPos = newPos;
    DrawDisplay();
  }
}

// ----------------------------------------------------------------------------

void PositionDisplayComponent::DrawDisplay()
{
  drawing.Validate();
  
  auto& d2d = GetGame()->GameDevice->D2D;
  d2d.DrawTo(texture);

  auto targetSize = d2d.DeviceContext->GetSize();
  auto rect = D2D1::RectF(0, 0, targetSize.width, targetSize.height);

  std::wstringstream textbuf;
  textbuf << L"x: " << int(lastPos.x + 0.5f) << L" ";
  textbuf << L"y: " << int(lastPos.z + 0.5f);
  auto text = textbuf.str();

  d2d.DeviceContext->DrawText(text.c_str(), (UINT) text.size(),
                              drawing.format, rect, drawing.brush);

  d2d.EndDraw();
}

// ----------------------------------------------------------------------------

void PositionDisplayComponent::DrawingResources::Validate()
{
  auto& d2d = GetGame()->GameDevice->D2D;

  if (timestamp >= d2d.ResourceTimestamp)
    return;

  Release();
  d2d.DeviceContext->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), &brush);
  d2d.WriteFactory->CreateTextFormat(
    L"", // Font
    nullptr,
    DWRITE_FONT_WEIGHT_NORMAL,
    DWRITE_FONT_STYLE_NORMAL,
    DWRITE_FONT_STRETCH_NORMAL,
    72, // font size
    nullptr, // locale
    &format
  );
}

// ----------------------------------------------------------------------------

void PositionDisplayComponent::DrawingResources::Release()
{
  ReleaseDXInterface(brush);
  ReleaseDXInterface(format);
}

// ----------------------------------------------------------------------------

PositionDisplayComponentFactory::PositionDisplayComponentFactory()
  : allocator(sizeof(PositionDisplayComponent))
{
}

// ----------------------------------------------------------------------------

Component *PositionDisplayComponentFactory::CreateObject(
  void *memory, component_factory_data& data)
{
  auto& name = data["target_name"].as_string();

  auto *component = new (memory) PositionDisplayComponent(name);
  return component;
}

// ----------------------------------------------------------------------------

mrb_value PositionDisplayComponent::GetRubyWrapper()
{
  return mrb_nil_value();
}

// ----------------------------------------------------------------------------
