#########################
# Http.rb
# Connor Hilarides
# Created 2014/10/17
#########################

module Http
  class Headers
    def []=(key, value)
      value = [value] if value.is_a? String
      raise TypeError, "value must be String or Array" unless value.is_a? Array

      set = self[key]
      set.clear
      value.each {|v| set.add v }
    end

    def inspect
      hsh = {}
      self.each do |set|
        hsh[set.key] = set.entries
      end
      hsh.inspect
    end

    class Set
      def inspect
        self.entries.inspect
      end

      def [](index)
        to_a[index]
      end

      alias push add
      alias delete remove
    end
  end
end
