/*********************************
 * ModelBuilder.cpp
 * Connor Hilarides
 * Created 2014/09/09
 *********************************/

#include "Common.h"
#include "ModelBuilder.h"
#include "RubyComponent.h"
#include "CustomModelComponent.h"

#include "mruby/array.h"

// ----------------------------------------------------------------------------

ModelBuilder::ModelBuilder()
  : graphics(GetGame()->GameDevice)
{
}

// ----------------------------------------------------------------------------

ModelBuilder::~ModelBuilder()
{
  if (vertices.size())
    std::cerr
      << "[WARN] A ModelBuilder was destroyed while it still "
      << "had vertices. Wastin' time here :U" << std::endl;
}

// ----------------------------------------------------------------------------

void ModelBuilder::AddTri(const TexturedVertex vertices[3])
{
  indices.push_back(SaveVertex(vertices[0]));
  indices.push_back(SaveVertex(vertices[1]));
  indices.push_back(SaveVertex(vertices[2]));
}

// ----------------------------------------------------------------------------

void ModelBuilder::AddQuad(const TexturedVertex vertices[4])
{
  UINT vindices[4] =
    {
      SaveVertex(vertices[0]),
      SaveVertex(vertices[1]),
      SaveVertex(vertices[2]),
      SaveVertex(vertices[3]),
    };

  indices.push_back(vindices[0]);
  indices.push_back(vindices[1]);
  indices.push_back(vindices[3]);
  indices.push_back(vindices[1]);
  indices.push_back(vindices[3]);
  indices.push_back(vindices[2]);
}

// ----------------------------------------------------------------------------

Model *ModelBuilder::CreateModel()
{
  auto model = new Model
    {
      graphics->Device,
      vertices.data(), static_cast<UINT>(vertices.size()),
      indices.data(), static_cast<UINT>(indices.size())
    };

  vertices.clear();
  indices.clear();

  return model;
}

// ----------------------------------------------------------------------------

UINT ModelBuilder::SaveVertex(const TexturedVertex &vertex)
{
  auto &indexSlot =
    vertexIndex
    [static_cast<int64_t>(vertex.position.x)]
    [static_cast<int64_t>(vertex.position.y)]
    [static_cast<int64_t>(vertex.position.z)];

  auto it = std::find_if(indexSlot.begin(), indexSlot.end(),
                         [&vertex](const std::pair<TexturedVertex, UINT> &pair)
                         {
                           return pair.first == vertex;
                         });

  if (it != indexSlot.end())
    return it->second;

  vertices.push_back(vertex);
  UINT index = UINT(vertices.size() - 1);

  indexSlot.push_back({vertex, index});

  return index;
}

// ----------------------------------------------------------------------------

mrb_data_type builder_data_type;
static void mrb_builder_data_free(mrb_state *mrb, void *data);

static mrb_value mrb_modelbuilder_build_model(mrb_state *mrb, mrb_value self);

static mrb_value mrb_builder_add_tri(mrb_state *mrb, mrb_value self);
static mrb_value mrb_builder_add_quad(mrb_state *mrb, mrb_value self);

// ----------------------------------------------------------------------------

void ModelBuilder::InitializeRubyModule(mrb_state *mrb)
{
  ruby::ruby_engine engine{mrb};

  auto module = engine.define_module("ModelBuilder");

  module.define_method("build_model", mrb_modelbuilder_build_model, ARGS_BLOCK());

  builder_data_type.dfree = nullptr;
  builder_data_type.struct_name = "ModelBuilder";

  auto bclass = module.define_class("Builder");
  bclass.define_method("add_tri", mrb_builder_add_tri, ARGS_REQ(3));
  bclass.define_method("add_quad", mrb_builder_add_quad, ARGS_REQ(4));

  TilemapBuilder::InitializeRubyModule(mrb);
}

// ----------------------------------------------------------------------------

static mrb_value mrb_modelbuilder_build_model(mrb_state *mrb, mrb_value self)
{
  mrb_value block;
  mrb_get_args(mrb, "&", &block);

  auto bclass = mrb_inst->get_module("ModelBuilder").get_class("Builder");

  ModelBuilder builder;
  auto param = mrb_data_object_alloc(mrb, bclass, &builder, &builder_data_type);

  mrb_yield(mrb, block, mrb_obj_value(param));

  auto entity = ruby::read_native_ptr<Component>(mrb, self)->Owner;
  auto modcomp = static_cast<CustomModelComponent *>(entity->GetComponent("CustomModelComponent"));

  if (modcomp->CustomModel)
    delete modcomp->CustomModel;

  modcomp->CustomModel = builder.CreateModel();

  return mrb_nil_value();
}

// ----------------------------------------------------------------------------

static mrb_value mrb_builder_add_tri(mrb_state *mrb, mrb_value self)
{
  ModelBuilder &builder = *static_cast<ModelBuilder *>(mrb_data_check_get_ptr(mrb, self, &builder_data_type));

  mrb_value *a_vertices;
  mrb_int c_vertices;
  mrb_get_args(mrb, "*", &a_vertices, &c_vertices);

  if (c_vertices != 3)
    mrb_raise(mrb, mrb->eException_class, "add_tri takes 3 Vertex items");

  ruby::ruby_value r_vertices[3] =
    {
      a_vertices[0], a_vertices[1], a_vertices[2]
    };

  TexturedVertex vertices[3];
  for (int i = 0; i < 3; ++i)
  {
    vertices[i].position = r_vertices[i].functions["position"].call();
    vertices[i].color = r_vertices[i].functions["color"].call();
    vertices[i].texture = r_vertices[i].functions["texture"].call();
  }

  builder.AddTri(vertices);

  return self;
}

// ----------------------------------------------------------------------------

