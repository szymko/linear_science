#include "liblinear-1.91/linear.h"
#include "ruby.h"

#define MALLOC(type,n) (type *)malloc((n)*sizeof(type))

VALUE cClassifier;
VALUE mLinearScience;
static int id_puts;

static VALUE rb_puts 
(VALUE obj) {
	return rb_funcall(rb_cObject, rb_intern("puts"), 1,  obj);
}

static VALUE 
each_block(VALUE yield_value, VALUE data2, int argc, VALUE argv[]) {
	rb_puts(yield_value);
	return Qtrue;	
}

static VALUE
rb_block_call_test(VALUE self, VALUE obj) {
	rb_puts(INT2NUM(RHASH_SIZE(obj)));
	rb_block_call(obj,
								rb_intern("each_key"),
								0,
								NULL,
								each_block,
								Qnil
								);
	return Qnil;
}

static struct parameter * alloc_parameter
(struct parameter * param) {
	param = MALLOC(struct parameter, 1);
	return param;
}

static void free_parameters
(struct parameter * param) {
	free(param->weight_label);
	free(param->weight);
}

static void set_param_weights
(struct parameter * param, VALUE rb_weights) {
	VALUE rb_weight_labels = rb_funcall(rb_weights, rb_intern("keys"), 0);
	VALUE rb_weight_values = rb_funcall(rb_weights, rb_intern("vaules"), 0);
	long i;

	param->nr_weight = RHASH_SIZE(rb_weights);
	param->weight_label = MALLOC(int, param->nr_weight);
	param->weight = MALLOC(double, param->nr_weight);

	if(param->weight_label == NULL || param->weight == NULL){
		return;
	}

	for(i = 0; i < param->nr_weight; i++) {
		param->weight_label[i] = NUM2INT(rb_ary_entry(rb_weight_labels, i));
		param->weight[i] = NUM2DBL(rb_ary_entry(rb_weight_values, i));
	}
}

static struct problem * alloc_problem
(struct problem * c_problem) {
	c_problem = MALLOC(struct problem, 1);
	return c_problem;
}

static void free_problem
(struct problem * c_problem) {
	int i;

	free(c_problem->y);
	for(i = 0; i < c_problem->l; i++) {
		free(c_problem->x[i]);
	}
	free(c_problem->x);
}

static void read_rb_examples(struct problem * c_problem, VALUE rb_example_ary, double c_bias) {
	int i, j;
	int current_length;
	int len_with_bias;
	VALUE rb_example_labels;
	VALUE rb_example_values;

	for(i = 0; i < c_problem->l; i++) {
		current_length = RHASH_SIZE(rb_ary_entry(rb_example_ary, 1));
		len_with_bias = c_bias > 0.0 ? ++current_length : current_length;
		rb_example_labels = rb_funcall(rb_ary_entry(rb_example_ary, 1), rb_intern("keys"), 0);
		rb_example_values = rb_funcall(rb_ary_entry(rb_example_ary, 1), rb_intern("values"), 0);

		c_problem->x[i] = MALLOC(struct feature_node, len_with_bias);
		c_problem->y[i] = NUM2DBL(rb_ary_entry(rb_example_ary, 0));

		for(j = 0; j < current_length; j++) {
			c_problem->x[i][j].index = NUM2INT(rb_ary_entry(rb_example_labels, j));
			c_problem->x[i][j].value = NUM2DBL(rb_ary_entry(rb_example_values,j));
		}

		if(c_bias > 0) {
			c_problem->x[i][len_with_bias - 1].index = -1;
			c_problem->x[i][len_with_bias - 1].value = c_bias;
		}
	}
}

static void read_problem
(struct problem * c_problem, VALUE rb_dataset, double c_bias) {
	c_problem->l = RARRAY_LEN(rb_dataset); // l -> number of examples
	c_problem->n = NUM2INT(rb_funcall(rb_dataset, rb_intern("max_dimension"),0));
	// n -> maximum dimension of feature space
	c_problem->bias = c_bias;
	c_problem->x = MALLOC(struct feature_node *, c_problem->l);
	c_problem->y = MALLOC(double, c_problem->l);

	read_rb_examples(c_problem, rb_iv_get(rb_dataset, "@examples"), c_bias);	
}

static void assign_param_value 
(struct parameter * param, VALUE rb_param_key, VALUE rb_param_value) {
	if(StringValueCStr(rb_param_key) == "kernel")
		param->solver_type = NUM2INT(rb_param_value);
	else if(StringValueCStr(rb_param_key) == "c_cost")
		param->C = NUM2DBL(rb_param_value);
	else if(StringValueCStr(rb_param_key) == "p_epsilon")
		param->p = NUM2DBL(rb_param_value);
	else if(StringValueCStr(rb_param_key) == "e_epsilon")
		param->eps = NUM2DBL(rb_param_value);
	else if(StringValueCStr(rb_param_key) == "weights")
		set_param_weights(param, rb_param_value);
}


static void read_parameters
(struct parameter * param, VALUE rb_param_hash) {
	VALUE rb_param_keys = rb_funcall(rb_param_hash, rb_intern("keys"), 0);
	VALUE rb_param_values = rb_funcall(rb_param_hash, rb_intern("values"), 0);
	int i;

	for(i = 0; i < RHASH_SIZE(rb_param_hash); i++)
		assign_param_value(param, rb_ary_entry(rb_param_keys, i), rb_ary_entry(rb_param_values, i));
}

void Init_linear_ext() {
	mLinearScience = rb_define_module("LinearScience");
	cClassifier = rb_define_class_under(mLinearScience, "Classifier", rb_cObject);
	//rb_define_singleton_method(cClassifier, "read_parameters", c_read_parameters, 1);
	id_puts = rb_intern("puts");
	rb_define_global_function("rb_block_call_test", rb_block_call_test, 1);
}