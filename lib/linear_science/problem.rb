module LinearScience
	class Problem

		class DimensionException < Exception
		end

		attr_reader :examples

		def initialize
			@examples = []
		end

		def add_example(klass, vector)
			raise ArgumentError, 'Vector must be build in the form of {dimension => value} (respond to #each_pair).' unless vector.respond_to?(:each_pair)

			check_vector(vector)
			num_klass = Integer(klass)
			@examples << [num_klass, vector]
		end

		def delete_examples(example_no = -1)
			if example_no < 0 or example_no > @examples.length - 1
				@examples = []
			else
				@examples = @examples[0 .. @examples.length - example_no - 1]
			end
		end

		def max_dimension
			@examples.inject(0) do |max_dim,example|
				current_max = example[1].keys.sort.last
				max_dim = (max_dim >= current_max ? max_dim : current_max)
			end
		end

		private

		def check_vector(vector)
			vector.each_pair do |dim,val|
				raise DimensionException, 'Dimension must be a natural number greater than one.' unless Integer(dim) > 0
				raise TypeError, 'Vector values must be numeric.' unless val.kind_of?(Numeric)
			end
		end
	end
end