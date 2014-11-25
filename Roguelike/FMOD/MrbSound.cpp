/*********************************
* fmod.cpp
* Troy
* Created 2014/11/01
* Copyright © 2014 DigiPen Institute of Technology, All Rights Reserved
*********************************/

#include <Engine/Common.h>
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

  ManagedSound sound;
  decltype(allSounds)::iterator iterator;

  ~SoundRef()
  {
    allSounds.erase(iterator);
  }
};

// ----------------------------------------------------------------------------

mrb_data_type mrb_sound_dt;

mrb_value mrb_sound_alloc(mrb_state *mrb, ManagedSound sound);
mrb_value mrb_sound_new(mrb_state *mrb, mrb_value klass);

mrb_value mrb_sound_gvolume_push(mrb_state *mrb, mrb_value klass);
mrb_value mrb_sound_gvolume_pop(mrb_state *mrb, mrb_value klass);

// ----------------------------------------------------------------------------

EXTERN_C void mrb_mruby_sound_init(mrb_state *mrb)
{
  (mrb);
}

// ----------------------------------------------------------------------------
