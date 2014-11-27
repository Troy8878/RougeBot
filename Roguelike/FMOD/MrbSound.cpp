/*********************************
* fmod.cpp
* Troy
* Created 2014/11/01
* Copyright © 2014 DigiPen Institute of Technology, All Rights Reserved
*********************************/

#include <Engine/Common.h>
#include <mruby/class.h>

#include "SoundSystem.h"
#include "SoundManager.h"

// ----------------------------------------------------------------------------

static std::list<ManagedSound> allSounds;
std::stack<double> gvolumeStack = std::stack<double>(std::deque<double>{1.0});

struct SoundRef
{
  explicit SoundRef(const ManagedSound &sound)
    : sound(sound)
  {
    allSounds.push_front(sound);
    iterator = allSounds.begin();
  }

  NO_ASSIGNMENT_OPERATOR(SoundRef);
  NO_COPY_CONSTRUCTOR(SoundRef);

  ManagedSound sound;

  ~SoundRef()
  {
    allSounds.erase(iterator);
  }

private:
  decltype(allSounds)::iterator iterator;
};

// ----------------------------------------------------------------------------

mrb_data_type mrb_sound_dt;

static mrb_value mrb_sound_alloc(mrb_state *mrb, ManagedSound sound, RClass *klass);
static mrb_value mrb_sound_new(mrb_state *mrb, mrb_value klass);

static mrb_value mrb_sound_play(mrb_state *mrb, mrb_value self);
static mrb_value mrb_sound_stop(mrb_state *mrb, mrb_value self);

static mrb_value mrb_sound_gvolume_push(mrb_state *mrb, mrb_value klass);
static mrb_value mrb_sound_gvolume_pop(mrb_state *mrb, mrb_value klass);

// ----------------------------------------------------------------------------

EXTERN_C void mrb_mruby_sound_init(mrb_state *mrb)
{
  ruby::data_type_init<SoundRef>(mrb_sound_dt, ruby::data_scalar_delete<SoundRef>);

  auto klass = mrb_define_class(mrb, "Sound", mrb->object_class);

  mrb_define_class_method(mrb, klass, "new", mrb_sound_new, MRB_ARGS_REQ(1));
  mrb_define_class_method(mrb, klass, "load", mrb_sound_new, MRB_ARGS_REQ(1));

  mrb_define_method(mrb, klass, "play", mrb_sound_play, MRB_ARGS_NONE());
  mrb_define_method(mrb, klass, "stop", mrb_sound_stop, MRB_ARGS_NONE());
}

// ----------------------------------------------------------------------------

static mrb_value mrb_sound_alloc(mrb_state *mrb, ManagedSound sound, RClass *klass)
{
  auto data = mrb_data_object_alloc(mrb, klass, new SoundRef(sound), &mrb_sound_dt);
  auto obj = mrb_obj_value(data);
  return obj;
}

// ----------------------------------------------------------------------------

static mrb_value mrb_sound_new(mrb_state *mrb, mrb_value klass)
{
  mrb_value data;
  mrb_get_args(mrb, "o", &data);

  auto audioDef = mrb_inst->value_to_json(data);
  auto audio = SoundManager::Instance.Load(audioDef);

  return mrb_sound_alloc(mrb, audio, mrb_class_ptr(klass));
}

// ----------------------------------------------------------------------------

static mrb_value mrb_sound_play(mrb_state *mrb, mrb_value self)
{
  auto ref = ruby::data_get<SoundRef>(mrb, self);
  auto &sound = ref->sound;

  sound->Play();

  return mrb_nil_value();
}

// ----------------------------------------------------------------------------

static mrb_value mrb_sound_stop(mrb_state *mrb, mrb_value self)
{
  auto ref = ruby::data_get<SoundRef>(mrb, self);
  auto &sound = ref->sound;

  sound->Stop();

  return mrb_nil_value();
}

// ----------------------------------------------------------------------------

