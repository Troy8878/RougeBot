/*********************************
 * mrbgem-random.cpp
 * Connor Hilarides
 * Created 2014/09/26
 *********************************/

#include "mruby.h"
#include "mruby/data.h"
#include "mruby/array.h"
#include "mruby/string.h"
#include "mruby/variable.h"
#include "mruby/hash.h"
#include "mruby/gems/regexp.h"

#include "Engine/Common.h"
#include "Engine/RubyWrappers.h"

#include <random>
#include <functional>
#include <typeindex>
#include <chrono>

// ----------------------------------------------------------------------------

std::random_device cryptrand;

struct random_device_t {};

struct random_func
{
  void *randomizer;
  std::type_index type;
  std::function<void()> delete_randomizer;

  template <typename Rand>
  random_func(const Rand& _rand)
    : type(typeid(Rand))
  {
    Rand *rand = new Rand(_rand);

    randomizer = rand;
    delete_randomizer = [rand]() { delete rand; };

    // Verify a random can be performed with this func
    std::uniform_int_distribution<> dist;
    (*this)(dist);
  }

  /// Creates it with a std::random_device
  random_func(random_device_t)
    : type(typeid(std::random_device))
  {
    randomizer = (void *) &cryptrand;
    delete_randomizer = [](){};
  }

  ~random_func()
  {
    delete_randomizer();
  }

  #define APPLY_FIRST_TYPE(_type) \
    if (type == typeid(std:: _type)) { \
      return dist(*((std:: _type *) randomizer)); \
    }

  #define APPLY_TYPE(_type) \
    else if (type == typeid(std:: _type)) { \
      return dist(*((std:: _type *) randomizer)); \
    }

  template <typename Distribution>
  auto operator()(Distribution& dist) -> decltype(dist(cryptrand))
  {
    APPLY_FIRST_TYPE(random_device)
    APPLY_TYPE(default_random_engine)
    APPLY_TYPE(minstd_rand0)
    APPLY_TYPE(minstd_rand)
    APPLY_TYPE(mt19937)
    APPLY_TYPE(mt19937_64)
    APPLY_TYPE(ranlux24_base)
    APPLY_TYPE(ranlux48_base)
    APPLY_TYPE(ranlux24)
    APPLY_TYPE(ranlux48)
    APPLY_TYPE(knuth_b)

    throw std::exception("An unknown random device type has been given");
  }

  #undef APPLY_TYPE
};

// ----------------------------------------------------------------------------

static mrb_data_type mrb_random_data_type;

static mrb_value mrb_random_new(mrb_state *mrb, mrb_value);
static void mrb_random_free(mrb_state *mrb, void *data);
static random_func& mrbv_get_randfunc(mrb_state *mrb, mrb_value self);

static mrb_value mrb_random_bool(mrb_state *mrb, mrb_value self);
static mrb_value mrb_random_int(mrb_state *mrb, mrb_value self);
static mrb_value mrb_random_float(mrb_state *mrb, mrb_value self);
static mrb_value mrb_random_unit_vec2(mrb_state *mrb, mrb_value self);
static mrb_value mrb_random_vec2(mrb_state *mrb, mrb_value self);
static mrb_value mrb_random_unit_vec3(mrb_state *mrb, mrb_value self);
static mrb_value mrb_random_vec3(mrb_state *mrb, mrb_value self);
static mrb_value mrb_random_int_range_inc(mrb_state *mrb, mrb_value self);
static mrb_value mrb_random_int_range(mrb_state *mrb, mrb_value self);
static mrb_value mrb_random_int_var(mrb_state *mrb, mrb_value self);
static mrb_value mrb_random_float_range(mrb_state *mrb, mrb_value self);
static mrb_value mrb_random_float_var(mrb_state *mrb, mrb_value self);

static mrb_value mrb_random_die_roll(mrb_state *mrb, mrb_value self);
static mrb_value mrb_random_probability(mrb_state *mrb, mrb_value self);
static mrb_value mrb_random_coin_flip(mrb_state *mrb, mrb_value self);
static mrb_value mrb_random_bell_curve(mrb_state *mrb, mrb_value self);

