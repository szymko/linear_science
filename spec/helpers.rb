require 'linear_ext'
Dir.entries("./lib/linear_science").grep(/.rb/).each{|f| require File.expand_path(f, "./lib/linear_science")}

module Helpers
	def add_examples_to_classifier(classifier)
		classifier.problem.add_example( 1, 1 =>  1, 3 =>  1)
		classifier.problem.add_example(-1, 1 => -1, 3 => -1)
	end
end