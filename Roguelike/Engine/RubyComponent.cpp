/*********************************
 * RubyComponent.cpp
 * Connor Hilarides
 * Created 2014/08/28
 *********************************/

#include "Common.h"
#include "RubyComponent.h"

// ----------------------------------------------------------------------------

RubyComponent::RubyComponent(ruby::ruby_class rclass, component_factory_data& data)
{
  auto& mrb = *ruby::ruby_engine::global_engine;

  component_inst = mrb.hash_from_map(data).silent_reset();

  static auto rclass_sym = 
    mrb_symbol_value(mrb_intern_cstr(mrb, "new_inst_rclass"));
  auto rclass_v = mrb_obj_value(rclass.mrb_handle());

  mrb_hash_set(mrb, component_inst, rclass_sym, rclass_v);
}

// ----------------------------------------------------------------------------

void RubyComponent::Initialize(Entity *owner, const std::string& name)
{
  Component::Initialize(owner, name);

  auto& mrb = *ruby::ruby_engine::global_engine;

  static mrb_value 
    entity_ptr_key = mrb_symbol_value(mrb_intern_cstr(mrb, "entity_ptr_v")),
    comp_ptr_key = mrb_symbol_value(mrb_intern_cstr(mrb, "comp_ptr_v"));

  mrb_value
    entity_ptr_v = mrb.wrap_native_ptr(owner),
    comp_ptr_v = mrb.wrap_native_ptr(static_cast<Component *>(this));

  auto map = component_inst;

  mrb_hash_set(mrb, map, entity_ptr_key, entity_ptr_v);
  mrb_hash_set(mrb, map, comp_ptr_key, comp_ptr_v);

  static auto rclass_sym = 
    mrb_symbol_value(mrb_intern_cstr(mrb, "new_inst_rclass"));
  auto rclass_p = (RClass *) mrb_object(mrb_hash_get(mrb, map, rclass_sym));
  ruby::ruby_class rclass{&mrb, rclass_p};

  component_inst = rclass.new_inst(map).silent_reset();
  mrb.log_and_clear_error();
}

// ----------------------------------------------------------------------------

RubyComponentFactory::RubyComponentFactory(ruby::ruby_class rclass)
  : allocator(sizeof(RubyComponent)), rclass(rclass)
{
}

// ----------------------------------------------------------------------------

Component *RubyComponentFactory::CreateObject(void *memory, 
                                              component_factory_data& data)
{
  return new (memory) RubyComponent(rclass, data);
}

// ----------------------------------------------------------------------------