// ----------------------------------------------------------------------------

extern "C" void mrb_mruby_random_init(mrb_state *mrb)
{
  auto rclass = mrb_define_class(mrb, "Random", mrb->object_class);

  mrb_define_class_method(mrb, rclass, "new", mrb_random_new, ARGS_REQ(1) | ARGS_OPT(1));

  mrb_define_method(mrb, rclass, "bool", mrb_random_bool, ARGS_NONE());
  mrb_define_method(mrb, rclass, "int", mrb_random_int, ARGS_REQ(1));
  mrb_define_method(mrb, rclass, "float", mrb_random_float, ARGS_NONE());
  mrb_define_method(mrb, rclass, "unit_vec2", mrb_random_unit_vec2, ARGS_NONE());
  mrb_define_method(mrb, rclass, "vec2", mrb_random_vec2, ARGS_REQ(2));
  mrb_define_method(mrb, rclass, "unit_vec3", mrb_random_unit_vec3, ARGS_NONE());
  mrb_define_method(mrb, rclass, "vec3", mrb_random_vec3, ARGS_REQ(2));
  mrb_define_method(mrb, rclass, "int_range_inc", mrb_random_int_range_inc, ARGS_REQ(2));
  mrb_define_method(mrb, rclass, "int_range", mrb_random_int_range, ARGS_REQ(2));
  mrb_define_method(mrb, rclass, "int_var", mrb_random_int_var, ARGS_REQ(2));
  mrb_define_method(mrb, rclass, "float_range", mrb_random_float_range, ARGS_REQ(2));
  mrb_define_method(mrb, rclass, "float_var", mrb_random_float_var, ARGS_REQ(2));
  
  mrb_define_method(mrb, rclass, "die_roll", mrb_random_die_roll, ARGS_REQ(1));
  mrb_define_method(mrb, rclass, "probability", mrb_random_probability, ARGS_REQ(1));
  mrb_define_method(mrb, rclass, "coin_flip", mrb_random_coin_flip, ARGS_NONE());
  mrb_define_method(mrb, rclass, "bell_curve", mrb_random_bell_curve, ARGS_OPT(3));
}

// ----------------------------------------------------------------------------

