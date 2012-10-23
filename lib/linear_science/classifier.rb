module LinearScience
	class Classifier

		attr_accessor :problem, :parameters, :model

		def initialize
			@problem = Problem.new
			@parameters = Parameters.new
			@model = Model.new
		end
	end
end