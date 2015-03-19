class SerializedInput
  attr_reader :data

  def initialize
    @data = {
      dependencies: [],
      properties: [],
    }
  end

  def dependency(dep)
    @data[:dependencies] << dep
  end

  # Data Types

  def bool(prop, opts = {})
    opts[:default] ||= false
    add_prop prop, (with_opts opts, {
      type: :bool
    })
  end

  def string(prop, opts = {})
    opts[:default] ||= ""
    add_prop prop, (with_opts opts, {
      type: :string
    })
  end

  def float(prop, opts = {})
    opts[:default] ||= 0.0
    add_prop prop, (with_opts opts, {
      type: :float
    })
  end

  def integer(prop, opts = {})
    opts[:default] ||= 0
    add_prop prop, (with_opts opts, {
      type: :integer
    })
  end

  def enum(prop, opts = {})
    opts[:default] ||= opts[:options][0]
    add_prop prop, (with_opts opts, {
      type: :enum,
      options: opts[:options]
    })
  end

  # Compound types

  def vector(prop, opts = {})
    opts[:default] ||= [0,0,0,0]
    add_prop prop, (with_opts opts, {
      type: :vector,
      dimensions: opts[:dimms] || 4
    })
  end

  def array(prop, opts = {})
    opts[:default] ||= []
    add_prop prop, (with_opts opts, {
      type: :array,
      contained: opts[:contained] || "string"
    })
  end

  def map(prop, opts = {})
    opts[:default] ||= {}
    add_prop prop, (with_opts opts, {
      type: :map,
      key: opts[:key] || "string",
      value: opts[:value] || "string"
    })
  end

  def entity(prop, opts = {})
    opts[:default] ||= {}
    add_prop prop, (with_opts opts, {
      type: :entity
    })
  end

  def direction(prop, opts = {})
    opts[:default] ||= [1,0]
    add_prop prop, (with_opts opts, {
      type: :direction
    })
  end

  def raw(prop, opts = {})
    opts[:default] ||= nil
    add_prop prop, (with_opts opts, {
      type: :raw
    })
  end

  private
  def with_opts(opts, hsh)
    hsh[:required] = opts[:required] || false
    hsh[:semantics] = opts[:semantics]
    hsh[:locked] = opts[:locked] || false
    hsh[:default] = opts[:default]
    hsh
  end

  def add_prop(prop, usg)
    @data[:properties] << {
      property: prop,
      usage: usg,
    }
  end
end

class ComponentBase
  class <<self
    attr_reader :serialization_input, :component_list
  end

  def self.serialized_input(&block)
    input = SerializedInput.new
    block.call(input)
    @serialization_input = input
  end

  @component_list = []

  def self.inherited(sub_class)
    @component_list << sub_class unless @component_list.include? sub_class
  end
end

def dump_serialization_maps
  h = ComponentBase.component_list.inject([]) do |h, c|
    input = c.serialization_input
    if input
      data = input.data.dup
      data[:name] = c.to_s
      h << data
    end
    h
  end

  begin
    # TODO: JSON API
    f = File.new
    f.open "serialize.json", :out, :truncate
    f.write JSON.stringify(h)
    f.close
  rescue Exception => ex
    puts ex.inspect
    return h
  end
end

def dsmap
  dump_serialization_maps
end