static mrb_value mrb_builder_add_quad(mrb_state *mrb, mrb_value self)
{
  ModelBuilder &builder = *static_cast<ModelBuilder *>(mrb_data_check_get_ptr(mrb, self, &builder_data_type));

  mrb_value *a_vertices;
  mrb_int c_vertices;
  mrb_get_args(mrb, "*", &a_vertices, &c_vertices);

  if (c_vertices != 4)
    mrb_raise(mrb, mrb->eException_class, "add_tri takes 3 Vertex items");

  ruby::ruby_value r_vertices[4] =
    {
      a_vertices[0], a_vertices[1], a_vertices[2], a_vertices[3]
    };

  TexturedVertex vertices[4];
  for (int i = 0; i < 4; ++i)
  {
    vertices[i].position = r_vertices[i].functions["position"].call();
    vertices[i].color = r_vertices[i].functions["color"].call();
    vertices[i].texture = r_vertices[i].functions["texture"].call();
  }

  builder.AddQuad(vertices);

  return self;
}

// ----------------------------------------------------------------------------

TilemapBuilder::TilemapBuilder(size_t zipSize)
  : graphics(GetGame()->GameDevice), zipSize(zipSize)
{
}

// ----------------------------------------------------------------------------

TilemapBuilder::~TilemapBuilder()
{
  if (vertices.size())
    std::cerr << "[WARN] A TilemapBuilder was destroyed while it still "
      << "had vertices. Wastin' time here :U" << std::endl;
}

// ----------------------------------------------------------------------------

void TilemapBuilder::AddTile(mrb_int xo, mrb_int yo, UINT texture)
{
  UINT vertex_offset = assert_limits<UINT>(this->vertices.size());

  const UINT segments = 8;

  for (UINT y = 0; y < segments; ++y)
  {
    for (UINT x = 0; x < segments; ++x)
    {
      auto xprog = x * (1.0f / (segments - 1));
      auto yprog = y * (1.0f / (segments - 1));

      TexturedVertex vertex;
      vertex.position.x = xo + -0.5f + xprog;
      vertex.position.y = yo + 0.5f - yprog;
      vertex.texture.x = xprog;
      vertex.texture.y = (texture + yprog) / zipSize;
      vertex.color = math::Vector{1,1,1,1};

      vertices.push_back(vertex);
    }
  }

  for (UINT y = 0; y < segments - 1; ++y)
  {
    for (UINT x = 0; x < segments - 1; ++x)
    {
      UINT vtl = (y) * segments + (x);
      UINT vtr = (y) * segments + (x + 1);
      UINT vbl = (y + 1) * segments + (x);
      UINT vbr = (y + 1) * segments + (x + 1);

      indices.push_back(vertex_offset + vtl);
      indices.push_back(vertex_offset + vbl);
      indices.push_back(vertex_offset + vbr);
      indices.push_back(vertex_offset + vtl);
      indices.push_back(vertex_offset + vbr);
      indices.push_back(vertex_offset + vtr);
    }
  }
}

// ----------------------------------------------------------------------------

Model *TilemapBuilder::CreateModel()
{
  auto model = new Model
    {
      graphics->Device,
      vertices.data(), static_cast<UINT>(vertices.size()),
      indices.data(), static_cast<UINT>(indices.size())
    };

  vertices.clear();
  indices.clear();

  return model;
}

// ----------------------------------------------------------------------------

mrb_data_type tilemap_dt;

// ----------------------------------------------------------------------------

static mrb_value mrb_tilemap_build_model(mrb_state *mrb, mrb_value self)
{
  mrb_int zipSize;
  mrb_value block;
  mrb_get_args(mrb, "i&", &zipSize, &block);

  auto bclass = mrb_class_get_under(mrb, mrb_module_get(mrb, "TilemapBuilder"), "Builder");

  TilemapBuilder builder(static_cast<size_t>(zipSize));
  auto param = mrb_data_object_alloc(mrb, bclass, &builder, &tilemap_dt);

  mrb_yield(mrb, block, mrb_obj_value(param));

  auto entity = ruby::data_get<Entity>(mrb, mrb_funcall_argv(mrb, self, mrb_intern_lit(mrb, "owner"), 0, nullptr));
  auto modcomp = static_cast<CustomModelComponent *>(entity->GetComponent("CustomModelComponent"));

  if (modcomp->CustomModel)
    delete modcomp->CustomModel;

  modcomp->CustomModel = builder.CreateModel();

  return mrb_nil_value();
}

// ----------------------------------------------------------------------------

static mrb_value mrb_tilemap_add_tile(mrb_state *mrb, mrb_value self)
{
  mrb_int xo, yo, tex;
  mrb_get_args(mrb, "iii", &xo, &yo, &tex);

  auto *tilemap = ruby::data_get<TilemapBuilder>(mrb, self);
  tilemap->AddTile(xo, yo, assert_limits_mrb<UINT>(mrb, tex));

  return mrb_nil_value();
}

// ----------------------------------------------------------------------------

void TilemapBuilder::InitializeRubyModule(mrb_state *mrb)
{
  auto mod = mrb_define_module(mrb, "TilemapBuilder");
  auto bclass = mrb_define_class_under(mrb, mod, "Builder", mrb->object_class);

  mrb_define_method(mrb, mod, "build_model", mrb_tilemap_build_model, ARGS_REQ(1) | ARGS_BLOCK());

  tilemap_dt.dfree = ruby::data_nop_delete;
  tilemap_dt.struct_name = typeid(TilemapBuilder).name();

  mrb_define_method(mrb, bclass, "add_tile", mrb_tilemap_add_tile, MRB_ARGS_REQ(3));
}

// ----------------------------------------------------------------------------