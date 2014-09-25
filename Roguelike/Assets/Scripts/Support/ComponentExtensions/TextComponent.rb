#########################
# TextComponent.rb
# Connor Hilarides
# Created 2014/09/24
#########################

module Components
  class TextComponent
    class TextArray
      def initialize(text)
        @text = text
      end

      def [](index)
        @text.get_text_at index
      end

      def []=(index, value)
        @text.set_text_at index, value
      end

      def each(&block)
        @text.to_a.each(&block)
      end
    end
  end
end
