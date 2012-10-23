require './spec/helpers'

describe LinearScience::Classifier do
	include Helpers

	before :each do
		@classifier = LinearScience::Classifier.new
	end

	describe '#new' do
		it 'should be an instance of LinearScience::Classifier' do
			@classifier.should be_an_instance_of LinearScience::Classifier
		end
	end

	describe '#train' do
		it 'should raise an error if there are no training examples' do
			@classifier.problem.delete_examples
			expect{ @classifier.train }.to raise_error
		end

		it 'should return LinearScience::Model after training' do
			add_examples_to_classifier(@classifier)
			@classifier.train.should be_an_instance_of LinearScience::Model
		end

		it 'should be able to receive custom parameters and train' do
			add_examples_to_classifier(@classifier)
			@classifier.parameters.e_epsilon = 0.1
			@classifier.parameters.weights = {1 => 0.5}
			@classifier.train.should be_an_instance_of LinearScience::Model
		end
	end
end