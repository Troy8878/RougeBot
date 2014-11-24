#########################
# TextComponent.rb
# Connor Hilarides
# Created 2014/09/24
# Copyright © 2014 DigiPen Institute of Technology, All Rights Reserved
#########################

module Components
  class TextComponent < ComponentBase
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

      begin # Enumerable stuff
        include Enumerable

        def to_a
          @text.to_a
        end

        def each(&block)
          to_a.each(&block)
        end

        def map!(&block)
          raise "map! only works with a block" unless block_given?
          @text.texts = map(&block)
        end

        def select!(&block)
          raise "select! only works with a block" unless block_given?
          @text.texts = select(&block)
        end

        def reject!(&block)
          raise "reject! only works with a block" unless block_given?
          @text.texts = reject(&block)
        end
      end
    end

    def texts
      TextArray.new self
    end
  end
end