static mrb_value mrb_random_new(mrb_state *mrb, mrb_value)
{
  typedef std::chrono::high_resolution_clock clock;

  std::random_device seeder;

  static mrb_sym default_type = mrb_intern_lit(mrb, "default_random_engine");

  mrb_sym type = default_type;
  mrb_int seed = seeder();
  mrb_get_args(mrb, "|ni", &type, &seed);

  random_func *func;

  #pragma region create func

  #define CREATE_WRAP_B(_type) \
    else if (type == mrb_intern_lit(mrb, #_type)) {

  #define CREATE_WRAP_E(_type) }
    
  #define CREATE_TYPE(_type) \
    CREATE_WRAP_B(_type) \
    auto r = std:: _type {}; \
    r.seed( (unsigned long) seed ); \
    func = new random_func(r); \
    CREATE_WRAP_E(_type)
    
  if (type == mrb_intern_lit(mrb, "random_device"))
  {
    func = new random_func(random_device_t());
  }

  CREATE_TYPE(default_random_engine)
  CREATE_TYPE(minstd_rand0)
  CREATE_TYPE(minstd_rand)
  CREATE_TYPE(mt19937)
  CREATE_TYPE(mt19937_64)
  CREATE_TYPE(ranlux24_base)
  CREATE_TYPE(ranlux48_base)
  CREATE_TYPE(ranlux24)
  CREATE_TYPE(ranlux48)
  CREATE_TYPE(knuth_b)

  else
  {
    mrb_raise(mrb, mrb->eException_class, "An unknown random generator type was specified");
  }

  #pragma endregion

  auto obj = mrb_data_object_alloc(mrb, mrb_class_get(mrb, "Random"), func, &mrb_random_data_type);
  return mrb_obj_value(obj);
}

// ----------------------------------------------------------------------------

static void mrb_random_free(mrb_state *mrb, void *data)
{
  delete reinterpret_cast<random_func *>(data);
}

// ----------------------------------------------------------------------------

static random_func& mrbv_get_randfunc(mrb_state *mrb, mrb_value self)
{
  void *data = mrb_data_get_ptr(mrb, self, &mrb_random_data_type);
  return *(random_func *) data;
}

// ----------------------------------------------------------------------------

static mrb_value mrb_random_bool(mrb_state *mrb, mrb_value self)
{
  auto& rand = mrbv_get_randfunc(mrb, self);

  std::bernoulli_distribution bern;
  return mrb_bool_value(rand(bern));
}

// ----------------------------------------------------------------------------

static mrb_value mrb_random_int(mrb_state *mrb, mrb_value self)
{
  auto& rand = mrbv_get_randfunc(mrb, self);

  mrb_int min = 0;
  mrb_int max;
  mrb_get_args(mrb, "i", &max);

  if (min == max)
    mrb_raise(mrb, mrb->eException_class, "max cannot be 0");

  if (max < min)
    std::swap(min, max);

  std::uniform_int_distribution<mrb_int> dist(min, max - 1);
  return mrb_fixnum_value(rand(dist));
}

// ----------------------------------------------------------------------------

static mrb_value mrb_random_float(mrb_state *mrb, mrb_value self)
{
  auto& rand = mrbv_get_randfunc(mrb, self);

  std::uniform_real_distribution<double> dist;
  return mrb_float_value(mrb, rand(dist));
}

// ----------------------------------------------------------------------------

static mrb_value mrb_random_unit_vec2(mrb_state *mrb, mrb_value self)
{
  auto& rand = mrbv_get_randfunc(mrb, self);

  std::uniform_real_distribution<float> adist(-1, 1);
  auto rotate = DirectX::XMMatrixRotationZ(rand(adist));
  math::Vector vect{1, 0, 0, 0};
  vect = rotate * vect;
  
  return ruby::create_new_vector(rotate * vect);
}

// ----------------------------------------------------------------------------

static mrb_value mrb_random_vec2(mrb_state *mrb, mrb_value self)
{
  auto& rand = mrbv_get_randfunc(mrb, self);

  mrb_float min, max;
  mrb_get_args(mrb, "ff", &min, &max);
  
  std::uniform_real_distribution<float> lendist((float) min, (float) max);
  std::uniform_real_distribution<float> adist(-1, 1);
  auto rotate = DirectX::XMMatrixRotationZ(rand(adist));
  math::Vector vect{rand(lendist), 0, 0, 0};
  vect = rotate * vect;
  
  return ruby::create_new_vector(vect);
}

// ----------------------------------------------------------------------------

static mrb_value mrb_random_unit_vec3(mrb_state *mrb, mrb_value self)
{
  auto& rand = mrbv_get_randfunc(mrb, self);

  std::uniform_real_distribution<float> adist(0, math::pi * 2);
  auto rotate = DirectX::XMMatrixRotationZ(rand(adist)) *
                DirectX::XMMatrixRotationY(rand(adist)) *
                DirectX::XMMatrixRotationX(rand(adist));
  math::Vector vect{1, 0, 0, 0};
  vect = rotate * vect;
  
  return ruby::create_new_vector(rotate * vect);
}

// ----------------------------------------------------------------------------

static mrb_value mrb_random_vec3(mrb_state *mrb, mrb_value self)
{
  auto& rand = mrbv_get_randfunc(mrb, self);

  mrb_float min, max;
  mrb_get_args(mrb, "ff", &min, &max);
  
  std::uniform_real_distribution<float> lendist((float) min, (float) max);
  std::uniform_real_distribution<float> adist(0, math::pi * 2);
  auto rotate = DirectX::XMMatrixRotationZ(rand(adist)) *
                DirectX::XMMatrixRotationY(rand(adist)) *
                DirectX::XMMatrixRotationX(rand(adist));
  math::Vector vect{1, 0, 0, 0};
  vect = rotate * vect;
  
  return ruby::create_new_vector(rotate * vect);
}

// ----------------------------------------------------------------------------

static mrb_value mrb_random_int_range_inc(mrb_state *mrb, mrb_value self)
{
  auto& rand = mrbv_get_randfunc(mrb, self);

  mrb_int min, max;
  mrb_get_args(mrb, "ii", &min, &max);

  if (max < min)
    std::swap(min, max);

  std::uniform_int_distribution<mrb_int> dist(min, max);
  return mrb_fixnum_value(rand(dist));
}

// ----------------------------------------------------------------------------

static mrb_value mrb_random_int_range(mrb_state *mrb, mrb_value self)
{
  auto& rand = mrbv_get_randfunc(mrb, self);

  mrb_int min, max;
  mrb_get_args(mrb, "ii", &min, &max);

  if (min == max)
    mrb_raise(mrb, mrb->eException_class, "max cannot be 0");

  if (max < min)
    std::swap(min, max);

  std::uniform_int_distribution<mrb_int> dist(min, max - 1);
  return mrb_fixnum_value(rand(dist));
}

// ----------------------------------------------------------------------------

static mrb_value mrb_random_int_var(mrb_state *mrb, mrb_value self)
{
  auto& rand = mrbv_get_randfunc(mrb, self);

  mrb_int base, var;
  mrb_get_args(mrb, "ii", &base, &var);

  std::uniform_int_distribution<mrb_int> dist(base - var, base + var);
  return mrb_fixnum_value(rand(dist));
}

// ----------------------------------------------------------------------------

static mrb_value mrb_random_float_range(mrb_state *mrb, mrb_value self)
{
  auto& rand = mrbv_get_randfunc(mrb, self);

  mrb_float min, max;
  mrb_get_args(mrb, "ff", &min, &max);

  std::uniform_real_distribution<mrb_float> dist(min, max);
  return mrb_float_value(mrb, rand(dist));
}

// ----------------------------------------------------------------------------

static mrb_value mrb_random_float_var(mrb_state *mrb, mrb_value self)
{
  auto& rand = mrbv_get_randfunc(mrb, self);

  mrb_float base, var;
  mrb_get_args(mrb, "ff", &base, &var);

  std::uniform_real_distribution<mrb_float> dist(base - var, base + var);
  return mrb_float_value(mrb, rand(dist));
}

// ----------------------------------------------------------------------------

static mrb_value mrb_random_die_roll(mrb_state *mrb, mrb_value self)
{
  auto& rand = mrbv_get_randfunc(mrb, self);

  mrb_int sides;
  mrb_get_args(mrb, "i", &sides);
  if (sides <= 1)
    mrb_raise(mrb, mrb->eException_class, 
              "For a die roll, the number of sides must be greater than 1");

  std::uniform_int_distribution<mrb_int> die(1, sides);
  return mrb_fixnum_value(rand(die));
}

// ----------------------------------------------------------------------------

static mrb_value mrb_random_probability(mrb_state *mrb, mrb_value self)
{
  auto& rand = mrbv_get_randfunc(mrb, self);

  mrb_float prob;
  mrb_get_args(mrb, "f", &prob);

  std::bernoulli_distribution bern(prob);
  return mrb_bool_value(rand(bern));
}

// ----------------------------------------------------------------------------

static mrb_value mrb_random_coin_flip(mrb_state *mrb, mrb_value self)
{
  auto& rand = mrbv_get_randfunc(mrb, self);

  std::bernoulli_distribution bern;
  return mrb_bool_value(rand(bern));
}

// ----------------------------------------------------------------------------

static mrb_value mrb_random_bell_curve(mrb_state *mrb, mrb_value self)
{
  auto& rand = mrbv_get_randfunc(mrb, self);
  
  mrb_float mean = 0.0;
  mrb_float std_dev = 1.0;
  mrb_get_args(mrb, "|ff", &mean, &std_dev);

  std::normal_distribution<double> bell(mean, std_dev);
  return mrb_float_value(mrb, rand(bell));
}

// ----------------------------------------------------------------------------
