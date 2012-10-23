module LinearScience
	class Parameters
		class NotKernelType < Exception
		end;

		PARAMETERS = ["c_cost", "e_epsilon", "bias", "n_fold_cv"]

		PARAMETERS.each do |param|
			define_method(param + "=") do |value|
				instance_variable_set("@" + param, value)
				raise ArgumentError, 'Vector values must be numeric.' unless value.kind_of?(Numeric)
				value
			end
			
			define_method(param) do
				instance_variable_get("@" + param)
			end
		end

		attr_reader :kernel, :weights

		def initialize
			# numeric
			@kernel = nil
			@c_cost = nil
			@e_epsilon = nil
			@bias = nil
			@n_fold_cv = nil

			# hash
			@weights = nil
		end

		def kernel=(value)
			raise NotKernelType unless (0..13).include?(Integer(value))
			@kernel = value
		end

		def weights=(value)
			raise ArgumentError unless value.kind_of?(Hash)
			value.each do |key,value|
				raise ArgumentError unless key.kind_of?(Numeric) or value.kind_of?(Numeric)
			end

			@weights = value
		end

		def set_to(hash_params)
			last_value = {}

			hash_params.each do |key,value|
				if (PARAMETERS + ["kernel", "weights"]).include?(key.to_s)
					last_value[key] = self.__send__(key.to_s + "=", value)
				end
			end

			last_value.empty? ? nil : last_value
		end

		def pack
			instance_variables.each_with_object(Hash.new) do |var,params_hash|
				params_hash.default = -1
				var_name = var.to_s.gsub("@","")
				var_value = instance_variable_get(var)

				params_hash[var_name] = var_value unless var_value.nil?
			end
		end

		def set_default
			instance_variables.each{ |var| instance_variable_set(var,nil) }
		end
	end
end