require './lib/linear_science/classifier'

describe LinearScience::Classifier do

	before :each do
		@classifier = LinearScience::Classifier.new
	end

	describe '#new' do
		it 'should be an instance of LinearScience::Classifier' do
			@classifier.should be_an_instance_of LinearScience::Classifier
		end
	end
end