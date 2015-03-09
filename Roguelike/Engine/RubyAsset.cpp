/*********************************
 * RubyAsset.cpp
 * Enrique Rodriguez
 * Created 2014/08/19
 * Copyright © 2014 DigiPen Institute of Technology, All Rights Reserved
 *********************************/

#include "Common.h"
#include "mruby/class.h"

// ----------------------------------------------------------------------------

struct MrbAsset
{
  MrbAsset(const std::string &container, const std::string &asset);
  ~MrbAsset();

  Respack::ResourceContainer *container;
  Respack::Resource *resource;
};

// ----------------------------------------------------------------------------

mrb_data_type mrb_asset_dt;

static mrb_value mrb_asset_alloc(mrb_state *mrb, RClass *klass, MrbAsset *asset);
static mrb_value mrb_asset_new(mrb_state *mrb, mrb_value klass);

static mrb_value mrb_asset_read_text(mrb_state *mrb, mrb_value self);
static mrb_value mrb_asset_read_json(mrb_state *mrb, mrb_value self);

// ----------------------------------------------------------------------------

EXTERN_C void mrb_mruby_asset_init(mrb_state *mrb)
{
  ruby::data_type_init<MrbAsset>(mrb_asset_dt, ruby::data_scalar_delete<MrbAsset>);

  auto *klass = mrb_define_class(mrb, "Asset", mrb->object_class);
  mrb_define_class_method(mrb, klass, "new", mrb_asset_new, MRB_ARGS_REQ(2));
  mrb_define_class_method(mrb, klass, "load", mrb_asset_new, MRB_ARGS_REQ(2));

  mrb_define_method(mrb, klass, "read_text", mrb_asset_read_text, MRB_ARGS_NONE());
  mrb_define_method(mrb, klass, "read_json", mrb_asset_read_json, MRB_ARGS_NONE());
}

// ----------------------------------------------------------------------------

static mrb_value mrb_asset_alloc(mrb_state *mrb, RClass *klass, MrbAsset *asset)
{
  return mrb_obj_value(mrb_data_object_alloc(mrb, klass, asset, &mrb_asset_dt));
}

// ----------------------------------------------------------------------------

static mrb_value mrb_asset_new(mrb_state *mrb, mrb_value klass)
{
  mrb_value container;
  mrb_value asset;
  mrb_get_args(mrb, "SS", &container, &asset);

  auto *loadedAsset = new MrbAsset(mrb_str_to_stdstring(container), mrb_str_to_stdstring(asset));
  return mrb_asset_alloc(mrb, mrb_class_ptr(klass), loadedAsset);
}

// ----------------------------------------------------------------------------

static mrb_value mrb_asset_read_text(mrb_state *mrb, mrb_value self)
{
  auto asset = ruby::data_get<MrbAsset>(mrb, self);

  if (!asset->resource)
    return mrb_nil_value();

  auto data = asset->resource->GetData();
  auto size = asset->resource->GetSize();

  return mrb_str_new(mrb, reinterpret_cast<const char *>(data), size);
}

// ----------------------------------------------------------------------------

static mrb_value mrb_asset_read_json(mrb_state *mrb, mrb_value self)
{
  auto asset = ruby::data_get<MrbAsset>(mrb, self);

  if (!asset->resource)
    return mrb_nil_value();

  auto json = json::value::parse(asset->resource->Stream);
  return mrb_inst->json_to_value(json);
}

// ----------------------------------------------------------------------------

MrbAsset::MrbAsset(const std::string& containerName, const std::string& assetName)
  : container(nullptr), resource(nullptr)
{
  container = GetGame()->Respack[containerName];

  if (container)
    resource = container->GetResource(assetName);
}

// ----------------------------------------------------------------------------

MrbAsset::~MrbAsset()
{
  ReleaseDXInterface(resource);
  ReleaseDXInterface(container);
}

// ----------------------------------------------------------------------------
