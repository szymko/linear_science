require './lib/linear_science/parameters'

describe LinearScience::Parameters do
	before :each do
		@parameters = LinearScience::Parameters.new
	end

	describe '#new' do
		it 'should be an instance of LinearScience::Parameters' do
			@parameters.should be_an_instance_of LinearScience::Parameters
		end

		it 'should be able to set its variables correctly' do
			@parameters.instance_variables.each do |var|
				unless var == :@weights
					@parameters.send(var.to_s.gsub('@','') + "=",0.1)
					@parameters.instance_variable_get(var).should == 0.1
				else
					@parameters.weights = { 5 => 0.11 }
					@parameters.weights.should == { 5 => 0.11 }
				end
			end
		end
	end

	describe '#set_to' do
		it 'should set parameters according to a given hash' do
			params_hash = { :c_cost => 0.1, "kernel" => 1 }
			@parameters.set_to(params_hash)
			@parameters.c_cost.should == 0.1
			@parameters.kernel.should == 1
		end

		it 'should return nil if key is malformed' do
			malformed_name = { "i_do_not_exist" => 0.36 }
			@parameters.set_to(malformed_name).should be_nil
		end

		it 'should raise error if value does not match parameters\' requirements' do
			wrong_value = {:c_cost => "sir, I am a number fifteen"}
			expect { @parameters.set_to(wrong_value) }.to raise_error
		end
	end

	describe '#pack' do
		it 'should return a hash' do
			@parameters.pack.should be_an_instance_of Hash
		end

		it 'should return a hash set to default -1' do
			@parameters.pack["some_unexistant_key"].should == -1
		end

		it 'should return value dependent of given parameters' do
			@parameters.kernel = 1
			@parameters.pack["kernel"].should == 1
			@parameters.pack.length.should == 1
			
			@parameters.c_cost = 0.5
			@parameters.pack["c_cost"].should == 0.5
			@parameters.pack.length.should == 2
		end
	end

	describe '#set_default' do
		it 'should set all instance variables to nil' do
			@parameters.kernel = 1
			@parameters.set_default

			@parameters.instance_variables.each do |var|
				@parameters.instance_variable_get(var).should be_nil, "@parameters #{var.to_s} is not nil."
			end
		end
	end
end