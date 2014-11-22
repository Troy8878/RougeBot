#########################
# EvalOnEventComponent.rb
# Connor Hilarides
# Created 2014/11/18
# Copyright © 2014 DigiPen Institute of Technology, All Rights Reserved
#########################

class EvalOnEventComponent < ComponentBase

  def initialize(data)
    super data

    data["bindings"].each do |event, code|
      add_eval_event event, code
    end
  end

  def add_eval_event(event, code)
    event_sym = event.to_sym
    method_sym = "#{event}_exec".to_sym

    proc = eval(
      "-> (singleton) {
        singleton.instance_eval do
          def #{method_sym}(e)
            #{code}
          end
        end 
      }")
    proc.call(self)

    register_event event_sym, method_sym
  end

  def remove_eval_event(event)
    remove_event event.to_sym
  end

  register_component "EvalOnEventComponent"
end
