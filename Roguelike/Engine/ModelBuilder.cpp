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
    std::cerr << "[WARN] A ModelBuilder was destroyed while it still "
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
  indices.push_back(vindices[2]);
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
    vertices.data(), (UINT) vertices.size(), 
    indices.data(), (UINT) indices.size()
  };

  vertices.clear();
  indices.clear();
  
  return model;
}

// ----------------------------------------------------------------------------

UINT ModelBuilder::SaveVertex(const TexturedVertex& vertex)
{
  auto it = std::find(vertices.begin(), vertices.end(), vertex);
  if (it != vertices.end())
    return UINT(it - vertices.begin());

  vertices.push_back(vertex);
  return UINT(vertices.size() - 1);
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
  auto modcomp = (CustomModelComponent *) entity->GetComponent("CustomModelComponent");

  if (modcomp->CustomModel)
    delete modcomp->CustomModel;

  modcomp->CustomModel = builder.CreateModel();

  return mrb_nil_value();
}

// ----------------------------------------------------------------------------

static mrb_value mrb_builder_add_tri(mrb_state *mrb, mrb_value self)
{
  ModelBuilder& builder = *(ModelBuilder *) 
    mrb_data_check_get_ptr(mrb, self, &builder_data_type);

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
  ModelBuilder& builder = *(ModelBuilder *) 
    mrb_data_check_get_ptr(mrb, self, &builder_data_type);

  mrb_value *a_vertices;
  mrb_int c_vertices;
  mrb_get_args(mrb, "*", &a_vertices, &c_vertices);

  if (c_vertices != 4)
    mrb_raise(mrb, mrb->eException_class, "add_tri takes 3 Vertex items");

  ruby::ruby_value r_vertices[4] =
  {
    a_vertices[0], a_vertices[1], a_vertices[2], a_vertices[4]
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
