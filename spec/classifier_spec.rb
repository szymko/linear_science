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
			@classifier.parameters.weights = { 1 => 0.5 }
			@classifier.train
			@classifier.model.parameters["e_epsilon"].should == 0.1
		end
	end

	describe '#predict' do
		before :each do
			add_examples_to_classifier(@classifier)
			@classifier.train
			@classification_example = { 1 => 1, 3 => 1 }
		end

		it 'should return predicted label' do
			@classifier.predict(@classification_example).should be_a_kind_of Numeric
		end

		it 'should not raise error if example dimension exceeds problem dimension' do
			@oversized_example = { 1 => 1, 6 => 1 }
			@classifier.predict(@oversized_example).should be_kind_of Numeric 
		end

		it 'should raise error if example is ill-formed' do
			@ill_formed_example = { "sir, im a number" => "somehow i doubt" }
			expect{ @classifier.predict(@ill_formed_example) }.to raise_error
		end
	end

	describe '#predict_probability' do
		before :each do
			@classifier.parameters.kernel = 0
			add_examples_to_classifier(@classifier)
			@classifier.train
			@classification_example = { 1 => 1, 3 => 1 }
		end

		it 'should be able to return probabilities' do
			@classifier.predict_probability(@classification_example).should be_an_instance_of Hash
		end

		it 'should return hash of the length equal to number of classification classes' do
			@classifier.predict_probability(@classification_example).length.should == @classifier.model.nr_class
		end

		it 'should return hash containing values between 0 and 1' do
			@classifier.predict_probability(@classification_example).values.each do |prob|
				(0.0..1.0).should cover prob
			end
		end

		it 'should raise an error if wrong kernel is used' do
			@classifier.parameters.kernel = 1
			@classifier.train
			expect{ @classifier.predict_probability(@classification_example) }.to raise_error
		end
	end
end