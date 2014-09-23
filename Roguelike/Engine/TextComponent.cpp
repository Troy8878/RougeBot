/*********************************
 * TextComponent.cpp
 * Connor Hilarides
 * Created 2014/09/22
 *********************************/

#include "Common.h"
#include "TextComponent.h"
#include "TextureComponent.h"

// ----------------------------------------------------------------------------

TextComponentFactory TextComponent::factory;

// ----------------------------------------------------------------------------

TextComponent::TextComponent()
{
}

// ----------------------------------------------------------------------------

void TextComponent::Initialize(Entity *owner, const std::string& name)
{
  Component::Initialize(owner, name);

  drawing.textures = Owner->GetComponent<TextureComponent>("TextureComponent");

  DEF_EVENT_ID(update);
  Owner->AddEvent(this, update, &TextComponent::OnUpdate);
}

// ----------------------------------------------------------------------------

const std::string& TextComponent::GetText(size_t index)
{
  if (drawing.texts.size() <= index)
    drawing.texts.resize(index + 1);

  return drawing.texts[index];
}

// ----------------------------------------------------------------------------
  
void TextComponent::SetText(size_t index, const std::string& text)
{
  if (drawing.texts.size() <= index)
    drawing.texts.resize(index + 1);

  drawing.texts[index] = text;
  OnChanged();
}

// ----------------------------------------------------------------------------

void TextComponent::AppendText(const std::string& text)
{
  drawing.texts.emplace_back(text);
  OnChanged();
}

// ----------------------------------------------------------------------------

void TextComponent::OnUpdate(Events::EventMessage&)
{
  drawing.Validate();
}

// ----------------------------------------------------------------------------

void TextComponent::DrawingResources::Release()
{
  ReleaseDXInterface(textFormat);
  ReleaseDXInterface(textBrush);
  ReleaseDXInterface(bgBrush);
}

// ----------------------------------------------------------------------------

void TextComponent::DrawingResources::Validate()
{
  if (!textures)
    return;

  auto& d2d = GetGame()->GameDevice->D2D;

  if (timestamp >= d2d.ResourceTimestamp)
    return;

  Release();

  HRESULT hr;
  ID2D1SolidColorBrush *scBrush;

  hr = d2d.DeviceContext->CreateSolidColorBrush(textColor, &scBrush);
  CHECK_HRESULT(hr);
  textBrush = scBrush;

  hr = d2d.DeviceContext->CreateSolidColorBrush(bgColor, &scBrush);
  CHECK_HRESULT(hr);
  bgBrush = scBrush;

  hr = d2d.WriteFactory->CreateTextFormat(
    font.c_str(), 
    nullptr,
    DWRITE_FONT_WEIGHT_NORMAL,
    DWRITE_FONT_STYLE_NORMAL,
    DWRITE_FONT_STRETCH_NORMAL,
    fontSize,
    L"",
    &textFormat);
  CHECK_HRESULT(hr);

  hr = textFormat->SetTextAlignment(textAlign);
  CHECK_HRESULT(hr);

  hr = textFormat->SetParagraphAlignment(paragAlign);
  CHECK_HRESULT(hr);

  timestamp = clock::now();

  Draw();
}

// ----------------------------------------------------------------------------

void TextComponent::DrawingResources::Draw()
{
  auto& d2d = GetGame()->GameDevice->D2D;
  auto& textures = this->textures->Textures;

  for (size_t i = 0; i < texts.size() && i < textures.size(); ++i)
  {
    d2d.DrawTo(textures[i]);
    
    auto targetSize = d2d.DeviceContext->GetSize();
    auto rect = D2D1::RectF(0, 0, targetSize.width, targetSize.height);

    auto text = widen(texts[i]);
    
    // Clear the buffer and draw the new content
    d2d.DeviceContext->Clear(D2D1::ColorF(1, 1, 1, 0));
    d2d.DeviceContext->FillRectangle(rect, bgBrush);
    d2d.DeviceContext->DrawText(text.c_str(), (UINT) text.size(),
                                textFormat, rect, textBrush);

    HRESULT hr = d2d.EndDraw();
    CHECK_HRESULT(hr);
  }
}

// ----------------------------------------------------------------------------

TextComponentFactory::TextComponentFactory()
  : allocator(sizeof(TextComponent))
{
}

// ----------------------------------------------------------------------------

Component *TextComponentFactory::CreateObject(
  void *memory, component_factory_data& data)
{
  auto *component = new (memory) TextComponent;

  component_factory_data::iterator it;

  it = data.find("text");
  if (it != data.end())
  {
    component->AppendText(it->second.as_string());
  }

  it = data.find("texts");
  if (it != data.end())
  {
    auto& texts = it->second.as_array();
    for (auto& text : texts)
    {
      component->AppendText(text.as_string());
    }
  }

  component->TextColor = StringToColor(data["text_color"].as_string());
  component->BGColor = StringToColor(data["bg_color"].as_string());

  component->Font = widen(data["font"].as_string());
  component->FontSize = (FLOAT) map_fetch(data, "font_size", 48).as_number();
  
  auto& talign = data["text_align"].as_string();
  if (talign == "center")
    component->TextAlign = DWRITE_TEXT_ALIGNMENT_CENTER;
  else if (talign == "justified")
    component->TextAlign = DWRITE_TEXT_ALIGNMENT_JUSTIFIED;
  else if (talign == "leading")
    component->TextAlign = DWRITE_TEXT_ALIGNMENT_LEADING;
  else if (talign == "trailing")
    component->TextAlign = DWRITE_TEXT_ALIGNMENT_TRAILING;
  
  auto& palign = data["paragraph_align"].as_string();
  if (palign == "center")
    component->ParagraphAlign = DWRITE_PARAGRAPH_ALIGNMENT_CENTER;
  else if (palign == "near")
    component->ParagraphAlign = DWRITE_PARAGRAPH_ALIGNMENT_NEAR;
  else if (palign == "far")
    component->ParagraphAlign = DWRITE_PARAGRAPH_ALIGNMENT_FAR;

  return component;
}

// ----------------------------------------------------------------------------

mrb_value TextComponent::GetRubyWrapper()
{
  ONE_TIME_MESSAGE("[WARN] TODO: Implement ruby wrapper for TextComponent");
  return mrb_nil_value();
}

// ----------------------------------------------------------------------------
