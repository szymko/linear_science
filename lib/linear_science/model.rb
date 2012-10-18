module LinearScience
	class Model

		def initialize
			@parameters = Parameters.new
			@nr_class = nil
			@nr_feature = nil
			@weights = []
			@labels = []
			@bias = nil
		end
	end
end