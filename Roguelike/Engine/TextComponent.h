/*********************************
 * TextComponent.h
 * Connor Hilarides
 * Created 2014/09/22
 *********************************/

#pragma once

#include "Common.h"
#include "Helpers\BucketAllocator.h"

// ----------------------------------------------------------------------------

class TextComponentFactory;
class TextureComponent;

// ----------------------------------------------------------------------------

class TextComponent : public Component
{
public:
  TextComponent();

  void Initialize(Entity *owner, const std::string& name) override;

  DRAWING_PROPERTY(D2D1::ColorF, TextColor, drawing.textColor);
  DRAWING_PROPERTY(D2D1::ColorF, BGColor, drawing.bgColor);
  DRAWING_PROPERTY(DWRITE_TEXT_ALIGNMENT, TextAlign, drawing.textAlign);
  DRAWING_PROPERTY(DWRITE_PARAGRAPH_ALIGNMENT, ParagraphAlign, drawing.paragAlign);
  DRAWING_PROPERTY(std::wstring, Font, drawing.font);
  DRAWING_PROPERTY(FLOAT, FontSize, drawing.fontSize);
  
  const std::string& GetText(size_t index);
  void SetText(size_t index, const std::string& text);
  void AppendText(const std::string& text);
  void PopulateTextureComponent(const D2D1_SIZE_F& size);

  void OnUpdate(Events::EventMessage&);

  mrb_value GetRubyWrapper() override;

  static TextComponentFactory factory;

private:
  
  typedef GraphicsDevice::D2DData::clock clock;
  struct DrawingResources final
  {
    clock::time_point timestamp = clock::from_time_t(0);
    
    std::vector<std::string> texts;
    D2D1::ColorF textColor = D2D1::ColorF::Black;
    D2D1::ColorF bgColor = D2D1::ColorF(1, 1, 1, 0);
    TextureComponent *textures = nullptr;

    DWRITE_TEXT_ALIGNMENT textAlign = DWRITE_TEXT_ALIGNMENT_CENTER;
    DWRITE_PARAGRAPH_ALIGNMENT paragAlign = DWRITE_PARAGRAPH_ALIGNMENT_CENTER;
    std::wstring font;
    FLOAT fontSize;

    IDWriteTextFormat *textFormat = nullptr;
    ID2D1Brush *textBrush = nullptr;
    ID2D1Brush *bgBrush = nullptr;
    
    void Release();
    void Validate();
    void Draw();

    ~DrawingResources() { Release(); }

  } drawing;

  void OnChanged() { drawing.timestamp = clock::from_time_t(0); }
};

// ----------------------------------------------------------------------------

class TextComponentFactory : public IComponentFactory
{
public:
  TextComponentFactory();

  Component *CreateObject(void *memory, component_factory_data& data) override;
  IAllocator *_GetAllocator() override { return &allocator; }

private:
  BucketAllocator allocator;
};

// ----------------------------------------------------------------------------
