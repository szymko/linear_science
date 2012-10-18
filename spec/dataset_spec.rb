require './lib/linear_science/dataset'

describe LinearScience::Dataset do
	
	before :each do
		@dataset = LinearScience::Dataset.new
	end

	before :all do
		@vector_ary = []
		5.times do |i|
			@vector_ary << {i+1 => i}
		end
		@num_vector = { 1 => 1, 2 => 2 }
	end

	describe '#new' do
		it 'should be an instance of LinearScience::Dataset' do
			@dataset.should be_an_instance_of LinearScience::Dataset
		end

		it 'should come with an empty array' do
			@dataset.examples.should be_empty
		end
	end

	describe '#add_example' do

		it 'accepts new examples' do
			@dataset.add_example(0,@num_vector)
			string_vector = { 1 => 1, "5" => 5 }
			@dataset.add_example(0,string_vector)
			@dataset.add_example("1",string_vector)
		end

		it 'stores added examples' do
			@vector_ary.each{|v| @dataset.add_example(0,v)}
			@dataset.examples.length.should  == 5
		end

		it 'does not accept faulty klass identifiers' do
			faulty_klass = "sample string"
			expect{ @dataset.add_example(faulty_klass,@num_vector) }.to raise_error
		end

		it 'throws an exception if dimensions cannot be interpreted as positive Integer' do
			faulty_num_vector = { 0 => 1, 19 => 2 }
			expect { @dataset.add_example(0,faulty_num_vector) }.to raise_error
			faulty_string_vector = { "a" => 1, 1 => 2 }
			expect { @dataset.add_example(0,faulty_string_vector) }.to raise_error
		end

		it 'throws an exception if vector values are not numerals' do
			faulty_vector = { 1 => "a", 2 => 2}
			expect { @dataset.add_example(0,faulty_vector) }.to raise_error
		end
	end

	describe '#max_dimension' do
		it 'should find maximum dimension of all examples' do
			@vector_ary.each{|v| @dataset.add_example(0,v)}
			@dataset.max_dimension.should  == 5
		end
	end
end