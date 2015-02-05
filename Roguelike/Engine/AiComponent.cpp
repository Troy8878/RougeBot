/*********************************
 * AiComponent.cpp
 * YOUR NAME HERE
 * Created 2015/YOUR DATE HERE
 * Copyright © 2014 DigiPen Institute of Technology, All Rights Reserved
 *********************************/

#include "Common.h"
#include "AiComponent.h"

// ----------------------------------------------------------------------------

AiComponentFactory AiComponent::factory;

// ----------------------------------------------------------------------------

AiComponent::AiComponent()
{
}

// ----------------------------------------------------------------------------

void AiComponent::Initialize(Entity *owner, const std::string &name)
{
  Component::Initialize(owner, name);

  DEF_EVENT_ID(update);
  Owner->AddEvent(this, update, &AiComponent::OnUpdate);
}

// ----------------------------------------------------------------------------

void AiComponent::OnUpdate(Events::EventMessage &)
{
  while (!decisions.empty())
  {
    auto dec = decisions.front();
    auto res = dec->GetResult();
    
    if (res.is_set())
    {
      decisions.pop();
      auto airesult = res.get();

      mrb_state *mrb = *mrb_inst;
      ruby::ruby_gc_guard gcg{mrb};

      mrb_value result;
      switch (airesult.action)
      {
        case AIResult::Move:
          result = mrb_str_new_lit(mrb, "move");
          break;

        case AIResult::Attack:
          result = mrb_str_new_lit(mrb, "attack");
          break;

        case AIResult::Custom:
          result = mrb_str_new(mrb, airesult.custom.c_str(), airesult.custom.size());
          break;

        default:
          result = mrb_nil_value();
      }

      mrb_value hsh = mrb_hash_new(mrb);
      mrb_hash_set(mrb, hsh, mrb_str_new_lit(mrb, "result"), result);
      mrb_hash_set(mrb, hsh, mrb_str_new_lit(mrb, "x"), mrb_fixnum_value(airesult.x));
      mrb_hash_set(mrb, hsh, mrb_str_new_lit(mrb, "y"), mrb_fixnum_value(airesult.y));

      DEF_EVENT_ID(ai_complete);
      Events::RubyEvent data(hsh);
      Events::EventMessage message(ai_complete, &data);

      Owner->LocalEvent(message);
    }
    else
    {
      break;
    }
  }
}

// ----------------------------------------------------------------------------

AiComponentFactory::AiComponentFactory()
  : allocator(sizeof(AiComponent))
{
}

// ----------------------------------------------------------------------------

Component *AiComponentFactory::CreateObject(void *memory, component_factory_data &)
{
  auto *component = new(memory) AiComponent;
  return component;
}

// ----------------------------------------------------------------------------

static mrb_data_type mrb_aic_dt;
static void mrb_aic_init(mrb_state *mrb);

static mrb_value mrb_aic_enqueue(mrb_state *mrb, mrb_value self);

// ----------------------------------------------------------------------------

mrb_value AiComponent::GetRubyWrapper()
{
  static mrb_state *mrb = *mrb_inst;
  RUN_ONCE(mrb_aic_init(mrb));
  static auto klass = mrb_class_get(mrb, "AiComponent");

  auto data = mrb_data_object_alloc(mrb, klass, this, &mrb_aic_dt);
  return mrb_obj_value(data);
}

// ----------------------------------------------------------------------------

static void mrb_aic_init(mrb_state *mrb)
{
  ruby::data_type_init<AiComponent>(mrb_aic_dt);

  auto cbase = mrb_class_get(mrb, "ComponentBase");
  auto klass = mrb_define_class(mrb, "AiComponent", cbase);

  mrb_define_class_method(mrb, klass, "new", mrb_nop, MRB_ARGS_NONE());
  mrb_define_method(mrb, klass, "enqueue", mrb_aic_enqueue, MRB_ARGS_REQ(4));
}

// ----------------------------------------------------------------------------

static mrb_value mrb_aic_enqueue(mrb_state *mrb, mrb_value self)
{
  auto comp = ruby::data_get<AiComponent>(mrb, self);

  mrb_value behavior, owner, target, params = mrb_nil_value();
  mrb_get_args(mrb, "Soo|H", &behavior, &owner, &target, &params);
  
  auto etarget = ruby::data_get<Entity>(mrb, target);
  auto enowner = ruby::data_get<Entity>(mrb, owner);
  auto factory = AIFactory::GetFactory(mrb_str_to_stdstring(behavior));

  if (!factory)
    mrb_raise(mrb, E_RUNTIME_ERROR, "Unknown AI behavior");
    
  auto jparams = mrb_inst->value_to_json(params);

  auto dec = GetGame()->AI->QueueBehavior(*factory, jparams, enowner, etarget);
  comp->AddDecision(dec);

  return mrb_nil_value();
}

// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
