/*********************************
 * TextComponent.cpp
 * Connor Hilarides
 * Created 2014/09/22
 *********************************/

#include "Common.h"
#include "TextComponent.h"
#include "TextureComponent.h"

#include "mruby/array.h"

#pragma warning (disable : 4127)

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

void TextComponent::PopulateTextureComponent(const D2D1_SIZE_F& size)
{
  drawing.populateSize = size;
  drawing.shouldPopulate = true;
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

  if (shouldPopulate)
  {
    DoPopulate();
    shouldPopulate = false;
  }

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

void TextComponent::DrawingResources::DoPopulate()
{
  auto& tc = *textures;

  // Clear old textures in case someone defined them
  while (tc.TextureCount)
    tc.RemoveTexture(tc.TextureCount - 1);

  std::ostringstream texturename;
  texturename << "SPECIAL/SURFACE/";
  texturename << int(populateSize.width);
  texturename << ":";
  texturename << int(populateSize.height);

  for (size_t i = 0; i < texts.size(); ++i)
    tc.AddTexture(json::value::string(texturename.str()));
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

  it = data.find("autocreate_textures");
  if (it != data.end())
  {
    auto jsize = it->second.as_array_of<json::value::number_t>();
    component->PopulateTextureComponent(D2D1::SizeF((FLOAT)jsize[0], (FLOAT)jsize[1]));
  }

  return component;
}

// ----------------------------------------------------------------------------

static RClass *cbase;

static mrb_data_type mrb_textcomp_data_type;

static mrb_value mrb_textcomp_new(mrb_state *mrb, TextComponent *comp);
static void mrb_textcomp_free(mrb_state *, void *) {}

static mrb_value mrb_textcomp_to_a(mrb_state *mrb, mrb_value self);
static mrb_value mrb_textcomp_get_text_at(mrb_state *mrb, mrb_value self);
static mrb_value mrb_textcomp_set_text_at(mrb_state *mrb, mrb_value self);
static mrb_value mrb_textcomp_texts(mrb_state *mrb, mrb_value self);
static mrb_value mrb_textcomp_texts_set(mrb_state *mrb, mrb_value self);
static mrb_value mrb_textcomp_font_get(mrb_state *mrb, mrb_value self);
static mrb_value mrb_textcomp_font_set(mrb_state *mrb, mrb_value self);
static mrb_value mrb_textcomp_font_size_get(mrb_state *mrb, mrb_value self);
static mrb_value mrb_textcomp_font_size_set(mrb_state *mrb, mrb_value self);
static mrb_value mrb_textcomp_text_color_get(mrb_state *mrb, mrb_value self);
static mrb_value mrb_textcomp_text_color_set(mrb_state *mrb, mrb_value self);
static mrb_value mrb_textcomp_bg_color_get(mrb_state *mrb, mrb_value self);
static mrb_value mrb_textcomp_bg_color_set(mrb_state *mrb, mrb_value self);

//static mrb_value mrb_textcomp_(mrb_state *mrb, mrb_value self);

// ----------------------------------------------------------------------------

static void mrb_textcomp_gem_init(mrb_state *mrb)
{
  mrb_textcomp_data_type.dfree = mrb_textcomp_free;
  mrb_textcomp_data_type.struct_name = "TextComponent";
  
  auto rmod = mrb_module_get(mrb, "Components");
  auto rclass = mrb_define_class_under(mrb, rmod, "TextComponent", cbase);

  mrb_define_class_method(mrb, rclass, "new", mrb_nop, ARGS_ANY());

  mrb_define_method(mrb, rclass, "to_a", mrb_textcomp_to_a, ARGS_NONE());
  mrb_define_method(mrb, rclass, "texts", mrb_textcomp_texts, ARGS_NONE());
  //mrb_define_method(mrb, rclass, "texts=", mrb_textcomp_texts_set, ARGS_REQ(1));
}

// ----------------------------------------------------------------------------

mrb_value TextComponent::GetRubyWrapper()
{
  RUN_ONCE(cbase = Component::GetComponentRClass(),
           mrb_textcomp_gem_init(*mrb_inst));

  return mrb_textcomp_new(*mrb_inst, this);
}

// ----------------------------------------------------------------------------

static mrb_value mrb_textcomp_new(mrb_state *mrb, TextComponent *comp)
{
  auto rmod = mrb_module_get(mrb, "Components");
  auto rclass = mrb_class_get_under(mrb, rmod, "TextComponent");

  auto obj = mrb_data_object_alloc(mrb, rclass, comp, &mrb_textcomp_data_type);
  return mrb_obj_value(obj);
}

// ----------------------------------------------------------------------------

static mrb_value mrb_textcomp_to_a(mrb_state *mrb, mrb_value self)
{
  mrb_value ary = mrb_ary_new(mrb);
  
  auto *tcomp = (TextComponent *) mrb_data_get_ptr(mrb, self, &mrb_textcomp_data_type);

  for (auto& text : *tcomp)
  {
    (text);
  }

  return ary;
}

// ----------------------------------------------------------------------------

static mrb_value mrb_textcomp_get_text_at(mrb_state *mrb, mrb_value self)
{
  mrb_int index;
  mrb_get_args(mrb, "i", &index);

  auto *tcomp = (TextComponent *) mrb_data_get_ptr(mrb, self, &mrb_textcomp_data_type);
  auto& text = tcomp->GetText((size_t) index);

  return mrb_str_new(mrb, text.c_str(), text.size());
}

// ----------------------------------------------------------------------------

static mrb_value mrb_textcomp_set_text_at(mrb_state *mrb, mrb_value self)
{
  mrb_int index;
  mrb_value str;
  mrb_get_args(mrb, "iS", &index, &str);
  
  auto *tcomp = (TextComponent *) mrb_data_get_ptr(mrb, self, &mrb_textcomp_data_type);
  auto text = mrb_str_to_stdstring(str);

  tcomp->SetText((size_t) index, text);
}

// ----------------------------------------------------------------------------

static mrb_value mrb_textcomp_texts(mrb_state *mrb, mrb_value self)
{
  auto tclass = mrb_obj_class(mrb, self);
  auto rclass = mrb_class_get_under(mrb, tclass, "TextArray");

  return mrb_obj_new(mrb, rclass, 1, &self);
}

// ----------------------------------------------------------------------------

static mrb_value mrb_textcomp_texts_set(mrb_state *mrb, mrb_value self);

// ----------------------------------------------------------------------------

static mrb_value mrb_textcomp_font_get(mrb_state *mrb, mrb_value self);

// ----------------------------------------------------------------------------

static mrb_value mrb_textcomp_font_set(mrb_state *mrb, mrb_value self);

// ----------------------------------------------------------------------------

static mrb_value mrb_textcomp_font_size_get(mrb_state *mrb, mrb_value self);

// ----------------------------------------------------------------------------

static mrb_value mrb_textcomp_font_size_set(mrb_state *mrb, mrb_value self);

// ----------------------------------------------------------------------------

static mrb_value mrb_textcomp_text_color_get(mrb_state *mrb, mrb_value self);

// ----------------------------------------------------------------------------

static mrb_value mrb_textcomp_text_color_set(mrb_state *mrb, mrb_value self);

// ----------------------------------------------------------------------------

static mrb_value mrb_textcomp_bg_color_get(mrb_state *mrb, mrb_value self);

// ----------------------------------------------------------------------------

static mrb_value mrb_textcomp_bg_color_set(mrb_state *mrb, mrb_value self);

// ----------------------------------------------------------------------------

