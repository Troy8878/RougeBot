class SerializedInput
  attr_reader :data

  def initialize
    @data = {
      dependencies: [],
      properties: {},
    }
  end

  def dependency(dep)
    @data[:dependencies] << dep
  end

  # Data Types

  def bool(prop, opts = {})
    @data[:properties][prop] = with_opts opts, {
      type: :bool
    }
  end

  def string(prop, opts = {})
    @data[:properties][prop] = with_opts opts, {
      type: :string
    }
  end

  def float(prop, opts = {})
    @data[:properties][prop] = with_opts opts, {
      type: :float
    }
  end

  def integer(prop, opts = {})
    @data[:properties][prop] = with_opts opts, {
      type: :integer
    }
  end

  def enum(prop, opts = {})
    @data[:properties][prop] = with_opts opts, {
      type: :enum,
      options: opts[:options]
    }
  end

  # Compound types

  def float_pair(prop, opts = {})
    @data[:properties][prop] = with_opts opts, {
      type: :float_pair
    }
  end

  def vector(prop, opts = {})
    @data[:properties][prop] = with_opts opts, {
      type: :vector,
      dimmensions: opts[:dimms] || 4
    }
  end

  def array(prop, opts = {})
    @data[:properties][prop] = with_opts opts, {
      type: :array,
      contained: opts[:contained] || "string"
    }
  end

  private
  def with_opts(opts, hsh)
    hsh[:required] = opts[:required] || false
    hsh[:semantics] = opts[:semantics]
    hsh[:locked] = opts[:locked] || false
    hsh[:default] = opts[:default]
    hsh
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
