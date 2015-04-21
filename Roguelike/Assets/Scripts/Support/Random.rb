#######################
# Random.rb
# Connor Hilarides
# Created 2014/09/26
# Copyright © 2015 DigiPen Institute of Technology, All Rights Reserved
#######################

##
# Types of random:
#   - :default_random_engine (default)
#   - :random_device
#   - :minstd_rand0
#   - :minstd_rand
#   - :mt19937
#   - :mt19937_64
#   - :ranlux24_base
#   - :ranlux48_base
#   - :ranlux24
#   - :ranlux48
#   - :knuth_b
class Random
  @@instance = Random.new :random_device

  private
  def self.alias_instance_func(method)
    sclass = class << self; self; end
    sclass.define_method(method) do |*args|
      @@instance.send(method, *args)
    end
  end

  alias_instance_func :bool # bool() true or false
  alias_instance_func :int # int(max) [0, max)
  alias_instance_func :float # float() [0, 1]
  alias_instance_func :unit_vec2 # unit_vec2() randomized normalized vec2
  alias_instance_func :vec2 # vec2(min_len, max_len) randomized vec2
  alias_instance_func :unit_vec3 # randomized normalized vec3
  alias_instance_func :vec3 # vec3(min_len, max_len) randomized vec3
  alias_instance_func :int_range_inc # int_range_inc(min, max) int in [min, max]
  alias_instance_func :int_range # int_range(min, max) int in [min, max)
  alias_instance_func :int_var # int_var(base, var) [base-var, base+var]
  alias_instance_func :float_range # float_range(min, max) [min, max]
  alias_instance_func :float_var # float_var(base, var) [base-var, base+var]

  alias_instance_func :die_roll # die_roll(sides) [1, sides]
  alias_instance_func :probability # probability(prob) true or false
  alias_instance_func :coin_flip # coin_flip() true if heads
  alias_instance_func :bell_curve # bell_curve(mean = 0.0, std_dev = 1.0)

end
